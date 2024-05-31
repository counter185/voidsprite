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
