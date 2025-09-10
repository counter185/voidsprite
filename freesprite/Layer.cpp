#include "Layer.h"
#include "LayerPalettized.h"
#include "mathops.h"
#include "Notification.h"

void Layer::setLayerData(std::vector<LayerVariant> data, XY dimensions)
{
    layerData = data;
    w = dimensions.x;
    h = dimensions.y;
}

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

    markLayerDirty();
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
                &ARRAY2DPOINT(pixels32(), position.x, y + position.y, w),
                &ARRAY2DPOINT(sourceLayer->pixels32(), clipSource.x, y + clipSource.y, sourceLayer->w),
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
    u32* px32 = pixels32();
    u32* srcpx32 = sourceLayer->pixels32();
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

Layer* Layer::copyCurrentVariant()
{
    auto variantsCopy = copyAllVariants();
    if (variantsCopy.size() > 0) {
        Layer* ret = new Layer(w, h, variantsCopy);
        ret->name = name;
        ret->currentLayerVariant = currentLayerVariant;
        ret->colorKey = colorKey;
        ret->markLayerDirty();
        return ret;
    }
    return NULL;
}

//i don't even know if this works
Layer* Layer::copyCurrentVariantScaled(XY dimensions)
{
    bool shouldIntegerScale = 
        ixmax(dimensions.x, w) % ixmin(dimensions.x, w) == 0
        && ixmax(dimensions.y, h) % ixmin(dimensions.y, h) == 0;

    Layer* newLayer = NULL;
    if (isPalettized) {
        newLayer = LayerPalettized::tryAllocIndexedLayer(dimensions.x, dimensions.y);
        if (newLayer != NULL) {
            ((LayerPalettized*)newLayer)->palette = ((LayerPalettized*)this)->palette;
        }
    }
    else {
        newLayer = tryAllocLayer(dimensions.x, dimensions.y);
        if (newLayer != NULL) {
            newLayer->layerAlpha = layerAlpha;
        }
    }

    if (newLayer != NULL) {
        newLayer->name = name;
        newLayer->colorKey = colorKey;
        newLayer->colorKeySet = colorKeySet;
        newLayer->hidden = hidden;

        if (!shouldIntegerScale) {
            loginfo(frmt("Performing float-based scale {}x{} -> {}x{}", w, h, dimensions.x, dimensions.y));
            for (int y = 0; y < newLayer->h; y++) {
                for (int x = 0; x < newLayer->w; x++) {
                    XY samplePoint = {
                        w * (x / (float)newLayer->w),
                        h * (y / (float)newLayer->h)
                    };
                    newLayer->setPixel({ x, y }, getPixelAt(samplePoint));
                    //newLayer->setPixel(XY{(int)(x * (dimensions.x / (float)w)), (int)(y * (dimensions.y / (float)h))}, getPixelAt(XY{x, y}));
                }
            }
        }
        else {
            loginfo(frmt("Performing integer scale {}x{} -> {}x{}", w,h, dimensions.x, dimensions.y));
            bool downscale = dimensions.x < w || dimensions.y < h;
            int scaleFactorX = ixmax(dimensions.x, w) / ixmin(dimensions.x, w);
            int scaleFactorY = ixmax(dimensions.y, h) / ixmin(dimensions.y, h);

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

Layer* Layer::copyAllVariantsScaled(XY dimensions)
{
    bool shouldIntegerScale =
        ixmax(dimensions.x, w) % ixmin(dimensions.x, w) == 0
        && ixmax(dimensions.y, h) % ixmin(dimensions.y, h) == 0;

    Layer* newLayer = NULL;

    std::vector<LayerVariant> scaledVariants;
    for (LayerVariant& variant : layerData) {
        LayerVariant newVariant = variant;
        newVariant.pixelData = (u8*)tracked_malloc(dimensions.x * dimensions.y * 4, "Layers");
        scaledVariants.push_back(newVariant);
    }

    if (isPalettized) {
        newLayer = new LayerPalettized(dimensions.x, dimensions.y, scaledVariants);
        if (newLayer != NULL) {
            ((LayerPalettized*)newLayer)->palette = ((LayerPalettized*)this)->palette;
        }
    }
    else {
        newLayer = new Layer(dimensions.x, dimensions.y, scaledVariants);
        if (newLayer != NULL) {
            newLayer->layerAlpha = layerAlpha;
        }
    }

    if (newLayer != NULL) {
        newLayer->name = name;
        newLayer->colorKey = colorKey;
        newLayer->colorKeySet = colorKeySet;
        newLayer->hidden = hidden;

        if (!shouldIntegerScale) {
            loginfo(frmt("Performing float-based scale {}x{} -> {}x{}", w, h, dimensions.x, dimensions.y));
            for (int v = 0; v < layerData.size(); v++) {
                for (int y = 0; y < newLayer->h; y++) {
                    for (int x = 0; x < newLayer->w; x++) {
                        XY samplePoint = {
                            w * (x / (float)newLayer->w),
                            h * (y / (float)newLayer->h)
                        };
                        newLayer->setPixel({ x, y }, getPixelAt(samplePoint, true, &layerData[v]), &scaledVariants[v]);
                    }
                }
            }
        }
        else {
            loginfo(frmt("Performing integer scale {}x{} -> {}x{}", w, h, dimensions.x, dimensions.y));
            bool downscale = dimensions.x < w || dimensions.y < h;
            int scaleFactorX = ixmax(dimensions.x, w) / ixmin(dimensions.x, w);
            int scaleFactorY = ixmax(dimensions.y, h) / ixmin(dimensions.y, h);

            for (int v = 0; v < layerData.size(); v++) {
                for (int y = 0; y < newLayer->h; y++) {
                    for (int x = 0; x < newLayer->w; x++) {
                        XY samplePoint =
                            !downscale ? XY{ x / scaleFactorX, y / scaleFactorY }
                        : XY{ x * scaleFactorX, y * scaleFactorY };
                        newLayer->setPixel({ x, y }, getPixelAt(samplePoint, true, &layerData[v]), &scaledVariants[v]);
                    }
                }
            }
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

LayerScaleData Layer::scaleGeneric(XY newSize, std::function<void(u32*,u32*)> operation)
{
    if (4ull * (u64)newSize.x * (u64)newSize.y > 1000000000ull) {
        g_addNotification(NOTIF_MALLOC_FAIL);
        logerr("[scaleGeneric] massive resize attempted");
        return { false };
    }
    std::vector<LayerVariant> oldVariants = layerData;
    std::vector<LayerVariant> scaledVariants;

    std::vector<u32*> pxdsCreated;

    for (auto& variant : layerData) {
        uint32_t* newPixelData = (uint32_t*)tracked_malloc(newSize.x * newSize.y * 4, "Layers");

        if (newPixelData != NULL) {
            pxdsCreated.push_back(newPixelData);
        }
        else {
            logerr("resize malloc fail");
            g_addNotification(NOTIF_MALLOC_FAIL);
            for (u32*& pxds : pxdsCreated) {
                tracked_free(pxds);
            }
            return { false };
        }

        uint32_t* pixelDataNow = (uint32_t*)variant.pixelData;
        memset(newPixelData, 0, 4ull * newSize.x * newSize.y);
        operation(pixelDataNow, newPixelData);
        LayerVariant variantCopy = variant;
        variantCopy.pixelData = (uint8_t*)newPixelData;
        scaledVariants.push_back(variantCopy);
    }
    return { true, newSize, scaledVariants };
}

LayerScaleData Layer::resize(XY to)
{
    return scaleGeneric(to, [this, to](u32* pxdNow, u32* pxdNew) {
        for (int y = 0; y < ixmin(h, to.y); y++) {
            for (int x = 0; x < ixmin(w, to.x); x++) {
                ARRAY2DPOINT(pxdNew, x, y, to.x) = ARRAY2DPOINT(pxdNow, x, y, w);
            }
        }
    });
}

LayerScaleData Layer::resizeByTileSizes(XY tileSizesNow, XY targetTileSize)
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

    return scaleGeneric(newSize, [&](u32* pxdNow, u32* pxdNew) {
        for (int tileY = 0; tileY < newTilesCount.y; tileY++) {
            for (int tileX = 0; tileX < newTilesCount.x; tileX++) {

                XY oldTilePos = XY{ tileX * tileSizesNow.x, tileY * tileSizesNow.y };
                XY newTilePos = XY{ tileX * targetTileSize.x, tileY * targetTileSize.y };
                for (int y = 0; y < ixmin(tileSizesNow.y, targetTileSize.y); y++) {
                    for (int x = 0; x < ixmin(tileSizesNow.x, targetTileSize.x); x++) {
                        if (oldTilePos.x + x < sizeNow.x && oldTilePos.y + y < sizeNow.y) {
                            pxdNew[(newTilePos.x + x) + ((newTilePos.y + y) * newSize.x)] = pxdNow[(oldTilePos.x + x) + ((oldTilePos.y + y) * w)];
                        }
                    }
                }
            }
        }
    });
}

std::vector<LayerVariant> Layer::resizeByTileCount(XY tileSizesNow, XY newTileCount)
{
    XY newSize = XY{
        tileSizesNow.x * newTileCount.x,
        tileSizesNow.y * newTileCount.y
    };

    std::vector<LayerVariant> oldVariants = layerData;
    std::vector<LayerVariant> scaledVariants;

    std::vector<u32*> pxdsCreated;

    for (auto& variant : layerData) {
        uint32_t* newPixelData = (uint32_t*)tracked_malloc(newSize.x * newSize.y * 4, "Layers");

        if (newPixelData != NULL) {
            pxdsCreated.push_back(newPixelData);
        }
        else {
            logerr("resizeByTileCount malloc fail");
            g_addNotification(NOTIF_MALLOC_FAIL);
            for (u32*& pxds : pxdsCreated) {
                tracked_free(pxds);
            }
            return {};
        }

        uint32_t* oldPixelData = (uint32_t*)variant.pixelData;
        memset(newPixelData, 0, newSize.x * newSize.y * 4);
        for (int y = 0; y < ixmin(h, newSize.y); y++) {
            memcpy(newPixelData + (y * newSize.x), oldPixelData + (y * w), ixmin(w, newSize.x) * 4);
        }
        LayerVariant variantCopy = variant;
        variantCopy.pixelData = (uint8_t*)newPixelData;
        scaledVariants.push_back(variantCopy);
    }

    layerData = scaledVariants;
    w = newSize.x;
    h = newSize.y;
    return oldVariants;
}

LayerScaleData Layer::integerScale(XY scale)
{
    XY newSize = { w * scale.x, h * scale.y };

    return scaleGeneric(newSize, [&](u32* pxNow, u32* pxNew) {
        for (int y = 0; y < newSize.y; y++) {
            for (int x = 0; x < newSize.x; x++) {
                pxNew[x + (y * newSize.x)] = pxNow[(x / scale.x) + ((y / scale.y) * w)];
            }
        }
    });
}

LayerScaleData Layer::integerDownscale(XY scale)
{
    if (w % scale.x == 0 && h % scale.y == 0) {

        XY newSize = { w / scale.x, h / scale.y };

        return scaleGeneric(newSize, [&](u32* pxNow, u32* pxNew) {
            for (int y = 0; y < newSize.y; y++) {
                for (int x = 0; x < newSize.x; x++) {
                    pxNew[x + (y * newSize.x)] = pxNow[(x * scale.x) + ((y * scale.y) * w)];
                }
            }
        });
    }
    else {
        return {false};
    }
}

ScanlineMap Layer::wandSelectAt(XY pos) {
    ScanlineMap r;
    wandSelectWithOperationAt(pos, [&](XY a) {
        r.addPoint(a);
    });
    return r;
}
void Layer::wandSelectWithOperationAt(XY pos, std::function<void(XY)> foreachPoint) {
    u32 pixel = getPixelAt(pos);
    ScanlineMap ret;
    std::vector<XY> openList;
    openList.push_back(pos);
    std::vector<XY> nextList;
    while (!openList.empty()) {
        for (XY& openListElement : openList) {
            uint32_t pixelRn = getPixelAt(openListElement);
            if (pointInBox(openListElement, { 0,0,w,h }) &&
                !ret.pointExists(openListElement) &&
                (pixelRn == pixel || (!isPalettized && pixelRn >> 24 == 0 && pixel >> 24 == 0))) {
                ret.addPoint(openListElement);
                foreachPoint(openListElement);
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
}

void Layer::clear(ScanlineMap* area)
{
    if (area == NULL) {
        memset(pixels32(), isPalettized ? -1 : 0, w * h * 4);
    }
    else {
        u32* ppx = pixels32();
        area->forEachPoint([&](XY p) {
            if (pointInBox(p, SDL_Rect{ 0,0,w,h })) {
                ARRAY2DPOINT(ppx, p.x, p.y, w) = isPalettized ? -1 : 0x00000000;
            }
        });
    }
    markLayerDirty();
}
