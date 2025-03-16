#include "Layer.h"
#include "LayerPalettized.h"
#include "mathops.h"

// Blits source layer onto this layer at position
void Layer::blit(Layer* sourceLayer, XY position)
{
    if (position.x >= w || position.y >= h){
        return;
    }

    XY endPosition = {
        ixmin(position.x + sourceLayer->w, w),
        ixmin(position.y + sourceLayer->h, h)
    };

    for (int y = position.y; y < endPosition.y; y++) {
        for (int x = position.x; x < endPosition.x; x++) {
            setPixel(XY{x, y}, sourceLayer->getPixelAt(XY{x - position.x, y - position.y}));
        }
    }

    layerDirty = true;
}

void Layer::blit(Layer* sourceLayer, XY position, SDL_Rect clipSource, bool fast)
{
    if (position.x >= w || position.y >= h) {
        return;
    }

    for (int y = 0; y < clipSource.h; y++) {
        if (!fast || sourceLayer->colorKeySet) {
            for (int x = 0; x < clipSource.w; x++) {
                u32 px = sourceLayer->getPixelAt(XY{ x + clipSource.x, y + clipSource.y}, false);
                if (sourceLayer->colorKeySet && px == sourceLayer->colorKey) {
					continue;
				}
                u32 blendedPixel = alphaBlend(getPixelAt(XY{ x + position.x, y + position.y }), px);
                setPixel(XY{ x + position.x, y + position.y }, blendedPixel);
            }
        }
        else {
            int nPixels = ixmin(clipSource.w, w - position.x) * 4;
            memcpy(
                pixelData + ((y + position.y) * w * 4) + (position.x * 4),
                sourceLayer->pixelData + ((y + clipSource.y) * sourceLayer->w * 4) + (clipSource.x * 4),
                nPixels
            );
            /*if (sourceLayer->colorKeySet) {
                replaceColor(sourceLayer->colorKey, 0x000000);
            }*/
        }
    }
        
}

void Layer::blitTile(Layer* sourceLayer, XY sourceTile, XY dstTile, XY tileSize)
{
    u32* px32 = (u32*)pixelData;
    u32* srcpx32 = (u32*)sourceLayer->pixelData;
    XY originSrc = XY{sourceTile.x * tileSize.x, sourceTile.y * tileSize.y};
    XY originDst = XY{dstTile.x * tileSize.x, dstTile.y * tileSize.y};
    for (int y = 0; y < tileSize.y; y++) {
        XY srcPos = XY{originSrc.x, originSrc.y + y};
        XY dstPos = XY{originDst.x, originDst.y + y};
        if (pointInBox(dstPos, { 0,0,w,h }) && pointInBox(srcPos, {0,0,sourceLayer->w,sourceLayer->h})) {
            int clippedXSize = ixmin(ixmin(tileSize.x, w - dstPos.x), sourceLayer->w - srcPos.x);
            u64 srcIndex = srcPos.x + (srcPos.y * sourceLayer->w);
            u64 dstIndex = dstPos.x + (dstPos.y * w);
            memcpy(px32 + dstIndex, srcpx32 + srcIndex, clippedXSize * 4);
        }
    }
}

Layer* Layer::copy()
{
    Layer* ret = new Layer(w, h);
    ret->name = name;
    ret->colorKey = colorKey;
    ret->layerDirty = true;
    memcpy(ret->pixelData, pixelData, w * h * 4);
    return ret;
}

Layer* Layer::copyWithNoTextureInit()
{
    Layer* ret = new Layer();
    ret->w = w;
    ret->h = h;
    ret->pixelData = (uint8_t*)tracked_malloc(w * h * 4, "Layers");
    ret->name = name;
    ret->colorKey = colorKey;
    memcpy(ret->pixelData, pixelData, w * h * 4);
    return ret;
}

