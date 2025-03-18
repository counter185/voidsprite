#pragma once
#include "globals.h"
#include "mathops.h"
class Layer
{
protected:
    Layer() {}
public:
    bool isPalettized = false;

    std::map<std::string, std::string> importExportExtdata;

    /// ARGB pixel data
    u8* pixelData;
    std::vector<u8*> undoQueue;
    std::vector<u8*> redoQueue;
    int w, h;
    SDL_Texture* tex = NULL;
    XY texDimensions = {0,0};
    bool layerDirty = true;
    bool bgOpFinished = false;

    std::string name = "Layer";
    bool hidden = false;

    bool colorKeySet = false;
    u32 colorKey = 0;
    u8 lastConfirmedlayerAlpha = 255;
    u8 layerAlpha = 255;

    SDL_Texture* effectPreviewTexture = NULL;

    Layer(int width, int height) {
        w = width;
        h = height;
        pixelData = (u8*)tracked_malloc(width * height * 4, "Layers");
        if (pixelData != NULL) {
            memset(pixelData, 0, width * height * 4);
            tex = tracked_createTexture(g_rd, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);
            texDimensions = XY{ w,h };
            SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
        }
    }
    Layer(SDL_Surface* from) : Layer(from->w, from->h) {
        //pixelData = (u8*)tracked_malloc(area() * 4);
        if (from->format == SDL_PIXELFORMAT_ARGB8888) {
            memcpy(pixelData, from->pixels, area() * 4);
        }
        else {
            SDL_ConvertPixels(w, h, from->format, from->pixels, from->pitch, SDL_PIXELFORMAT_ARGB8888, pixelData, w * 4);
            SDL_FreeSurface(from);
        }
    }

    virtual ~Layer() {
        tracked_free(pixelData);
        for (u8*& u : undoQueue) {
            tracked_free(u);
        }
        for (u8*& r : redoQueue) {
            tracked_free(r);
        }
        tracked_destroyTexture(tex);
    }

    virtual void updateTexture() {
        u8* pixels;
        int pitch;
        if (texDimensions.x != w || texDimensions.y != h) {
            tracked_destroyTexture(tex);
            tex = tracked_createTexture(g_rd, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);
            texDimensions = XY{ w,h };
            SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
        }
        SDL_LockTexture(tex, NULL, (void**)&pixels, &pitch);
        if (pitch == w*4) {
            memcpy(pixels, pixelData, area() * 4);
        } else {
            for (int y = 0; y < h; y++) {
                memcpy(pixels + y * pitch, pixelData + y * w * 4, w* 4);
            }
        }
        //memcpy(pixels, pixelData, area() * 4);

        //todo respect the pitch in the below too
        if (colorKeySet) {
            u32* px32 = (u32*)pixelData;
            for (int y = 0; y < h; y++) {
                for (int x = 0; x < w; x++) {
                    if ((ARRAY2DPOINT(px32, x,y, w) & 0xffffff) == (colorKey & 0xFFFFFF)) {
                        ARRAY2DPOINT(pixels, x*4+3, y, pitch) = 0;
                    }
                }
            }
            /*for (u64 p = 0; p < area(); p++) {
                if ((px32[p] & 0xffffff) == (colorKey & 0xFFFFFF)) {
                    pixels[p * 4+3] = 0;
                }
            }*/
        }
        SDL_UnlockTexture(tex);
        layerDirty = false;
    }

    void render(SDL_Rect where, u8 alpha = 255) {
        SDL_Texture* target = effectPreviewTexture ? effectPreviewTexture : tex;
        if (layerDirty) {
            updateTexture();
        }
        if (bgOpFinished) {
            updateTexture();
            bgOpFinished = false;
        }
        SDL_SetTextureAlphaMod(target, alpha);
        SDL_RenderCopy(g_rd, target, NULL, &where);
    }

    void render(SDL_Rect where, SDL_Rect clip, u8 alpha = 255) {
        if (layerDirty) {
            updateTexture();
        }
        if (bgOpFinished) {
            updateTexture();
            bgOpFinished = false;
        }
        SDL_SetTextureAlphaMod(tex, alpha);
        SDL_RenderCopy(g_rd, tex, &clip, &where);
    }

