#pragma once
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include <windows.h>

#define PlatformNativePathString std::wstring
#define PlatformFileModeRB L"rb"
#define u64 uint64_t
#define u32 uint32_t
#define u8 uint8_t

u32 modAlpha(u32 color, u8 alpha);
u32 alphaBlend(u32 colora, u32 colorb);

inline int fclose(IStream* s) {
    //destructor already releases this so
    //s->Release();
    return 0;
}
inline int fread(void* buffer, size_t size, size_t count, IStream* s) {
    ULONG bytesRead = 0;
    HRESULT hr = s->Read(buffer, size * count, &bytesRead);
    return bytesRead;
}
inline int fseek(IStream* s, long offset, int origin) {
    LARGE_INTEGER offs{};
    offs.QuadPart = offset;
    s->Seek(offs, 
        origin == SEEK_SET ? STREAM_SEEK_SET :
        origin == SEEK_CUR ? STREAM_SEEK_CUR :
        STREAM_SEEK_END,
        NULL);
    return 0;
}

struct XY {
    int x, y;
};

class Layer {
public:
    u32* pxd;
    u8 layerAlpha = 255;
    bool colorKeySet = false;
    u32 colorKey = 0;
    int w, h;
    bool hidden = false;
    bool isPalettized = false;

    Layer(int wd, int ht) {
        w = wd;
        h = ht;
        pxd = (u32*)malloc(w * h * 4);
    }
    ~Layer() {
        free(pxd);
    }

    u32* pixels32() { return pxd; }

    Layer* copy() {
        Layer* n = new Layer(w, h);
        u32* pp = n->pxd;
        memcpy(pp, pixels32(), w * h * 4);
        *n = *this;
        n->pxd = pp;
        return n;
    }

    static Layer* mergeLayers(Layer* bottom, Layer* top) {
        Layer* ret = new Layer(bottom->w, bottom->h);

        memcpy(ret->pixels32(), bottom->pixels32(), bottom->w * bottom->h * 4);

        if (bottom->isPalettized) {
            ret->isPalettized = true;
            uint32_t* ppx = top->pixels32();
            uint32_t* retppx = ret->pixels32();
            for (uint64_t p = 0; p < ret->w * ret->h; p++) {
                uint32_t pixel = ppx[p];
                uint32_t srcPixel = retppx[p];
                retppx[p] = pixel == -1 ? srcPixel : pixel;
            }
        }
        else {
            uint32_t* ppx = top->pixels32();
            uint32_t* retppx = ret->pixels32();
            for (uint64_t p = 0; p < ret->w * ret->h; p++) {
                uint32_t pixel = ppx[p];
                uint32_t srcPixel = retppx[p];
                pixel = modAlpha(pixel, (uint8_t)(((pixel >> 24) / 255.0f) * (top->layerAlpha / 255.0f) * 255));
                retppx[p] = alphaBlend(srcPixel, pixel);
            }
        }

        return ret;
    }

    void applyPalette(std::vector<u32> pal) {
        if (isPalettized) {
            for (int x = 0; x < w; x++) {
                for (int y = 0; y < h; y++) {
                    u32 pixel = getPixelAt({ x, y });

                    u32 targetColor =
                        pixel == -1 ? 0
                        : pixel < pal.size() ? pal[pixel]
                        : 0;
                    setPixel({ x, y }, targetColor);
                }
            }
        }
    }

    u32 getPixelAt(XY position)
    {
        if (position.x >= 0 && position.x < w
            && position.y >= 0 && position.y < h) {
            uint32_t pixel = pxd[position.x + (position.y * w)];
            return pixel;
        }
        return 0;
    }
    void setPixel(XY position, u32 color)
    {
        if (position.x >= 0 && position.x < w
            && position.y >= 0 && position.y < h) {
            pxd[position.x + (position.y * w)] = color;
        }
    }

    Layer* copyCurrentVariantScaled(XY dimensions)
    {
        bool shouldIntegerScale =
            max(dimensions.x, w) % min(dimensions.x, w) == 0
            && max(dimensions.y, h) % min(dimensions.y, h) == 0;

        Layer* newLayer = NULL;
        /*if (isPalettized) {
            newLayer = LayerPalettized::tryAllocIndexedLayer(dimensions.x, dimensions.y);
            if (newLayer != NULL) {
                ((LayerPalettized*)newLayer)->palette = ((LayerPalettized*)this)->palette;
            }
        }
        else*/ {
            newLayer = new Layer(dimensions.x, dimensions.y);
            if (newLayer != NULL) {
                newLayer->layerAlpha = layerAlpha;
            }
        }

        if (newLayer != NULL) {
            newLayer->colorKey = colorKey;
            newLayer->colorKeySet = colorKeySet;
            newLayer->hidden = hidden;

            if (!shouldIntegerScale) {
                //loginfo(frmt("Performing float-based scale {}x{} -> {}x{}", w, h, dimensions.x, dimensions.y));
                for (int y = 0; y < newLayer->h; y++) {
                    for (int x = 0; x < newLayer->w; x++) {
                        XY samplePoint = {
                            w * (x / (float)newLayer->w),
                            h * (y / (float)newLayer->h)
                        };
                        newLayer->setPixel({ x, y }, getPixelAt(samplePoint));
                    }
                }
            }
            else {
                //loginfo(frmt("Performing integer scale {}x{} -> {}x{}", w, h, dimensions.x, dimensions.y));
                bool downscale = dimensions.x < w || dimensions.y < h;
                int scaleFactorX = max(dimensions.x, w) / min(dimensions.x, w);
                int scaleFactorY = max(dimensions.y, h) / min(dimensions.y, h);

                for (int y = 0; y < newLayer->h; y++) {
                    for (int x = 0; x < newLayer->w; x++) {
                        XY samplePoint =
                            !downscale ? XY{ x / scaleFactorX, y / scaleFactorY }
                        : XY{ x * scaleFactorX, y * scaleFactorY };
                        newLayer->setPixel({ x, y }, getPixelAt(samplePoint));
                    }
                }
            }
        }
        return newLayer;
    }
};

Layer* readVOIDSN(IStream* s);