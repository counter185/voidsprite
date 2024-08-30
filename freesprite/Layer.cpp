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

void Layer::resizeByTileSizes(XY tileSizesNow, XY targetTileSize)
{
}
