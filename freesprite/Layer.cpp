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
