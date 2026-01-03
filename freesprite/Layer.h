#pragma once
#include "globals.h"
#include "mathops.h"
#include "background_operation.h"

struct LayerVariant {
    std::string name = "Variant";
    u8* pixelData = NULL;
};

struct LayerPerRendererData {
    SDL_Texture* tex = NULL;
    XY texDimensions = {-1,-1};
    bool layerDirty = true;
};

struct LayerUndoData {
    int variantIndex = 0;
    u8* data;
};

struct LayerScaleData {
    bool success = false;
    XY newSize;
    std::vector<LayerVariant> scaledVariants;
};

class Layer
{
protected:
    Layer() {}
public:
    bool isPalettized = false;

    std::map<std::string, std::string> importExportExtdata;

    std::vector<LayerVariant> layerData;
    int currentLayerVariant = 0;

    int w = -1, h = -1;
    bool bgOpFinished = false;

    //please clean this up some day
    std::map<SDL_Renderer*, LayerPerRendererData> renderData;

    std::string name = "Layer";
    bool hidden = false;

    bool colorKeySet = false;
    uint32_t colorKey = 0;
    uint8_t lastConfirmedlayerAlpha = 255;
    uint8_t layerAlpha = 255;

    SDL_Texture* effectPreviewTexture = NULL;

    Layer(int width, int height) {
        w = width;
        h = height;
        allocMemory();
    }
    Layer(int width, int height, std::vector<LayerVariant> variants) {
        w = width;
        h = height;
        layerData = variants;
    }

    virtual ~Layer() {
        for (auto& variant : layerData) {
            tracked_free(variant.pixelData);
        }
        if (!renderData.empty()) {
            auto renderDataCopy = renderData;
            g_startNewMainThreadOperation([renderDataCopy]() {
                for (auto& [rd, t] : renderDataCopy) {
                    tracked_destroyTexture(t.tex);
                }
            });
        }
    }

    static Layer* createFromSurface(SDL_Surface* from);

    /// <summary>
    /// Tries to allocate a new layer. If either memory allocation or texture creation fails, frees it and returns NULL.
    /// </summary>
    /// <param name="width"></param>
    /// <param name="height"></param>
    /// <returns>Valid Layer* on success, NULL on failure.</returns>
    static Layer* tryAllocLayer(int width, int height) {
        if (width > 0 && height > 0) {
            Layer* ret = new Layer();
            ret->w = width;
            ret->h = height;
            if (ret->allocMemory()) {
                return ret;
            }
            else {
                delete ret;
                return NULL;
            }
        }
        return NULL;
    }

    bool allocMemory() {
        return newLayerVariant();
    }

    virtual LayerVariant allocNewVariant() {
        u64 size = w * h * 4ull;
        LayerVariant newVariant;
        newVariant.name = "Variant";
        newVariant.pixelData = (uint8_t*)tracked_malloc(size, "Layers");
        if (newVariant.pixelData != NULL) {
            memset(newVariant.pixelData, 0, size);
        }
        return newVariant;
    }

    bool newLayerVariant() {
        LayerVariant newVariant = allocNewVariant();
        if (newVariant.pixelData != NULL) {
            layerData.push_back(newVariant);
            currentLayerVariant = layerData.size() - 1;
        }
        return newVariant.pixelData != NULL;
    }
    bool duplicateVariant(int index) {
        if (layerData.size() > index) {
            LayerVariant& copyTarget = layerData[index];
            LayerVariant newVariant = allocNewVariant();
            if (newVariant.pixelData != NULL) {
                memcpy(newVariant.pixelData, copyTarget.pixelData, w * h * 4);
                newVariant.name = copyTarget.name + " copy";
                layerData.push_back(newVariant);
                currentLayerVariant = layerData.size() - 1;
                markLayerDirty();
            }
            return newVariant.pixelData != NULL;
        }
        return false;
    }
    bool duplicateVariant() {
        duplicateVariant(currentLayerVariant);
    }
    bool switchVariant(int variantIndex) {
        if (layerData.size() > variantIndex && variantIndex >= 0) {
            currentLayerVariant = variantIndex;
            markLayerDirty();
            return true;
        }
        return false;
    }