//i don't even know if this works
Layer* Layer::copyScaled(XY dimensions)
{
    Layer* newLayer = new Layer(dimensions.x, dimensions.y);

    for (int y = 0; y < newLayer->h; y++) {
        for (int x = 0; x < newLayer->w; x++) {
            XY samplePoint = {
                w * (x / (float)newLayer->w),
                h * (y / (float)newLayer->h)
            };
            newLayer->setPixel({x, y}, getPixelAt(samplePoint));
            //newLayer->setPixel(XY{(int)(x * (dimensions.x / (float)w)), (int)(y * (dimensions.y / (float)h))}, getPixelAt(XY{x, y}));
        }
    }
    return newLayer;
}

void Layer::paintBucket(XY pos, u32 color) {
    wandSelectAt(pos).forEachPoint([&](XY p) {
        setPixel(p, color);
    });
}

Layer* Layer::trim(SDL_Rect r)
{
    if (r.x + r.w > w || r.y + r.h > h) {
        return NULL;
    }
    Layer* newLayer;
    if (isPalettized) {
        newLayer = new LayerPalettized(r.w, r.h);
        ((LayerPalettized*)newLayer)->palette = ((LayerPalettized*)this)->palette;
    }
    else {
        newLayer = new Layer(r.w, r.h);
    }

    for (int y = 0; y < r.h; y++) {
        for (int x = 0; x < r.w; x++) {
            newLayer->setPixel(XY{x, y}, getPixelAt(XY{x + r.x, y + r.y}, false));
        }
    }
    return newLayer;
}

uint8_t* Layer::resize(XY to)
{
    uint32_t* newPixelData = (uint32_t*)tracked_malloc(to.x * to.y * 4, "Layers");
    uint32_t* pixelDataNow = (uint32_t*)pixelData;
    memset(newPixelData, 0, to.x * to.y * 4);
    for (int y = 0; y < ixmin(h, to.y); y++) {
        for (int x = 0; x < ixmin(w, to.x); x++) {
            newPixelData[x + (y * to.x)] = pixelDataNow[x + (y * w)];
        }
    }
    pixelData = (uint8_t*)newPixelData;
    w = to.x;
    h = to.y;
    return (uint8_t*)pixelDataNow;
}

uint8_t* Layer::resizeByTileSizes(XY tileSizesNow, XY targetTileSize)
{
    XY sizeNow = XY{w, h};
    XY newTilesCount = XY{
        (int)ceil(w / (float)tileSizesNow.x),
        (int)ceil(h / (float)tileSizesNow.y)
    };
    XY newSize = XY{
        targetTileSize.x * newTilesCount.x,
        targetTileSize.y * newTilesCount.y
    };

    uint32_t* newPixelData = (uint32_t*)tracked_malloc(newSize.x * newSize.y * 4, "Layers");
    uint32_t* oldPixelData = (uint32_t*)pixelData;
    memset(newPixelData, 0, newSize.x * newSize.y * 4);
    for (int tileY = 0; tileY < newTilesCount.y; tileY++) {
        for (int tileX = 0; tileX < newTilesCount.x; tileX++) {
            
            XY oldTilePos = XY{tileX * tileSizesNow.x, tileY * tileSizesNow.y};
            XY newTilePos = XY{tileX * targetTileSize.x, tileY * targetTileSize.y};
            for (int y = 0; y < ixmin(tileSizesNow.y, targetTileSize.y); y++) {
                for (int x = 0; x < ixmin(tileSizesNow.x, targetTileSize.x); x++) {
                    if (oldTilePos.x + x < sizeNow.x && oldTilePos.y + y < sizeNow.y) {
                        newPixelData[(newTilePos.x + x) + ((newTilePos.y + y) * newSize.x)] = oldPixelData[(oldTilePos.x + x) + ((oldTilePos.y + y) * w)];
                    }
                }
            }
        }
    }
    pixelData = (uint8_t*)newPixelData;
    w = newSize.x;
    h = newSize.y;
    return (uint8_t*)oldPixelData;
}

