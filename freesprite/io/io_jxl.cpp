#include "io_base.h"
#include "io_jxl.h"

#if VOIDSPRITE_JXL_ENABLED

#include <jxl/encode_cxx.h>
#include <jxl/decode_cxx.h>

std::string getlibjxlVersion()
{
    return std::to_string(JPEGXL_MAJOR_VERSION) + "." + std::to_string(JPEGXL_MINOR_VERSION) +
        "." + std::to_string(JPEGXL_PATCH_VERSION);
}

MainEditor* readJpegXL(PlatformNativePathString path, OperationProgressReport* progress)
{
    ENSURE_REPORT_VALID(progress);
    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {
        DoOnReturn closeFile([f]() { fclose(f); });

        //full disclosure
        //this format sucks ass
        //i wasted half a day writing an encoder function and all i got were vague errors that told me nothing

        progress->enterSection("Initializing...");

        fseek(f, 0, SEEK_END);
        u64 jxlSize = ftell(f);
        fseek(f, 0, SEEK_SET);

        u8* jxlData = (u8*)tracked_malloc(jxlSize);
        if (jxlData == NULL) {
            g_addNotificationFromThread(NOTIF_MALLOC_FAIL);
            return NULL;
        }
        DoOnReturn freeJxlData([jxlData]() { tracked_free(jxlData); });
        fread(jxlData, 1, jxlSize, f);

        JxlDecoderPtr decoder = JxlDecoderMake(NULL);
        if (decoder == NULL) {
            logerr("JxlDecoderMake failed");
            return NULL;
        }

        if (JxlDecoderSetInput(decoder.get(), jxlData, jxlSize) != JXL_DEC_SUCCESS) {
            logerr("JxlDecoderSetInput failed");
            return NULL;
        }

        JxlPixelFormat pixel_format = { 4, JXL_TYPE_UINT8, JXL_LITTLE_ENDIAN, 0 }; // RGBA
        if (JxlDecoderSubscribeEvents(decoder.get(), JXL_DEC_BASIC_INFO | JXL_DEC_FULL_IMAGE | JXL_DEC_FRAME) != JXL_DEC_SUCCESS) {
            logerr("JxlDecoderSubscribeEvents failed");
            return NULL;
        }

        JxlBasicInfo basic_info;
        std::vector<u8*> pixelDatas;
        DoOnReturn freePixelDatas([&pixelDatas]() {
            for (auto& p : pixelDatas) {
                tracked_free(p);
            }
        });
        int w = -1, h = -1;
        double ticksPerSecond = 0;
        int sumAnimTicks = 0;
        int numAnimTicks = 0;
        int framesDecoded = 0;

        progress->updateLastSection("Decoding JXL...");
        progress->enterSection("Reading frame");
        bool decoding = true;
        while (decoding) {
            JxlDecoderStatus status = JxlDecoderProcessInput(decoder.get());
            JxlFrameHeader frame_header{};

            switch (status) {
                case JXL_DEC_SUCCESS:
                    decoding = false;
                    break;
                case JXL_DEC_FULL_IMAGE:
                    //loginfo(frmt("Frame {} decoded", framesDecoded));
                    progress->updateLastSection(frmt("{} loaded frames", ++framesDecoded));
                    break;
                case JXL_DEC_FRAME:
                    if (JxlDecoderGetFrameHeader(decoder.get(), &frame_header) == JXL_DEC_SUCCESS) {
                        sumAnimTicks += frame_header.duration;
                        numAnimTicks++;
                    }
                    break;
                case JXL_DEC_ERROR:
                    logerr("JXL_DEC_ERROR");
                    return NULL;
                case JXL_DEC_NEED_MORE_INPUT:
                    logerr("JXL_DEC_NEED_MODE_INPUT (file either corrupted or truncated)");
                    return NULL;
                case JXL_DEC_BASIC_INFO:
                    if (JxlDecoderGetBasicInfo(decoder.get(), &basic_info) == JXL_DEC_SUCCESS){
                        w = basic_info.xsize;
                        h = basic_info.ysize;
                        ticksPerSecond = basic_info.have_animation && basic_info.animation.tps_denominator != 0
                            ? (double)basic_info.animation.tps_numerator / basic_info.animation.tps_denominator
                            : ticksPerSecond;
                    } else 
                    {
                        logerr("JxlDecoderGetBasicInfo failed");
                        return NULL;
                    }
                    break;
                case JXL_DEC_NEED_IMAGE_OUT_BUFFER:
                    {
                        size_t buffer_size = 0;
                        if (JxlDecoderImageOutBufferSize(decoder.get(), &pixel_format, &buffer_size) != JXL_DEC_SUCCESS) {
                            logerr("JxlDecoderImageOutBufferSize failed");
                            return NULL;
                        }

                        u8* newPixelData = (u8*)tracked_malloc(buffer_size);
                        if (newPixelData == NULL) {
                            g_addNotificationFromThread(NOTIF_MALLOC_FAIL);
                            return NULL;
                        }
                        pixelDatas.push_back(newPixelData);

                        if (JxlDecoderSetImageOutBuffer(decoder.get(), &pixel_format, newPixelData, buffer_size) != JXL_DEC_SUCCESS) {
                            logerr("JxlDecoderSetImageOutBuffer failed");
                            return NULL;
                        }
                    }
                    break;
                default:
                    logwarn(frmt("unhandled jxldecoderstatus {}", (int)status));
                    break;
            }
        }

        //line below crashes too btw
        //this gets automatically called apparently?
        //JxlDecoderDestroy(decoder.get());
        MainEditor* ret = NULL;
        std::vector<Frame*> frames;
        for (auto*& pixelData : pixelDatas) {
            Layer* l = NULL;
            if (w > 0 && h > 0 && pixelData != NULL) {
                l = Layer::tryAllocLayer(w, h);
                if (l != NULL) {
                    Frame* f = new Frame();
                    f->layers = { l };
                    frames.push_back(f);
                    l->name = TL("vsp.layer.jxl");
                    SDL_ConvertPixels(w, h, SDL_PIXELFORMAT_ABGR8888, pixelData, 4 * w, SDL_PIXELFORMAT_ARGB8888, l->pixels32(), 4 * w);
                }
            }
        }
        if (!frames.empty()) {
            ret = new MainEditor(frames);
            double avgTicksPerFrame = numAnimTicks > 0 ? ((double)sumAnimTicks / numAnimTicks) : 0;
            double msPerTick = ticksPerSecond > 0 ? (1000.0 / ticksPerSecond) : 0;
            int msPerFrame = (int)(avgTicksPerFrame * msPerTick);
            ret->frameAnimMSPerFrame = msPerFrame;
        }
        return ret;
    }
    return NULL;
}