    std::vector<LayerVariant> copyAllVariants() {
        std::vector<LayerVariant> ret;
        for (auto& v : layerData) {
            LayerVariant newVariant;
            newVariant.name = v.name;
            newVariant.pixelData = (uint8_t*)tracked_malloc(w * h * 4, "Layers");
            if (newVariant.pixelData != NULL) {
                memcpy(newVariant.pixelData, v.pixelData, w * h * 4);
                ret.push_back(newVariant);
            }
        }
        return ret;
    }

    void setLayerData(std::vector<LayerVariant> data, XY dimensions);

    void markLayerDirty() {
        for (auto& [rd, dirty] : renderData) {
            renderData[rd].layerDirty = true;
        }
    }

    virtual void updateTexture() {
        uint8_t* pixels;
        u32 pitch;
        if (renderData[g_rd].tex == NULL || !xyEqual(renderData[g_rd].texDimensions, XY{w,h})) {
            if (renderData[g_rd].tex != NULL) {
                tracked_destroyTexture(renderData[g_rd].tex);
            }
            renderData[g_rd].tex = tracked_createTexture(g_rd, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);
            renderData[g_rd].texDimensions = XY{ w,h };
            renderData[g_rd].layerDirty = true;
            SDL_SetTextureBlendMode(renderData[g_rd].tex, SDL_BLENDMODE_BLEND);
        }
        if (SDL_LockTexture(renderData[g_rd].tex, NULL, (void**)&pixels, (int*)&pitch)) {
            copyPixelsToTexture(pixels32(), w, h, pixels, pitch);

            //todo respect the pitch in the below too
            if (colorKeySet) {
                u32* px32 = (u32*)pixels32();
                for (int y = 0; y < h; y++) {
                    for (int x = 0; x < w; x++) {
                        if ((ARRAY2DPOINT(px32, x, y, w) & 0xffffff) == (colorKey & 0xFFFFFF)) {
                            ARRAY2DPOINT(pixels, x * 4 + 3, y, pitch) = 0;
                        }
                    }
                }
            }
            SDL_UnlockTexture(renderData[g_rd].tex);
        }
        else {
            logerr("failed to update layer texture pixels (texture is likely null)");
        }
        renderData[g_rd].layerDirty = false;
    }

    void prerender() {
        if (!renderData.contains(g_rd) || renderData[g_rd].tex == NULL || renderData[g_rd].layerDirty) {
            updateTexture();
        }
        if (bgOpFinished) {
            updateTexture();
            bgOpFinished = false;
        }
    }

    void render(SDL_Rect where, uint8_t alpha = 255) {
        SDL_Texture* target = effectPreviewTexture ? effectPreviewTexture : renderData[g_rd].tex;
        prerender();
        SDL_SetTextureAlphaMod(target, alpha);
        SDL_RenderCopy(g_rd, target, NULL, &where);
    }

    void render(SDL_Rect where, SDL_Rect clip, uint8_t alpha = 255) {
        prerender();
        SDL_SetTextureAlphaMod(renderData[g_rd].tex, alpha);
        SDL_RenderCopy(g_rd, renderData[g_rd].tex, &clip, &where);
    }

    SDL_Texture* renderToTexture() {
        SDL_Texture* ret = tracked_createTexture(g_rd, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);
        SDL_SetTextureBlendMode(ret, SDL_BLENDMODE_BLEND);
        uint8_t* pixels;
        int pitch;
        SDL_LockTexture(ret, NULL, (void**)&pixels, &pitch);
        if (!isPalettized) {
            memcpy(pixels, pixels32(), w * h * 4);
        }
        else {
            uint32_t* px32 = (uint32_t*)pixels;
            for (int y = 0; y < h; y++) {
                for (int x = 0; x < w; x++) {
                    ARRAY2DPOINT(px32, x,y, pitch/4) = getVisualPixelAt(XY{ x,y });
                }
            }
        }
        SDL_UnlockTexture(ret);

        return ret;
    }

    void blit(Layer* sourceLayer, XY position);
    void blit(Layer* sourceLayer, XY position, SDL_Rect clip, bool fast = false);
    void blitTile(Layer* sourceLayer, XY sourceTile, XY dstTile, XY tileSize);

