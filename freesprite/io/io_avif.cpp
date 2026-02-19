#if VSP_USE_LIBAVIF
    #include <avif/avif_cxx.h>
#endif

#include <SDL3_image/SDL_image.h>

#include "io_base.h"
#include "io_avif.h"
#include "../FileIO.h"

std::string getLibAVIFVersion()
{
#if VSP_USE_LIBAVIF
    return frmt("{}.{}.{}", AVIF_VERSION_MAJOR, AVIF_VERSION_MINOR, AVIF_VERSION_PATCH);
#else
    return "<libavif is not used>";
#endif
}

std::vector<u8> writeAVIFToMem(Layer* l, int quality)
{
#if VSP_USE_LIBAVIF
    avifEncoder* encoder = avifEncoderCreate();
    encoder->quality = ixmin(AVIF_QUALITY_BEST, ixmax(AVIF_QUALITY_WORST, quality));
    DoOnReturn destroyEncoder([encoder]() { avifEncoderDestroy(encoder); });

    avifImage* image = avifImageCreate(l->w, l->h, 8, AVIF_PIXEL_FORMAT_YUV420);
    DoOnReturn destroyImage([image]() { avifImageDestroy(image); });

    avifRGBImage rgbImage;
    avifRGBImageSetDefaults(&rgbImage, image);
    rgbImage.format = AVIF_RGB_FORMAT_BGRA;
    rgbImage.pixels = (u8*)l->pixels32();
    rgbImage.rowBytes = 4 * l->w;
    if (avifImageRGBToYUV(image, &rgbImage) == AVIF_RESULT_OK) {
        if (avifEncoderAddImage(encoder, image, 1000, AVIF_ADD_IMAGE_FLAG_NONE) != AVIF_RESULT_OK) {
            logerr(frmt("avifEncoderAddImage failed: {}", encoder->diag.error));
            return {};
        }
    }
    else {
        logerr(frmt("avifImageRGBToYUV failed: {}", encoder->diag.error));
        return {};
    }
    std::vector<u8> output;
    avifRWData outputData{};
    outputData.data = NULL;
    outputData.size = 0;
    DoOnReturn freeOutputData([&outputData]() { avifRWDataFree(&outputData); });
    if (avifEncoderFinish(encoder, &outputData) == AVIF_RESULT_OK) {
        output.resize(outputData.size);
        memcpy(output.data(), outputData.data, outputData.size);
    }
    else {
        logerr(frmt("avifEncoderFinish failed: {}", encoder->diag.error));
        return {};
    }
    return output;
#else
    SDL_Surface* srf = SDL_CreateSurface(l->w, l->h, SDL_PIXELFORMAT_ARGB8888);
    if (srf != NULL) {
        DoOnReturn destroySurface([srf]() { SDL_DestroySurface(srf); });
        SDL_LockSurface(srf);
        for (int y = 0; y < l->h; y++) {
            memcpy(&(ARRAY2DPOINT((u8*)srf->pixels, 0, y, srf->pitch)), &(ARRAY2DPOINT(l->pixels32(), 0, y, l->w)), l->w * 4);
        }
        SDL_UnlockSurface(srf);
        SDLVectorU8IOStream* rawStream = NULL;
        SDL_IOStream* stream = SDLVectorU8IOStream::OpenNew(&rawStream);
        IMG_SaveAVIF_IO(srf, stream, false, quality);
        SDL_DestroySurface(srf);
        SDL_SeekIO(stream, 0, SDL_IO_SEEK_SET);
        std::vector<u8> data = rawStream->data;
        SDL_CloseIO(stream);

        return data;
    }
#endif
    return {};
}

