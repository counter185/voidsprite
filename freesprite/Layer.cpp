#include "Layer.h"
#include "mathops.h"

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

//i don't even know if this works
Layer* Layer::copyScaled(XY dimensions)
{
    Layer* newLayer = new Layer(dimensions.x, dimensions.y);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            newLayer->setPixel(XY{(int)(x * (dimensions.x / (float)w)), (int)(y * (dimensions.y / (float)h))}, getPixelAt(XY{x, y}));
        }
    }
    return newLayer;
}

uint8_t* Layer::resize(XY to)
{
    uint32_t* newPixelData = (uint32_t*)malloc(to.x * to.y * 4);
    uint32_t* pixelDataNow = (uint32_t*)pixelData;
    memset(newPixelData, 0, to.x * to.y * 4);
    for (int y = 0; y < ixmin(h, to.y); y++) {
		for (int x = 0; x < ixmin(w, to.x); x++) {
			newPixelData[x + (y * to.x)] = pixelDataNow[x + (y * h)];
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

    uint32_t* newPixelData = (uint32_t*)malloc(newSize.x * newSize.y * 4);
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
    uint32_t* newPixelData = (uint32_t*)malloc(newSize.x * newSize.y * 4);
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
