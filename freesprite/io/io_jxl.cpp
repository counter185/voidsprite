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

Layer* readJpegXL(PlatformNativePathString path, u64 seek)
{
    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {

        //full disclosure
        //this format sucks ass
        //i wasted half a day writing an encoder function and all i got were vague errors that told me nothing
        //so i let chatgpt write most of enc/dec instead and it actually works so i'm not touching it

        // Read the file into a buffer
        fseek(f, 0, SEEK_END);
        u64 jxlSize = ftell(f);
        fseek(f, 0, SEEK_SET);

        u8* jxlData = (u8*)tracked_malloc(jxlSize);
        if (jxlData == NULL) {
            g_addNotification(NOTIF_MALLOC_FAIL);
            fclose(f);
            return NULL;
        }
        fread(jxlData, 1, jxlSize, f);

        // Create a JxlDecoder instance
        JxlDecoderPtr decoder = JxlDecoderMake(nullptr);
        if (!decoder) {
            throw std::runtime_error("Failed to create JxlDecoder.");
        }

        // Set input data
        if (JxlDecoderSetInput(decoder.get(), jxlData, jxlSize) != JXL_DEC_SUCCESS) {
            throw std::runtime_error("Failed to set input data for JxlDecoder.");
        }

        // Set the decoder to output pixel data in RGBA format
        JxlPixelFormat pixel_format = { 4, JXL_TYPE_UINT8, JXL_LITTLE_ENDIAN, 0 }; // RGBA
        if (JxlDecoderSubscribeEvents(decoder.get(), JXL_DEC_BASIC_INFO | JXL_DEC_FULL_IMAGE) != JXL_DEC_SUCCESS) {
            throw std::runtime_error("Failed to subscribe to JxlDecoder events.");
        }

        // Decode the image
        JxlBasicInfo basic_info;
        u8* pixelData = NULL;
        int w = -1, h = -1;


        while (true) {
            JxlDecoderStatus status = JxlDecoderProcessInput(decoder.get());

            if (status == JXL_DEC_BASIC_INFO) {
                // Get basic info about the image
                if (JxlDecoderGetBasicInfo(decoder.get(), &basic_info) != JXL_DEC_SUCCESS) {
                    throw std::runtime_error("Failed to get basic info from JxlDecoder.");
                }

                w = basic_info.xsize;
                h = basic_info.ysize;

                // Allocate buffer for pixel data
                size_t buffer_size = w * h * 4; // RGBA
                pixelData = (u8*)tracked_malloc(buffer_size);
            }
            else if (status == JXL_DEC_NEED_IMAGE_OUT_BUFFER) {
                // Set the output buffer for pixel data
                size_t buffer_size;
                if (JxlDecoderImageOutBufferSize(decoder.get(), &pixel_format, &buffer_size) != JXL_DEC_SUCCESS) {
                    throw std::runtime_error("Failed to get image out buffer size.");
                }

                if (JxlDecoderSetImageOutBuffer(decoder.get(), &pixel_format, pixelData, buffer_size) != JXL_DEC_SUCCESS) {
                    throw std::runtime_error("Failed to set output buffer for pixel data.");
                }
            }
            else if (status == JXL_DEC_FULL_IMAGE) {
                // Full image is decoded
                break;
            }
            else if (status == JXL_DEC_SUCCESS) {
                // Decoding is complete
                break;
            }
            else if (status == JXL_DEC_ERROR) {
                throw std::runtime_error("Error during JXL decoding.");
            }
        }

        //line below crashes too btw
        //JxlDecoderDestroy(decoder.get());
        Layer* ret = NULL;
        if (w > 0 && h > 0 && pixelData != NULL) {
            ret = new Layer(w, h);
            ret->name = TL("vsp.layer.jxl");
            SDL_ConvertPixels(w, h, SDL_PIXELFORMAT_ABGR8888, pixelData, 4 * w, SDL_PIXELFORMAT_ARGB8888, ret->pixelData, 4 * w);
        }
        tracked_free(pixelData);
        fclose(f);
        return ret;
    }
    return NULL;
}

bool writeJpegXL(PlatformNativePathString path, Layer* data)
{
    FILE* f = platformOpenFile(path, PlatformFileModeWB);
    if (f != NULL) {

        u8* abgrPixels = (u8*)tracked_malloc(data->w * data->h * 4);
        if (abgrPixels == NULL) {
            fclose(f);
            g_addNotification(NOTIF_MALLOC_FAIL);
            return false;
        }
        SDL_ConvertPixels(data->w, data->h, SDL_PIXELFORMAT_ARGB8888,
            data->pixelData, data->w * 4,
            SDL_PIXELFORMAT_ABGR8888, abgrPixels, data->w * 4);

        //below code is raw fucking paste from chatgpt because i gave up

        // Initialize the JxlEncoder.
        JxlEncoderPtr encoder = JxlEncoderMake(nullptr);

        // Initialize a memory buffer to store encoded data.
        JxlEncoderFrameSettings* frame_settings = JxlEncoderFrameSettingsCreate(encoder.get(), nullptr);

        // Define the color encoding (sRGB).
        JxlColorEncoding color_encoding;
        JxlColorEncodingSetToSRGB(&color_encoding, /*is_gray=*/false);

        // Set basic image metadata.
        JxlBasicInfo basic_info;
        JxlEncoderInitBasicInfo(&basic_info);
        basic_info.xsize = data->w;
        basic_info.ysize = data->h;
        basic_info.bits_per_sample = 8;
        basic_info.num_color_channels = 3;
        basic_info.num_extra_channels = 1;
        basic_info.uses_original_profile = JXL_FALSE;
        JxlEncoderSetBasicInfo(encoder.get(), &basic_info);

        JxlEncoderSetFrameDistance(frame_settings, 0);

        // Set the pixel data format.
        JxlPixelFormat pixel_format = { 4, JXL_TYPE_UINT8, JXL_LITTLE_ENDIAN, 0 }; // RGBA

        // Add the image data to the encoder.
        int addImageResult = JxlEncoderAddImageFrame(frame_settings, &pixel_format, abgrPixels, static_cast<size_t>(data->w * data->h * 4));

        // Finalize the frame.
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

        bool jxlCompressResult = process_result == JXL_ENC_SUCCESS;
        if (jxlCompressResult) {
            fwrite(outputData.data(), outputData.size(), 1, f);
        }
        tracked_free(abgrPixels);
        fclose(f);
        return jxlCompressResult;
    }
    return false;
}

#endif