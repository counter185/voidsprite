#include "BaseTemplate.h"
#include "Layer.h"
#include "MainEditorPalettized.h"

void BaseTemplate::drawPattern(Layer* layer, uint8_t* pattern, XY patternDimensions, XY position, uint32_t color)
{
    for (int y = 0; y < patternDimensions.y; y++) {
        for (int x = 0; x < patternDimensions.x; x++) {
            if (pattern[x + y * patternDimensions.x] != 0) {
                layer->setPixel(xyAdd(position, XY{ x,y }), color);
            }
        }
    }
}

void BaseTemplate::drawCheckerboard(Layer* layer, XY at, XY tileSize, XY tileCount, uint32_t color1, uint32_t color2, bool reverse)
{
    for (int tileY = 0; tileY < tileCount.y; tileY++) {
        for (int tileX = 0; tileX < tileCount.x; tileX++) {
            uint32_t color = (tileY + tileX + (reverse?1:0)) % 2 == 0 ? color1 : color2;
            XY ulc = xyAdd(at, { tileSize.x * tileX, tileSize.y * tileY });
            layer->fillRect(ulc, xyAdd(ulc, tileSize), color);
        }
    }
}

MainEditor* LegacyTemplate::generateSession()
{
    Layer* genLayer = generate();
    if (genLayer != NULL) {
        MainEditor* ret = genLayer->isPalettized ? new MainEditorPalettized((LayerPalettized*)genLayer) : new MainEditor(genLayer);
        std::vector<CommentData> templateComments = placeComments();
        for (CommentData& comment : templateComments) {
            ret->getCommentStack().push_back(comment);
        }
        ret->ssne.tileDimensions = tileSize();
        ret->ssne.tileGridPaddingBottomRight = tilePadding();
        return ret;
    }
    return NULL;
}