SDL_Surface* readAVIFFromMem(u8* data, size_t dataSize)
{
#if VSP_USE_LIBAVIF
    avifDecoder* avif = avifDecoderCreate();
    DoOnReturn destroyDecoder([avif]() { avifDecoderDestroy(avif); });

    avifImage* image = avifImageCreateEmpty();
    DoOnReturn destroyImage([image]() { avifImageDestroy(image); });
    SDL_Surface* srf = NULL;
    if (avifDecoderReadMemory(avif, image, data, dataSize) == AVIF_RESULT_OK) {
        srf = SDL_CreateSurface(image->width, image->height, SDL_PIXELFORMAT_ARGB8888);
        SDL_LockSurface(srf);
        DoOnReturn unlockSurface([srf]() { SDL_UnlockSurface(srf); });

        avifRGBImage rgbImage;
        avifRGBImageSetDefaults(&rgbImage, avif->image);
        if (avifRGBImageAllocatePixels(&rgbImage) == AVIF_RESULT_OK && avifImageYUVToRGB(avif->image, &rgbImage) == AVIF_RESULT_OK) {
            SDL_ConvertPixels(avif->image->width, avif->image->height, SDL_PIXELFORMAT_ABGR8888, rgbImage.pixels, 4 * avif->image->width, SDL_PIXELFORMAT_ARGB8888, srf->pixels, 4 * avif->image->width);

            avifRGBImageFreePixels(&rgbImage);
        }
        else {
            SDL_DestroySurface(srf);
            srf = NULL;
        }

        return srf;
    }
    return NULL;
#else
    SDL_IOStream* stream = SDL_IOFromMem(data, dataSize);
    SDL_Surface* srf = IMG_LoadAVIF_IO(stream);
    SDL_CloseIO(stream);
    return srf;
#endif
}

MainEditor* readAVIF(PlatformNativePathString path, OperationProgressReport* progress)
{
#if VSP_USE_LIBAVIF
    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {
        DoOnReturn closeFile([f]() { fclose(f); });

        fseek(f, 0, SEEK_END);
        u64 avifSize = ftell(f);
        fseek(f, 0, SEEK_SET);

        progress->enterSection("Reading AVIF...");
        progress->enterSection("Initializing...");

        struct AVIFReadData {
            FILE* f;
            std::vector<u8> data;
        };
        AVIFReadData readBuffer{};
        readBuffer.f = f;

        avifDecoder* avif = avifDecoderCreate();
        DoOnReturn destroyDecoder([avif]() { avifDecoderDestroy(avif); });
        avifIO io{};
        io.data = &readBuffer;
        io.read = [](avifIO* io, uint32_t readFlags, uint64_t offset, size_t size, avifROData* out) {
            AVIFReadData* rd = (AVIFReadData*)io->data;
            FILE* f = rd->f;
            fseek(f, offset, SEEK_SET);
            rd->data.resize(size);
            size_t bytesRead = fread(rd->data.data(), 1, size, f);
            out->size = bytesRead;
            out->data = rd->data.data();
            return bytesRead == size ? AVIF_RESULT_OK : AVIF_RESULT_IO_ERROR;
        };
        io.sizeHint = avifSize;

        avifDecoderSetIO(avif, &io);

        progress->updateLastSection("Parsing...");
        avifResult result = avifDecoderParse(avif);
        if (result != AVIF_RESULT_OK) {
            logerr(frmt("avifDecoderParse failed: {}", avif->diag.error));
            return NULL;
        }

        std::vector<Frame*> frames;
        result = avifDecoderNextImage(avif);
        while (result == AVIF_RESULT_OK) {
            
            progress->updateLastSection(frmt("Frame {}/{}", frames.size()+1, avif->imageCount));
            Layer* l = Layer::tryAllocLayer(avif->image->width, avif->image->height);
            if (l != NULL) {
                avifRGBImage rgbImage;
                avifRGBImageSetDefaults(&rgbImage, avif->image);
                if (avifRGBImageAllocatePixels(&rgbImage) == AVIF_RESULT_OK && avifImageYUVToRGB(avif->image, &rgbImage) == AVIF_RESULT_OK) {
                    SDL_ConvertPixels(avif->image->width, avif->image->height, SDL_PIXELFORMAT_ABGR8888, rgbImage.pixels, 4 * avif->image->width, SDL_PIXELFORMAT_ARGB8888, l->pixels32(), 4 * avif->image->width);

                    avifRGBImageFreePixels(&rgbImage);
                    l->name = "AVIF Layer";
                    Frame* f = new Frame();
                    f->layers = { l };
                    frames.push_back(f);
                }

            }
            else {
                g_addNotificationFromThread(NOTIF_MALLOC_FAIL);
            }
            result = avifDecoderNextImage(avif);
        }

        if (!frames.empty()) {
            MainEditor* ret = new MainEditor(frames);
            ret->frameAnimMSPerFrame = (int)(avif->imageTiming.duration * 1000);
            return ret;
        }
    }
    return NULL;
#else
    Layer* l = readSDLImage(path);
    if (l != NULL) {
        return new MainEditor(l);
    }
    return NULL;
#endif
}