    void setPixel(XY position, u32 color, LayerVariant* targetVariant = NULL) {
        u32* intpxdata = targetVariant == NULL ? pixels32() : (u32*)targetVariant->pixelData;
        if (position.x >= 0 && position.x < w
            && position.y >= 0 && position.y < h) {
            intpxdata[position.x + (position.y * w)] = color;
            markLayerDirty();
        }
    }
    void fillRect(XY from, XY to, uint32_t color) {
        int minx = ixmin(from.x, to.x);
        int maxx = ixmax(from.x, to.x);
        int miny = ixmin(from.y, to.y);
        int maxy = ixmax(from.y, to.y);

        for (int x = minx; x <= maxx; x++) {
            for (int y = miny; y <= maxy; y++) {
                setPixel(XY{ x,y }, color);
            }
        }
    }
    void drawLine(XY from, XY to, uint32_t color) {
        rasterizeLine(from, to, [this, color](XY p) { setPixel(p, color); });
    }
    void drawRect(XY from, XY to, uint32_t color) {
        drawLine(from, XY{ to.x, from.y }, color);
        drawLine(from, XY{ from.x, to.y }, color);
        drawLine(XY{ to.x, from.y }, to, color);
        drawLine(XY{ from.x, to.y }, to, color);
    }

    virtual uint32_t getPixelAt(XY position, bool ignoreLayerAlpha = true, LayerVariant* targetVariant = NULL) {
        if (position.x >= 0 && position.x < w
            && position.y >= 0 && position.y < h) {
            uint32_t* intpxdata = targetVariant == NULL ? pixels32() : (u32*)targetVariant->pixelData;
            uint32_t pixel = intpxdata[position.x + (position.y * w)];
            uint8_t alpha = (((pixel >> 24) / 255.0f) * (ignoreLayerAlpha ? 1.0f : (layerAlpha / 255.0f))) * 255;
            pixel = (pixel & 0x00ffffff) | (alpha << 24);
            return pixel;
        }
        else {
            return 0xFF000000;
        }
    }

    virtual uint32_t getVisualPixelAt(XY position, bool ignoreLayerAlpha = true) {
        return getPixelAt(position, ignoreLayerAlpha);
    }

    LayerVariant& currentVariant() {
        return layerData[currentLayerVariant];
    }

    /// <summary>
    /// Returns a pointer to the current pixel data of this layer as a uint32 pointer
    /// </summary>
    u32* pixels32() {
        return (u32*)currentVariant().pixelData;
    }

    /// <summary>
    /// Returns a pointer to the current pixel data of this layer as a uint8 pointer
    /// </summary>
    u8* pixels8() {
        return (u8*)pixels32();
    }

    void flipHorizontally(SDL_Rect region = {-1,-1,-1,-1}) {
        if (region.w == -1) {
            region = {0, 0, w, h};
        }
        uint32_t* px32 = pixels32();
        for (int y = 0; y < region.h; y++) {
            for (int x = 0; x < region.w / 2; x++) {
                u32 p = ARRAY2DPOINT(px32, region.x + region.w-1-x, region.y + y, w);
                ARRAY2DPOINT(px32, region.x + region.w - 1 - x, region.y + y, w) = ARRAY2DPOINT(px32, region.x + x, region.y + y, w);
                ARRAY2DPOINT(px32, region.x + x, region.y + y, w) = p;
            }
        }
        markLayerDirty();
    }
    void flipVertically(SDL_Rect region = { -1,-1,-1,-1 }) {
        if (region.w == -1) {
            region = { 0, 0, w, h };
        }
        u32* px32 = pixels32();
        for (int y = 0; y < region.h/2; y++) {
            for (int x = 0; x < region.w; x++) {
                u32 p = ARRAY2DPOINT(px32, region.x + x, region.y + y, w);
                ARRAY2DPOINT(px32, region.x + x, region.y + y, w) = ARRAY2DPOINT(px32, region.x + x, region.y + region.h-1-y, w);
                ARRAY2DPOINT(px32, region.x + x, region.y + region.h-1-y, w) = p;
            }
        }
        markLayerDirty();
    }

    unsigned int numUniqueColors(bool onlyRGB = false) {
        return (unsigned int)getUniqueColors(onlyRGB).size();
    }

    virtual std::vector<u32> getUniqueColors(bool onlyRGB = false) {
        std::map<u32, int> cols;
        u32* pixels = pixels32();
        for (uint64_t x = 0; x < w * h; x++) {
            u32 px = pixels[x];
            if (onlyRGB) {
                px |= 0xff000000;
            }
            else {
                if ((px & 0xff000000) == 0) {
                    px = 0;
                }
            }
            cols[px] = 1;
        }
        std::vector<uint32_t> ret;
        for (auto& a : cols) {
            ret.push_back(a.first);
        }
        return ret;
    }

