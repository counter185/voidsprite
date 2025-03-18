#include "BrushFill.h"
#include "maineditor.h"
#include <algorithm>
#include "background_operation.h"
#include "FontRenderer.h"

bool BrushFill::closedListContains(XY a)
{
    return previewClosedList.pointExists(a);
    /*for (int x = previewClosedList.size(); x--> 0;) {
        if (xyEqual(a, previewClosedList[x])) {
            return true;
        }
    }
    return false;*/
}

void BrushFill::resetState() 
{ 
    timeStarted = SDL_GetTicks64();
    timeNextIter = timeStarted;
    previewIterations = 0;
    previewOpenList.clear();
    previewClosedList.clear();
    previewXBList.clear();
}

void BrushFill::clickPress(MainEditor* editor, XY pos)
{
    g_startNewOperation([this, editor, pos]() {
        (void) this;

        Layer* currentLayer = editor->getCurrentLayer();
        uint32_t pixel = currentLayer->getPixelAt(pos);
        uint32_t swapTo = editor->eraserMode ? (editor->isPalettized ? -1 : 0x00000000) : editor->getActiveColor();

        if (pixel == swapTo) {
            return;
        }

        XY tilePos = editor->tileDimensions.x == 0
            ? XY{ 0,0 }
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
                    ? XY{ 0,0 }
                    : XY{
                        openListElement.x / editor->tileDimensions.x,
                        openListElement.y / editor->tileDimensions.y,
                };
                if (editor->isInBounds(openListElement) && (pixelRn == pixel || (!editor->isPalettized && pixelRn >> 24 == 0 && pixel >> 24 == 0))
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
    });
}

void BrushFill::clickDrag(MainEditor*, XY, XY)
{
}

void BrushFill::rightClickPress(MainEditor* editor, XY pos)
{
    (void) pos;

    editor->commitStateToCurrentLayer();
    uint32_t swapTo = editor->eraserMode ? (editor->isPalettized ? -1 : 0x00000000) : editor->getActiveColor();
    previewClosedList.forEachScanline([editor, swapTo](ScanlineMapElement sme) {
        for (int x = 0; x < sme.size.x; x++) {
            editor->SetPixel(xyAdd(sme.origin, {x,0}), swapTo);
        }
    });
    /*for (XY& t : previewClosedList) {
        editor->SetPixel({ t }, swapTo);
    }*/
    for (XY& t : previewOpenList) {
        editor->SetPixel({ t }, swapTo);
    }
}

void BrushFill::renderOnCanvas(MainEditor* editor, int scale) {

    previewXBList.iPromiseNotToPutDuplicatePoints = true;
    XY canvasDrawPoint = editor->canvas.currentDrawPoint;
    if ((editor != lastEditor || !xyEqual(lastMouseMotionPos, previewLastPosition))
        && !previewClosedList.pointExists(lastMouseMotionPos) /*&&  std::find_if(previewClosedList.begin(), previewClosedList.end(), [this](XY a) { return xyEqual(a, lastMouseMotionPos); })
            == previewClosedList.end()*/) {
        lastEditor = editor;
        previewLastPosition = lastMouseMotionPos;
        previewSearchingColor = editor->getCurrentLayer()->getPixelAt(previewLastPosition);

        timeStarted = SDL_GetTicks64();
        timeNextIter = timeStarted;
        previewIterations = 0;
        previewOpenList.clear();
        previewClosedList.clear();
        previewXBList.clear();
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
                previewClosedList.addPoint(openListElement);
                
                if (previewXBList.pointCount < 256000) {
                    previewXBList.addPoint(openListElement.x, openListElement.y - openListElement.x);
                }
            }
        }
        previewOpenList = nextList;
        previewIterations++;
    }
    //g_fnt->RenderString(std::to_string(previewXBList.pointCount), 50, 50);
    hsl hslC = rgb2hsl(u32ToRGB(previewSearchingColor));
    u32 previewColor = hslC.l > 0.7 ? 0x000000 : 0xFFFFFF;
    uint64_t distanceTicks = SDL_GetTicks64() - timeStarted;
    double distanceTime = dxmax(1, distanceTicks / 16.0);

    previewClosedList.forEachScanline([this, editor, canvasDrawPoint, scale, distanceTime, previewColor](ScanlineMapElement sme) {

        SDL_Rect r = { canvasDrawPoint.x + sme.origin.x * scale, canvasDrawPoint.y + sme.origin.y * scale, scale * sme.size.x, scale };
        if (!((r.y < g_windowH || (r.y + r.h) > 0))) {
            return;
        }

        //double alphaModifier = xyDistance(statLineEndpoint({0,1}, sme.size, 0.5), previewLastPosition) / distanceTime;
        double alphaModifierAtLeft = xyDistance(sme.origin, previewLastPosition) / distanceTime;
        double alphaModifierAtCenter = xyDistance(xyAdd(sme.origin, {sme.size.x/2, sme.size.y/2}), previewLastPosition) / distanceTime;
        double alphaModifierAtRight = xyDistance(xyAdd(sme.origin, sme.size), previewLastPosition) / distanceTime;
        uint8_t alphaL = ixmax(0, 0xd0 * dxmin(alphaModifierAtLeft, 1));
        uint8_t alphaC = ixmax(0, 0xd0 * dxmin(alphaModifierAtCenter, 1));
        uint8_t alphaR = ixmax(0, 0xd0 * dxmin(alphaModifierAtRight, 1));
        
        //u32 baseColor = scale > 5 ? 0xFFFFFF : 0x707070;
        u32 colL = (alphaL << 24) | previewColor;
        u32 colC = (alphaC << 24) | previewColor;
        u32 colR = (alphaR << 24) | previewColor;

        //SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, alpha);
        
        if (sme.size.x >= 2) {
            SDL_Rect r1 = { r.x, r.y, r.w / 2, r.h };
            SDL_Rect r2 = { r.x + r.w / 2, r.y, r.w / 2, r.h };
            renderGradient(r1, colL, colC, colL, colC);
            renderGradient(r2, colC, colR, colC, colR);
        }
        else {
            SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, alphaC);
            SDL_RenderFillRect(g_rd, &r);
        }
        if (scale > 5) {
            //SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x40);
            
            for (int x = 0; x < sme.size.x; x++) {
                XY point = xyAdd(sme.origin, { x,0 });

                int b = point.y - point.x;
                //xbmapPtr->addPoint(point.x, b);

                /*SDL_Rect r = {canvasDrawPoint.x + point.x * scale, canvasDrawPoint.y + point.y * scale, scale, scale};
                bool cPre = r.x + r.w >= 0;
                bool cPost = r.x < g_windowW;
                if (!cPost) {
                    break;  // stop because we're outside the window
                }
                if (cPre) {
                    SDL_RenderDrawLine(g_rd, r.x, r.y, r.x + scale - 1, r.y + scale - 1);
                }*/
            }
        }
    });
    int lineDrawcalls = 0;
    int* ll = &lineDrawcalls;
    if (scale >= 2) {
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x40);
        previewXBList.forEachScanline([canvasDrawPoint, scale, ll](IntLineMapElement ilme, int b) {
            if (scale <= 4 && ((b & 0b11) != 0)) {
                return;
            }
            else if (scale < 9 && ((b & 1) != 0)) {
                return;
            }
            XY pointOrigin = { ilme.x, b + ilme.x };
            SDL_Rect r = { canvasDrawPoint.x + pointOrigin.x * scale, canvasDrawPoint.y + pointOrigin.y * scale, scale * ilme.w, scale * ilme.w };
            
            //test bottom left and top right intersect
            int bb = r.y - r.x;

            XY origin = { r.x, r.y };
            if (origin.y < g_windowH && bb < g_windowH && (g_windowW + bb) >= 0) {
                XY endpoint = { r.x + r.w - 1 , r.y + r.w - 1 };

                if (endpoint.y > g_windowH) {
                    int diff = endpoint.y - g_windowH;
                    endpoint.y -= diff;
                    endpoint.x -= diff;
                }
                if (origin.x < 0) {
                    int diff = 0 - origin.x;
                    origin.x += diff;
                    origin.y += diff;
                }

                SDL_RenderDrawLine(g_rd, origin.x, origin.y, endpoint.x, endpoint.y);
                (*ll)++;
            }
        });
    }
    //g_fnt->RenderString(std::format("Line DCs {}", lineDrawcalls), 50, 50);

    SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
    drawLocalPoint(canvasDrawPoint, lastMouseMotionPos, scale);
    SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
    drawPointOutline(canvasDrawPoint, lastMouseMotionPos, scale);
}
