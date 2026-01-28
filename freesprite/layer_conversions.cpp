#include "layer_conversions.h"
#include "Layer.h"
#include "LayerPalettized.h"
#include "Notification.h"

LayerPalettized* to8BitIndexed1BitAlpha(Layer* rgba) {
    std::vector<u32> palette = {{0x00000000}};
    LayerPalettized* ret = LayerPalettized::tryAllocIndexedLayer(rgba->w, rgba->h);
    if (ret == NULL) {
        g_addNotificationFromThread(NOTIF_MALLOC_FAIL);
        return NULL;
    }
    u32* pixels = rgba->pixels32();
    u32* retPixels = ret->pixels32();
    for (u64 px = 0; px < (u64)rgba->w * rgba->h; px++) {
        u32 ppx = pixels[px];
        SDL_Color c = uint32ToSDLColor(ppx);
        ppx = c.a > 0x7f ? modAlpha(ppx, 255) : 0;
        auto index = std::find(palette.begin(), palette.end(), ppx);
        if (index != palette.end()) {
            int ii = index - palette.begin();
            retPixels[px] = ii;
        } else {
            palette.push_back(ppx);
            int ii = palette.size() - 1;
            retPixels[px] = ii;
            if (palette.size() > 256) {
                delete ret;
                g_addNotificationFromThread(ErrorNotification(TL("vsp.cmn.error"), "Too many colors"));
                return NULL;
            }
        }
    }
    ret->palette = palette;
    return ret;
}