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
                    evt.motion.xrel * (g_shiftModifier ? 2 : 1),
                    evt.motion.yrel * (g_shiftModifier ? 2 : 1)
                });
            }
            break;
        case SDL_MOUSEWHEEL:
            zoom(evt.wheel.y > 0 ? 1 : -1);
            break;
    }
    return false;
}

void Canvas::lockToScreenBounds()
{
    currentDrawPoint = XY{
        iclamp(-dimensions.x * scale + 4, currentDrawPoint.x, g_windowW - 4),
        iclamp(-dimensions.y * scale + 4, currentDrawPoint.y, g_windowH - 4)
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

void Canvas::zoom(int how_much)
{
    XY screenCenterPoint = XY{
            (currentDrawPoint.x - g_windowW / 2) / -scale,
            (currentDrawPoint.y - g_windowH / 2) / -scale
    };
    scale += how_much;
    scale = scale < 1 ? 1 : scale;
    XY onscreenPointNow = XY{
        currentDrawPoint.x + screenCenterPoint.x * scale,
        currentDrawPoint.y + screenCenterPoint.y * scale
    };
    XY pointDiff = xySubtract(XY{ g_windowW / 2, g_windowH / 2 }, onscreenPointNow);
    currentDrawPoint = xyAdd(currentDrawPoint, pointDiff);
}

void Canvas::panCanvas(XY by)
{
    currentDrawPoint.x += by.x;
    currentDrawPoint.y += by.y;
}

void Canvas::recenter()
{
    currentDrawPoint = XY{
        (g_windowW / 2) - (dimensions.x * scale) / 2,
        (g_windowH / 2) - (dimensions.y * scale) / 2
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
    XY canvasPoint = xySubtract(screenPoint, currentDrawPoint);
    canvasPoint = { canvasPoint.x / scale, canvasPoint.y / scale };
    return xySubtract(canvasPoint, { canvasPoint.x < 0 ? 1 : 0, canvasPoint.y < 0 ? 1 : 0 });
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
