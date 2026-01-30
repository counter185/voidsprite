#include "layer_conversions.h"
#include "Layer.h"
#include "LayerPalettized.h"
#include "Notification.h"

bool hasTransparency(Layer* rgba, u8 threshold) {
    u32* pixels = rgba->pixels32();
    for (u64 px = 0; px < (u64)rgba->w * rgba->h; px++) {
        u32 ppx = pixels[px];
        SDL_Color c = uint32ToSDLColor(ppx);
        if (c.a < threshold) {
            return true;
        }
    }
    return false;
}

LayerOpResult to8BitIndexed1BitAlpha(Layer* rgba) {
    bool layerHasTransparency = hasTransparency(rgba, 128);

    std::vector<u32> palette = layerHasTransparency ? std::vector<u32>{0x00000000} : std::vector<u32>{};

    LayerPalettized* ret = LayerPalettized::tryAllocIndexedLayer(rgba->w, rgba->h);
    if (ret == NULL) {
        g_addNotificationFromThread(NOTIF_MALLOC_FAIL);
        return {false};
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
                return {false};
            }
        }
    }
    ret->palette = palette;
    return { 
        .success = true, 
        .outLayer = ret, 
        .alphaIndex = layerHasTransparency ? 0 : -1
    };
}

LayerOpResult to8BitIndexedWith1BitSingleIndexAlpha(LayerPalettized* idx)
{
    std::vector<u32> palette = idx->palette;
    auto findAlphaIndex = std::find_if(palette.begin(), palette.end(), [](u32 c) {
        u8 alpha = uint32ToSDLColor(c).a;
        return alpha == 0;
    });
    int alphaIndex = findAlphaIndex != palette.end() ? (findAlphaIndex - palette.begin()) : -1;

    //build index remap
    std::map<int, u8> indexRemap;
    for (int x = 0; x < palette.size(); x++) {
        SDL_Color color = uint32ToSDLColor(palette[x]);
        if (x == alphaIndex) {
            indexRemap[x] = x;
        }
        else if (color.a == 255) {
            indexRemap[x] = x;
        }
        else if (color.a < 0x7f) {
            if (alphaIndex == -1) {
                if (palette.size() < 256) {
                    palette.push_back(0x00000000);
                    alphaIndex = palette.size() - 1;
                }
                else {
                    g_addNotificationFromThread(ErrorNotification(TL("vsp.cmn.error"), "Too many colors for transparent index."));
                    return { false };
                }
            }
            indexRemap[x] = alphaIndex;
        }
        else if (color.a >= 0x7f) {
            palette[x] = modAlpha(palette[x], 255);
            indexRemap[x] = x;
        }
    }

    LayerPalettized* ret = LayerPalettized::tryAllocIndexedLayer(idx->w, idx->h);
    if (ret != NULL) {
        s32* srcPixels = (s32*)idx->pixels32();
        u32* dstPixels = ret->pixels32();
        for (u64 px = 0; px < (u64)idx->w * idx->h; px++) {
            s32 srcIndex = srcPixels[px];
            if (indexRemap.contains(srcIndex)) {
                dstPixels[px] = indexRemap[srcIndex];
            }
            else if (srcIndex == -1) {
                if (alphaIndex == -1) {
                    if (palette.size() < 256) {
                        palette.push_back(0x00000000);
                        alphaIndex = palette.size() - 1;
                    }
                    else {
                        g_addNotificationFromThread(ErrorNotification(TL("vsp.cmn.error"), "Too many colors for transparent index."));
                        delete ret;
                        return { false };
                    }
                }
                dstPixels[px] = alphaIndex;
            }
            else {
                //???
                dstPixels[px] = 0;
            }
        }

        ret->palette = palette;
        return { true, ret, alphaIndex };
    }
    else {
        return { false };
    }
}
