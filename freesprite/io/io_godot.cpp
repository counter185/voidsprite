#include <SDL3_image/SDL_image.h>

#include "io_base.h"
#include "io_godot.h"
#include "io_png.h"

Layer* readGodotCTEX(PlatformNativePathString path, u64 seek) {
    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {
        std::vector<u8> searchBuffer;
        searchBuffer.resize(0x50);

        fseek(f, 0, SEEK_END);
        u64 fileSize = ftell(f);
        fseek(f, 0, SEEK_SET);

        fread(searchBuffer.data(), 1, 0x50, f);
        //find RIFF (webp) or PNG (png)

        std::vector<u8> riffHeader = { 'R', 'I', 'F', 'F' };
        std::vector<u8> pngHeader = { 0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A };

        Layer* ret = NULL;

        auto searchRiff = std::search(searchBuffer.begin(), searchBuffer.end(), riffHeader.begin(), riffHeader.end());
        if (searchRiff != searchBuffer.end()) {
            u64 offset = std::distance(searchBuffer.begin(), searchRiff);
            fseek(f, offset, SEEK_SET);

            u8* bytes = (u8*)tracked_malloc(fileSize - offset);
            if (bytes != NULL) {
                fread(bytes, 1, fileSize - offset, f);

                SDL_IOStream* io = SDL_IOFromConstMem(bytes, fileSize - offset);
                SDL_Surface* srf = IMG_LoadWEBP_IO(io);
                tracked_free(bytes);
                SDL_CloseIO(io);

                if (srf != NULL) {
                    ret = Layer::createFromSurface(srf);
                    SDL_FreeSurface(srf);
                }
            }
        }
        else {
            auto searchPNG = std::search(searchBuffer.begin(), searchBuffer.end(), pngHeader.begin(), pngHeader.end());
            if (searchPNG != searchBuffer.end()) {
                u64 offset = std::distance(searchBuffer.begin(), searchPNG);
                fseek(f, offset, SEEK_SET);

                u8* bytes = (u8*)tracked_malloc(fileSize - offset);
                if (bytes != NULL) {
                    fread(bytes, 1, fileSize - offset, f);

                    ret = readPNGFromMem(bytes, fileSize - offset);
                    tracked_free(bytes);
                }
            }
        }
        fclose(f);
        return ret;

    }
    return NULL;
}