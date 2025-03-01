#include "Pattern.h"
#include "globals.h"
#include "Notification.h"
#include "LayerPalettized.h"
#include "FileIO.h"

void Pattern::tryLoadIcon()
{
    cachedIcon = IMGLoadToTexture(getIconPath());
}

CustomPattern* CustomPattern::load(PlatformNativePathString path)
{
    LayerPalettized* loadImage = NULL;
    if (stringEndsWithIgnoreCase(path, convertStringOnWin32(".pbm"))) {
        loadImage = (LayerPalettized*)readAnymapPBM(path);
    }
    else if (stringEndsWithIgnoreCase(path, convertStringOnWin32(".xbm"))) {
        loadImage = (LayerPalettized*)readXBM(path);
    }
    
    if (loadImage != NULL) {
        if (loadImage->w != 0 && loadImage->h != 0) {
            CustomPattern* ret = new CustomPattern(loadImage);
            PlatformNativePathString fileNameWithNoPathAndExtension = path.substr(path.find_last_of(convertStringOnWin32("/\\")) + 1);
            ret->name = convertStringToUTF8OnWin32(fileNameWithNoPathAndExtension.substr(0, fileNameWithNoPathAndExtension.find_last_of(convertStringOnWin32("."))));
            delete loadImage;
            return ret;
        }
    }
    g_addNotification(ErrorNotification("Error", "Can't load: " + convertStringToUTF8OnWin32(path)));
    std::string err = "Can't load: " + convertStringToUTF8OnWin32(path);
    printf("%s\n", err.c_str());
    return NULL;
}

CustomPattern::CustomPattern(LayerPalettized* from)
{
    if (from != NULL) {
        bitmap = (uint8_t*)tracked_malloc(from->w * from->h, "Patterns");
        for (uint64_t p = 0; p < from->w * from->h; p++) {
            bitmap[p] = ((uint32_t*)from->pixelData)[p] == 0 ? 0 : 1;
        }
        bitmapDimensions = XY{ from->w, from->h };
    }
}

bool CustomPattern::canDrawAt(XY position)
{
    if (bitmap != NULL) {
        if (position.x >= 0 && position.y >= 0) {
            return bitmap[(position.x % bitmapDimensions.x) + (position.y % bitmapDimensions.y) * bitmapDimensions.x] == 1;
        }
    }
    return true;
}

void CustomPattern::tryLoadIcon()
{
    if (bitmap != NULL) {
        cachedIcon = tracked_createTexture(g_rd, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 22, 22);
        int bitmapPitch;
        uint32_t* iconBitmap;
        SDL_LockTexture(cachedIcon, NULL, (void**)&iconBitmap, &bitmapPitch);
        for (int y = 0; y < 22; y++) {
            for (int x = 0; x < 22; x++) {
                iconBitmap[x + y * 22] = canDrawAt({x/2,y/2}) ? 0xffffffff : 0x00000000;
            }
        }
        SDL_UnlockTexture(cachedIcon);
    }
}