bool writeJpegXL(PlatformNativePathString path, MainEditor* data)
{
    FILE* f = platformOpenFile(path, PlatformFileModeWB);
    if (f != NULL) {

        DoOnReturn closeFile([f]() { fclose(f); });

        u8* abgrPixelBuffer = (u8*)tracked_malloc(data->canvas.dimensions.x * data->canvas.dimensions.y * 4);
        if (abgrPixelBuffer == NULL) {
            g_addNotification(NOTIF_MALLOC_FAIL);
            return false;
        }
        DoOnReturn freeAbgrPixels([abgrPixelBuffer]() { tracked_free(abgrPixelBuffer); });

        JxlEncoderPtr encoder = JxlEncoderMake(NULL);

        JxlColorEncoding color_encoding{};
        JxlColorEncodingSetToSRGB(&color_encoding, false);

        JxlBasicInfo basic_info;
        JxlEncoderInitBasicInfo(&basic_info);
        basic_info.xsize = data->canvas.dimensions.x;
        basic_info.ysize = data->canvas.dimensions.y;
        basic_info.bits_per_sample = 8;
        basic_info.num_color_channels = 3;
        basic_info.num_extra_channels = 1;
        basic_info.uses_original_profile = JXL_FALSE;
        if (data->frames.size() > 1) {
            basic_info.have_animation = JXL_TRUE;
            basic_info.animation.tps_numerator = 1000;
            basic_info.animation.tps_denominator = 1;
            //if you uncomment this process_result will start giving you JXL_ENC_ERR_GENERIC
            //shoutouts to the useless documentation

            /*basic_info.animation.num_loops = 0;
            basic_info.animation.have_timecodes = JXL_FALSE;
            //todo figure out what the values below mean
            */
        }
        JxlEncoderSetBasicInfo(encoder.get(), &basic_info);

        JxlEncoderFrameSettings* frame_settings = JxlEncoderFrameSettingsCreate(encoder.get(), NULL);
        JxlEncoderSetFrameDistance(frame_settings, 0.0f);
        JxlEncoderSetFrameLossless(frame_settings, JXL_TRUE);

        JxlPixelFormat pixel_format = { 4, JXL_TYPE_UINT8, JXL_LITTLE_ENDIAN, 0 };

        for (Frame*& f : data->frames) {
            Layer* flat = data->flattenFrame(f);
            if (flat != NULL) {
                SDL_ConvertPixels(flat->w, flat->h, SDL_PIXELFORMAT_ARGB8888,
                    flat->pixels32(), flat->w * 4,
                    SDL_PIXELFORMAT_ABGR8888, abgrPixelBuffer, flat->w * 4);

                JxlFrameHeader frameHeader;
                JxlEncoderInitFrameHeader(&frameHeader);
                frameHeader.duration = data->frameAnimMSPerFrame;
                frameHeader.timecode = 0;
                JxlEncoderSetFrameHeader(frame_settings, &frameHeader);

                int addImageResult = JxlEncoderAddImageFrame(frame_settings, &pixel_format, abgrPixelBuffer, flat->w * flat->h * 4);
                delete flat;
                if (addImageResult != JXL_ENC_SUCCESS) {
                    logerr(frmt("JxlEncoderAddImageFrame failed ({})", addImageResult));
                    return false;
                }
            }
            else {
                g_addNotificationFromThread(NOTIF_MALLOC_FAIL);
            }
        }

        JxlEncoderCloseInput(encoder.get());

        std::vector<u8> outputData;
        int chunkSize = 64;
        outputData.resize(chunkSize);
        u8* next_out = outputData.data();
        size_t avail_out = outputData.size();
        JxlEncoderStatus process_result = JXL_ENC_NEED_MORE_OUTPUT;
        while (process_result == JXL_ENC_NEED_MORE_OUTPUT) {
            process_result = JxlEncoderProcessOutput(encoder.get(), &next_out, &avail_out);
            if (process_result == JXL_ENC_NEED_MORE_OUTPUT) {
                chunkSize *= 2;
                size_t offset = next_out - outputData.data();
                outputData.resize(outputData.size() + chunkSize);
                next_out = outputData.data() + offset;
                avail_out = chunkSize;
            }
        }
        outputData.resize(next_out - outputData.data());
        //THE LINE BELOW CRASHES BY THE WAY
        //JxlEncoderDestroy(encoder);

        if (process_result == JXL_ENC_SUCCESS) {
            fwrite(outputData.data(), outputData.size(), 1, f);
            return true;
        }
        else {
            logerr(frmt("JxlEncoderProcessOutput failed ({})", (int)process_result));
            if (process_result == JXL_ENC_ERROR) {
                logerr(frmt("  error: {}", (int)JxlEncoderGetError(encoder.get())));
            }
        }
        return false;
    }
    return false;
}

#endif