uint8_t* Layer::resizeByTileCount(XY tileSizesNow, XY newTileCount)
{
    XY newSize = XY{
        tileSizesNow.x * newTileCount.x,
        tileSizesNow.y * newTileCount.y
    };
    uint32_t* newPixelData = (uint32_t*)tracked_malloc(newSize.x * newSize.y * 4, "Layers");
    uint32_t* oldPixelData = (uint32_t*)pixelData;
    memset(newPixelData, 0, newSize.x * newSize.y * 4);
    for (int y = 0; y < ixmin(h, newSize.y); y++) {
        memcpy(newPixelData + (y * newSize.x), oldPixelData + (y * w), ixmin(w, newSize.x) * 4);
        /*for (int x = 0; x < ixmin(w, newSize.x); x++) {
            newPixelData[x + (y * newSize.x)] = oldPixelData[x + (y * w)];
        }*/
    }
    pixelData = (uint8_t*)newPixelData;
    w = newSize.x;
    h = newSize.y;
    return (uint8_t*)oldPixelData;
}

uint8_t* Layer::integerScale(XY scale)
{
    XY newSize = { w * scale.x, h * scale.y };
    uint32_t* newPixelData = (uint32_t*)tracked_malloc(newSize.x * newSize.y * 4, "Layers");
    uint32_t* oldPixelData = (uint32_t*)pixelData;
    memset(newPixelData, 0, newSize.x * newSize.y * 4);
    for (int y = 0; y < newSize.y; y++) {
        for (int x = 0; x < newSize.x; x++) {
            newPixelData[x + (y * newSize.x)] = oldPixelData[(x / scale.x) + ((y / scale.y) * w)];
        }
    }
    pixelData = (uint8_t*)newPixelData;
    w = newSize.x;
    h = newSize.y;
    return (uint8_t*)oldPixelData;
}

uint8_t* Layer::integerDownscale(XY scale)
{
    if (w % scale.x == 0 && h % scale.y == 0) {
        XY newSize = { w / scale.x, h / scale.y };
        uint32_t* newPixelData = (uint32_t*)tracked_malloc(newSize.x * newSize.y * 4, "Layers");
        uint32_t* oldPixelData = (uint32_t*)pixelData;
        memset(newPixelData, 0, newSize.x * newSize.y * 4);
        for (int y = 0; y < newSize.y; y++) {
            for (int x = 0; x < newSize.x; x++) {
                newPixelData[x + (y * newSize.x)] = oldPixelData[(x * scale.x) + ((y * scale.y) * w)];
            }
        }
        pixelData = (uint8_t*)newPixelData;
        w = newSize.x;
        h = newSize.y;
        return (uint8_t*)oldPixelData;
    }
    else {
        return NULL;
    }
}

ScanlineMap Layer::wandSelectAt(XY pos) {
    u32 pixel = getPixelAt(pos);
    ScanlineMap ret;
    std::vector<XY> openList;
    openList.push_back(pos);
    std::vector<XY> nextList;
    while (!openList.empty()) {
        for (XY& openListElement : openList) {
            uint32_t pixelRn = getPixelAt(openListElement);
            if (pointInBox(openListElement,{0,0,w,h}) &&
                !ret.pointExists(openListElement) &&
                (pixelRn == pixel || (!isPalettized && pixelRn >> 24 == 0 && pixel >> 24 == 0))) {
                ret.addPoint(openListElement);
                XY p[] = {
                    {0,  1 },
                    {0,  -1},
                    {1,  0 },
                    {-1, 0 }
                };
                for (XY& pp : p) {
                    nextList.push_back(xyAdd(openListElement, pp));
                }
            }
        }
        openList = nextList;
        nextList.clear();
    }
    return ret;
}

void Layer::clear(ScanlineMap* area)
{
    if (area == NULL) {
        memset(pixelData, isPalettized ? -1 : 0, w * h * 4);
    }
    else {
        u32* ppx = (u32*)pixelData;
        area->forEachPoint([&](XY p) {
            if (pointInBox(p, SDL_Rect{ 0,0,w,h })) {
                ARRAY2DPOINT(ppx, p.x, p.y, w) = isPalettized ? -1 : 0x00000000;
            }
        });
    }
    layerDirty = true;
}