    inline u64 area() {
        return (u64) w * (u64) h;
    }

    SDL_Texture* renderToTexture() {
        SDL_Texture* ret = tracked_createTexture(g_rd, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);
        SDL_SetTextureBlendMode(ret, SDL_BLENDMODE_BLEND);
        u8* pixels;
        int pitch;
        SDL_LockTexture(ret, NULL, (void**)&pixels, &pitch);
        if (!isPalettized) {
            memcpy(pixels, pixelData, area() * 4);
        }
        else {
            u32* px32 = (u32*)pixels;
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

    void setPixel(XY position, u32 color) {
        u32* intpxdata = (u32*)pixelData;
        if (position.x >= 0 && position.x < w
            && position.y >= 0 && position.y < h) {
            intpxdata[position.x + (position.y * w)] = color;
            layerDirty = true;
        }
    }
    void fillRect(XY from, XY to, u32 color) {
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
    void drawLine(XY from, XY to, u32 color) {
        rasterizeLine(from, to, [this, color](XY p) { setPixel(p, color); });
    }
    void drawRect(XY from, XY to, u32 color) {
        drawLine(from, XY{ to.x, from.y }, color);
        drawLine(from, XY{ from.x, to.y }, color);
        drawLine(XY{ to.x, from.y }, to, color);
        drawLine(XY{ from.x, to.y }, to, color);
    }

    virtual u32 getPixelAt(XY position, bool ignoreLayerAlpha = true) {
        if (position.x >= 0 && position.x < w
            && position.y >= 0 && position.y < h) {
            u32* intpxdata = (u32*)pixelData;
            u32 pixel = intpxdata[position.x + (position.y * w)];
            u8 alpha = (((pixel >> 24) / 255.0f) * (ignoreLayerAlpha ? 1.0f : (layerAlpha / 255.0f))) * 255;
            pixel = (pixel & 0x00ffffff) | (alpha << 24);
            return pixel;
        }
        else {
            return 0xFF000000;
        }
    }

    virtual u32 getVisualPixelAt(XY position, bool ignoreLayerAlpha = true) {
        return getPixelAt(position, ignoreLayerAlpha);
    }

    void flipHorizontally(SDL_Rect region = {-1,-1,-1,-1}) {
        if (region.w == -1) {
            region = {0, 0, w, h};
        }
        u32* px32 = (u32*)pixelData;
        for (int y = 0; y < region.h; y++) {
            for (int x = 0; x < region.w / 2; x++) {
                u32 p = ARRAY2DPOINT(px32, region.x + region.w-1-x, region.y + y, w);
                ARRAY2DPOINT(px32, region.x + region.w - 1 - x, region.y + y, w) = ARRAY2DPOINT(px32, region.x + x, region.y + y, w);
                ARRAY2DPOINT(px32, region.x + x, region.y + y, w) = p;
            }
        }
        layerDirty = true;
    }
    void flipVertically(SDL_Rect region = { -1,-1,-1,-1 }) {
        if (region.w == -1) {
            region = { 0, 0, w, h };
        }
        u32* px32 = (u32*)pixelData;
        for (int y = 0; y < region.h/2; y++) {
            for (int x = 0; x < region.w; x++) {
                u32 p = ARRAY2DPOINT(px32, region.x + x, region.y + y, w);
                ARRAY2DPOINT(px32, region.x + x, region.y + y, w) = ARRAY2DPOINT(px32, region.x + x, region.y + region.h-1-y, w);
                ARRAY2DPOINT(px32, region.x + x, region.y + region.h-1-y, w) = p;
            }
        }
        layerDirty = true;
    }

    void discardRedoStack() {
        for (u8*& redoD : redoQueue) {
            tracked_free(redoD);
        }
        redoQueue.clear();
    }
    void discardLastUndo() {
        if (!undoQueue.empty()) {
            tracked_free(undoQueue[0]);
            undoQueue.erase(undoQueue.begin());
        }
    }
    void commitStateToUndoStack() {
        discardRedoStack();
        u8* copiedPixelData = (u8*)tracked_malloc(area() * 4, "Layers");
        if (copiedPixelData != NULL) {
            memcpy(copiedPixelData, pixelData, area() * 4);
            undoQueue.push_back(copiedPixelData);
        }
        else {
            printf("malloc FAILED we are FUCKED\n");
        }
    }
    void undo() {
        if (!undoQueue.empty()) {
            redoQueue.push_back(pixelData);
            pixelData = undoQueue[undoQueue.size() - 1];
            undoQueue.pop_back();
            layerDirty = true;
        }
    }
    void redo() {
        if (!redoQueue.empty()) {
            undoQueue.push_back(pixelData);
            pixelData = redoQueue[redoQueue.size()-1];
            redoQueue.pop_back();
            layerDirty = true;
        }
    }

    unsigned int numUniqueColors(bool onlyRGB = false) {
        return (unsigned int)getUniqueColors(onlyRGB).size();
    }

    virtual std::vector<u32> getUniqueColors(bool onlyRGB = false) {
        std::map<u32, int> cols;
        u32* pixels = (u32*)pixelData;
        for (u64 x = 0; x < area(); x++) {
            u64 px = pixels[x];
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
        std::vector<u32> ret;
        for (auto& a : cols) {
            ret.push_back(a.first);
        }
        return ret;
    }

    std::vector<u32> get256MostUsedColors(bool onlyRGB = false) {
        std::map<u32, int> cols;
        u32* pixels = (u32*)pixelData;
        for (u64 x = 0; x < (u64) w * (u64) h; x++) {
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

        std::vector<std::pair<u32, int>> colorValues(cols.size());
        std::copy(cols.begin(), cols.end(), colorValues.begin());
        std::sort(colorValues.begin(), colorValues.end(),
            [](const std::pair<u32, int>& a, const std::pair<u32, int>& b) {
                return a.second > b.second;
            }
        );

        std::vector<u32> ret;
        for (auto& a : colorValues) {
            ret.push_back(a.first);
            if (ret.size() >= 256) {
                break;
            }
        }
        return ret;

    }

    Layer* copy();
    Layer* copyWithNoTextureInit();
    Layer* copyScaled(XY dimensions);

    void setAllAlpha255() {
        u32* px32 = (u32*)pixelData;
        for (u64 x = 0; x < area(); x++) {
            px32[x] |= 0xff000000;
        }
        layerDirty = true;
    }

    void replaceColor(u32 from, u32 to, ScanlineMap* isolate = NULL) {
        u32* px32 = (u32*)pixelData;
        for (u64 x = 0; x < area(); x++) {
            if (isolate == NULL || isolate->pointExists(XY{ (int)(x % w), (int)(x / w) })) {
                if (px32[x] == from || (!isPalettized && (px32[x] & 0xFF000000) == 0 && (from & 0xFF000000) == 0)) {
                    px32[x] = to;
                }
            }
        }
        layerDirty = true;
    }

    void paintBucket(XY at, u32 color);

    void shiftLayerHSV(hsv shift) {
        if (isPalettized) {
            return;
        }
        u32* px32 = (u32*)pixelData;
        for (u64 dataPtr = 0; dataPtr < area(); dataPtr++) {
            px32[dataPtr] = hsvShift(px32[dataPtr], shift);
        }
        layerDirty = true;
    }

    void shiftLayerHSL(hsl shift) {
        if (isPalettized) {
            return;
        }
        u32* px32 = (u32*)pixelData;
        for (u64 dataPtr = 0; dataPtr < area(); dataPtr++) {
            px32[dataPtr] = hslShift(px32[dataPtr], shift);
        }
        layerDirty = true;
    }

    virtual Layer* trim(SDL_Rect r);

    //all of these below return the old pixel data
    /// <summary>
    /// Does not scale the image, only resizes the layer to the new dimensions.
    /// </summary>
    /// <param name="to">Target dimensions</param>
    /// <returns>Old pixel data</returns>
    u8* resize(XY to);
    u8* resizeByTileSizes(XY tileSizesNow, XY targetTileSize);
    u8* resizeByTileCount(XY tileSizesNow, XY newTileCount);
    u8* integerScale(XY scale);
    u8* integerDownscale(XY scale);

    ScanlineMap wandSelectAt(XY pos);

    void clear(ScanlineMap* area = NULL);
};

