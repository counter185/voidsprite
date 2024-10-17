#include "BrushFill.h"
#include "maineditor.h"

bool BrushFill::closedListContains(XY a)
{
    for (int x = previewClosedList.size(); x --> 0;) {
        if (xyEqual(a, previewClosedList[x])) {
            return true;
        }
    }
    return false;
}

void BrushFill::clickPress(MainEditor* editor, XY pos)
{
    Layer* currentLayer = editor->getCurrentLayer();
    uint32_t pixel = currentLayer->getPixelAt(pos);
    uint32_t swapTo = editor->eraserMode ? (editor->isPalettized ? -1 : 0x00000000) : editor->getActiveColor();

    if (pixel == swapTo) {
        return;
    }

    XY tilePos = editor->tileDimensions.x == 0
        ? XY{0,0}
        : XY{
            pos.x / editor->tileDimensions.x,
            pos.y / editor->tileDimensions.y,
        };

    std::vector<XY> openList;
    openList.push_back(pos);
    std::vector<XY> nextList;
    while (!openList.empty()) {
        for (XY& openListElement : openList) {
            uint32_t pixelRn = currentLayer->getPixelAt(openListElement);
            XY tilePosSelected = editor->tileDimensions.x == 0
                ? XY{0,0}
                : XY{
                    openListElement.x / editor->tileDimensions.x,
                    openListElement.y / editor->tileDimensions.y,
                };
            if (editor->isInBounds(openListElement) && (pixelRn == pixel || (!editor->isPalettized && pixelRn>>24 == 0 && pixel>>24 == 0))
                && (editor->tileDimensions.x == 0 || !g_config.fillToolTileBound || xyEqual(tilePosSelected, tilePos))) {
                currentLayer->setPixel(openListElement, swapTo);
                XY p[] = {
                    {0,1},
                    {0,-1},
                    {1,0},
                    {-1,0}
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

void BrushFill::clickDrag(MainEditor*, XY, XY)
{
}

void BrushFill::rightClickPress(MainEditor* editor, XY pos)
{
    editor->commitStateToCurrentLayer();
    uint32_t swapTo = editor->eraserMode ? (editor->isPalettized ? -1 : 0x00000000) : editor->getActiveColor();
    for (XY& t : previewClosedList) {
        editor->SetPixel({ t }, swapTo);
    }
    for (XY& t : previewOpenList) {
        editor->SetPixel({ t }, swapTo);
    }
}

void BrushFill::renderOnCanvas(MainEditor* editor, int scale) {

    XY canvasDrawPoint = editor->canvas.currentDrawPoint;
    if (editor != lastEditor || !xyEqual(lastMouseMotionPos, previewLastPosition)) {
        lastEditor = editor;
        previewLastPosition = lastMouseMotionPos;
        previewSearchingColor = editor->getCurrentLayer()->getPixelAt(previewLastPosition);

        timeStarted = SDL_GetTicks64();
        timeNextIter = timeStarted;
        previewIterations = 0;
        previewOpenList.clear();
        previewClosedList.clear();
        previewOpenList.push_back(previewLastPosition);
    }
    if (editor != NULL 
        && editor->isInBounds(previewLastPosition) 
        && g_deltaTime < 0.018
        && SDL_GetTicks64() > timeNextIter + 16) {
        timeNextIter = SDL_GetTicks64();
        XY tilePos = editor->tileDimensions.x == 0
            ? XY{0,0}
            : XY{
                lastMouseMotionPos.x / editor->tileDimensions.x,
                lastMouseMotionPos.y / editor->tileDimensions.y,
            };

        std::vector<XY> nextList;
        for (XY& openListElement : previewOpenList) {
            uint32_t pixelRn = editor->getCurrentLayer()->getPixelAt(openListElement);
            XY tilePosSelected = editor->tileDimensions.x == 0
                ? XY{0,0}
                : XY{
                    openListElement.x / editor->tileDimensions.x,
                    openListElement.y / editor->tileDimensions.y,
                };
            if (editor->isInBounds(openListElement)
                && (pixelRn == previewSearchingColor || (!editor->isPalettized && pixelRn >> 24 == 0 && previewSearchingColor >> 24 == 0))
                && !closedListContains(openListElement)
                && (editor->tileDimensions.x == 0 || !g_config.fillToolTileBound || xyEqual(tilePosSelected, tilePos))) {
                XY p[] = {
                    {0,1},
                    {0,-1},
                    {1,0},
                    {-1,0}
                };
                for (XY& pp : p) {
                    nextList.push_back(xyAdd(openListElement, pp));
                }
                previewClosedList.push_back(openListElement);
            }
        }
        previewOpenList = nextList;
        previewIterations++;
    }
    uint64_t distanceTicks = SDL_GetTicks64() - timeStarted;
    double distanceTime = dxmax(1, distanceTicks / 16.0);
    for (XY& cle : previewClosedList) {
        //double alphaModifier = dxmin((distanceTicks / 32.0) / dxmax(xyDistance(cle, previewLastPosition), 1), 1);
        double alphaModifier = xyDistance(cle, previewLastPosition) / distanceTime;
        uint8_t alpha = ixmax(0, 0xd0 * dxmin(alphaModifier, 1));

        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, alpha);
        drawLocalPoint(canvasDrawPoint, cle, scale);
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x40);
        drawPointStrikethrough(canvasDrawPoint, cle, scale);
    }

    SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
    drawLocalPoint(canvasDrawPoint, lastMouseMotionPos, scale);
    SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
    drawPointOutline(canvasDrawPoint, lastMouseMotionPos, scale);
}