    std::vector<uint32_t> get256MostUsedColors(bool onlyRGB = false) {
        std::map<uint32_t, int> cols;
        u32* pixels = pixels32();
        int xdiv = 1;
        int ydiv = 1;
        while (w / xdiv > 64) {
            xdiv++;
        }
        while (h / ydiv > 64) {
            ydiv++;
        }
        for (int x = 0; x < w/xdiv; x++) {
            for (int y = 0; y < h/ydiv; y++) {
                uint32_t px = ARRAY2DPOINT(pixels, x*xdiv,y*ydiv,w);
                if (onlyRGB) {
                    px |= 0xff000000;
                }
                else {
                    if ((px & 0xff000000) == 0) {
                        px = 0;
                    }
                }
                cols[px] = 1;
            }
        }

        std::vector<std::pair<uint32_t, int>> colorValues(cols.size());
        std::copy(cols.begin(), cols.end(), colorValues.begin());
        std::sort(colorValues.begin(), colorValues.end(),
            [](const std::pair<uint32_t, int>& a, const std::pair<uint32_t, int>& b) {
                return a.second > b.second;
            }
        );

        std::vector<uint32_t> ret;
        for (auto& a : colorValues) {
            ret.push_back(a.first);
            if (ret.size() >= 256) {
                break;
            }
        }
        return ret;

    }

    //please rename this function to something else some time later
    Layer* copyCurrentVariant();
    Layer* copyCurrentVariantScaled(XY dimensions);
    Layer* copyAllVariantsScaled(XY dimensions);

    void setAllAlpha255() {
        u32* px32 = pixels32();
        for (uint64_t x = 0; x < w * h; x++) {
            px32[x] |= 0xff000000;
        }
        markLayerDirty();
    }

    void replaceColor(uint32_t from, uint32_t to, ScanlineMap* isolate = NULL) {
        u32* px32 = pixels32();
        for (uint64_t x = 0; x < w * h; x++) {
            if (isolate == NULL || isolate->pointExists(XY{ (int)(x % w), (int)(x / w) })) {
                if (px32[x] == from || (!isPalettized && (px32[x] & 0xFF000000) == 0 && (from & 0xFF000000) == 0)) {
                    px32[x] = to;
                }
            }
        }
        markLayerDirty();
    }

    void paintBucket(XY at, u32 color);

    void shiftLayerHSV(hsv shift) {
        if (isPalettized) {
            return;
        }
        u32* px32 = pixels32();
        for (u64 dataPtr = 0; dataPtr < w * h; dataPtr++) {
            px32[dataPtr] = hsvShift(px32[dataPtr], shift);
        }
        markLayerDirty();
    }

    void shiftLayerHSL(hsl shift) {
        if (isPalettized) {
            return;
        }
        u32* px32 = pixels32();
        for (u64 dataPtr = 0; dataPtr < w * h; dataPtr++) {
            px32[dataPtr] = hslShift(px32[dataPtr], shift);
        }
        markLayerDirty();
    }
    
    virtual Layer* trim(SDL_Rect r);

    static Layer* mergeLayers(Layer* bottom, Layer* top);

    //all of these below return the old pixel data
    /// <summary>
    /// Does not scale the image, only resizes the layer to the new dimensions.
    /// </summary>
    /// <param name="to">Target dimensions</param>
    /// <returns>Old pixel data</returns>
    LayerScaleData scaleGeneric(XY newSize, std::function<void(u32* pxNow,u32* pxNew)>);
    LayerScaleData resize(XY to);
    LayerScaleData resizeByTileSizes(XY tileSizesNow, XY targetTileSize);
    //todo:update this one too
    std::vector<LayerVariant> resizeByTileCount(XY tileSizesNow, XY newTileCount);
    LayerScaleData integerScale(XY scale);
    LayerScaleData integerDownscale(XY scale);

    ScanlineMap wandSelectAt(XY pos);
    void wandSelectWithOperationAt(XY pos, std::function<void(XY)> foreachPoint);

    void clear(ScanlineMap* area = NULL);
};