bool writeAVIF(PlatformNativePathString path, MainEditor* editor)
{
#if VSP_USE_LIBAVIF
    FILE* f = platformOpenFile(path, PlatformFileModeWB);
    if (f != NULL) {
        DoOnReturn closeFile([f]() { fclose(f); });

        avifEncoder* encoder = avifEncoderCreate();
        encoder->timescale = 1000;
        encoder->quality = AVIF_QUALITY_LOSSLESS;
        DoOnReturn destroyEncoder([encoder]() { avifEncoderDestroy(encoder); });
        for (Frame* frame : editor->frames) {
            if (frame->layers.empty()) {
                continue;
            }
            Layer* l = editor->flattenFrame(frame);
            if (l != NULL) {
                DoOnReturn destroyLayer([l]() { delete l; });

                //svt-av1 only supports yuv4:2:0
                avifImage* image = avifImageCreate(l->w, l->h, 8, AVIF_PIXEL_FORMAT_YUV420);
                DoOnReturn destroyImage([image]() { avifImageDestroy(image); });

                avifRGBImage rgbImage;
                avifRGBImageSetDefaults(&rgbImage, image);
                rgbImage.format = AVIF_RGB_FORMAT_BGRA;
                rgbImage.pixels = (u8*)l->pixels32();
                rgbImage.rowBytes = 4 * l->w;
                if (avifImageRGBToYUV(image, &rgbImage) == AVIF_RESULT_OK) {
                    if (avifEncoderAddImage(encoder, image, editor->frameAnimMSPerFrame, AVIF_ADD_IMAGE_FLAG_NONE) != AVIF_RESULT_OK) {
                        logerr(frmt("avifEncoderAddImage failed: {}", encoder->diag.error));
                        return false;
                    }
                }
                else {
                    logerr(frmt("avifImageRGBToYUV failed: {}", encoder->diag.error));
                    return false;
                }
            }
        }

        avifRWData output{};
        if (avifEncoderFinish(encoder, &output) == AVIF_RESULT_OK) {
            DoOnReturn freeOutputData([&output]() { avifRWDataFree(&output); });
            fwrite(output.data, 1, output.size, f);
            return true;
        }
        else {
            logerr(frmt("avifEncoderFinish failed: {}", encoder->diag.error));
            return false;
        }
    }
    return false;
#else
    return writeAVIFWithSDLImage(path, editor);
#endif
}

bool writeAVIFWithSDLImage(PlatformNativePathString path, MainEditor* data)
{
    if (data->isPalettized) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Indexed image export not supported"));
        return false;
    }

    Layer* flat = data->flattenImage();
    if (flat == NULL) {
        g_addNotificationFromThread(NOTIF_MALLOC_FAIL);
        return false;
    }
    DoOnReturn destroyLayer([flat]() { delete flat; });

    SDL_Surface* surface = SDL_CreateSurface(flat->w, flat->h, SDL_PIXELFORMAT_ARGB8888);
    if (surface == NULL) {
        logerr(frmt("[AVIF] failed to create surface: {}", SDL_GetError()));
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.mallocfail")));
        return false;
    }
    memcpy(surface->pixels, flat->pixels32(), flat->w * flat->h * 4);

    std::string u8path = convertStringToUTF8OnWin32(path);

    bool ret = IMG_SaveAVIF(surface, u8path.c_str(), 100);

    SDL_FreeSurface(surface);

    return ret;
}