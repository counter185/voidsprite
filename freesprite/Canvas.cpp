#include "Canvas.h"
#include "mathops.h"

bool Canvas::takeInput(SDL_Event evt)
{
    switch (evt.type) {
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            if (evt.button.button == SDL_BUTTON_MIDDLE) {
                middleMouseHold = evt.type == SDL_MOUSEBUTTONDOWN;
            }
            break;
        case SDL_MOUSEMOTION:
            if (middleMouseHold) {
                panCanvas({ 
                    ((int)evt.motion.xrel) * (g_shiftModifier ? 2 : 1),
                    ((int)evt.motion.yrel) * (g_shiftModifier ? 2 : 1)
                });
            }
            break;
        case SDL_MOUSEWHEEL:
            zoom(evt.wheel.y > 0 ? 1 : -1);
            break;
    }
    return false;
}

void Canvas::lockToScreenBounds(int top, int left, int bottom, int right, XY bounds)
{
    bounds = bounds.x == -1 ? XY{g_windowW, g_windowH} : bounds;
    currentDrawPoint = XY{
        iclamp(-dimensions.x * scale + 4 + left, currentDrawPoint.x, bounds.x - 4 - right),
        iclamp(-dimensions.y * scale + 4 + top, currentDrawPoint.y, bounds.y - 4 - bottom)
    };
}

bool Canvas::pointInCanvasBounds(XY point)
{
    return pointInBox(point, {0, 0, dimensions.x, dimensions.y});
}

void Canvas::drawCanvasOutline(int shades, SDL_Color c)
{
    u8 outlineAlpha = c.a;
    SDL_Rect rect = getCanvasOnScreenRect();
    for (int x = 0; x < shades + 1; x++) {
        rect.x -= 1;
        rect.y -= 1;
        rect.w += 2;
        rect.h += 2;
        SDL_SetRenderDrawColor(g_rd, c.r, c.g, c.b, outlineAlpha);
        outlineAlpha /= 2;
        SDL_RenderDrawRect(g_rd, &rect);
    }
}

SDL_Rect Canvas::getCanvasOnScreenRect()
{
    return {
        currentDrawPoint.x,
        currentDrawPoint.y,
        dimensions.x * scale,
        dimensions.y * scale
    };
}

void Canvas::drawTileGrid(XY tileSize)
{
    SDL_Rect origin = getCanvasOnScreenRect();
    if (tileSize.x > 0) {
        for (int x = 0; x < dimensions.x; x += tileSize.x) {
            SDL_RenderDrawLine(g_rd, origin.x + x * scale, origin.y, origin.x + x * scale, origin.y + dimensions.y * scale);
        }
    }
    if (tileSize.y > 0) {
        for (int y = 0; y < dimensions.y; y += tileSize.y) {
            SDL_RenderDrawLine(g_rd, origin.x, origin.y + y * scale, origin.x + dimensions.x * scale, origin.y + y * scale);
        }
    }
}

void Canvas::zoom(int how_much, XY centerPoint)
{
    XYf screenCenterPoint = XYf{
        (currentDrawPoint.x - centerPoint.x) / (float)-scale,
        (currentDrawPoint.y - centerPoint.y) / (float)-scale
    };
    scale += how_much;
    scale = ixmax(scale, minScale);
    XY onscreenPointNow = XY{
        (int)(currentDrawPoint.x + screenCenterPoint.x * scale),
        (int)(currentDrawPoint.y + screenCenterPoint.y * scale)
    };
    XY pointDiff = xySubtract(centerPoint, onscreenPointNow);
    currentDrawPoint = xyAdd(currentDrawPoint, pointDiff);
}

void Canvas::panCanvas(XY by)
{
    currentDrawPoint.x += by.x;
    currentDrawPoint.y += by.y;
}

void Canvas::recenter(XY windowDimensions)
{
    scale = ixmax(scale, minScale);
    windowDimensions = windowDimensions.x == -1 ? XY{g_windowW, g_windowH} : windowDimensions;
    currentDrawPoint = XY{
        (windowDimensions.x / 2) - (dimensions.x * scale) / 2,
        (windowDimensions.y / 2) - (dimensions.y * scale) / 2
    };
}

XY Canvas::canvasPointToScreenPoint(XY canvasPoint)
{
    return {
        currentDrawPoint.x + canvasPoint.x * scale,
        currentDrawPoint.y + canvasPoint.y * scale
    };
}

SDL_Rect Canvas::canvasRectToScreenRect(SDL_Rect canvasRect)
{
    XY screenPoint = canvasPointToScreenPoint({ canvasRect.x, canvasRect.y });
    return { 
        screenPoint.x, screenPoint.y, 
        canvasRect.w * scale, canvasRect.h * scale 
    };
}

XY Canvas::screenPointToCanvasPoint(XY screenPoint)
{
    XY relPoint = xySubtract(screenPoint, currentDrawPoint);
    XY canvasPoint = { relPoint.x / scale, relPoint.y / scale };
    return xySubtract(canvasPoint, { relPoint.x < 0 ? 1 : 0, relPoint.y < 0 ? 1 : 0 });
}

XY Canvas::getTilePosAt(XY screenPoint, XY tileSize)
{
    tileSize.x = ixmax(1, tileSize.x);
    tileSize.y = ixmax(1, tileSize.y);
    XY canvasPoint = screenPointToCanvasPoint(screenPoint);
    canvasPoint = { canvasPoint.x / tileSize.x, canvasPoint.y / tileSize.y };
    return canvasPoint;
}

SDL_Rect Canvas::getTileScreenRectAt(XY canvasTileIndex, XY tileSize)
{
    return {
        currentDrawPoint.x + canvasTileIndex.x * tileSize.x * scale,
		currentDrawPoint.y + canvasTileIndex.y * tileSize.y * scale,
		tileSize.x * scale,
		tileSize.y * scale
    };
}

SDL_Rect Canvas::getTileRectAt(XY canvasTileIndex, XY tileSize)
{
    return {
        canvasTileIndex.x * tileSize.x,
        canvasTileIndex.y * tileSize.y,
        tileSize.x,
        tileSize.y
    };
}
