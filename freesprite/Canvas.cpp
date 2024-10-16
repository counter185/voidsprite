#include "Canvas.h"
#include "mathops.h"

XY Canvas::screenPointToCanvasPoint(XY screenPoint)
{
    XY canvasPoint = xySubtract(currentDrawPoint, screenPoint);
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
