#include "BrushRectFill.h"
#include "../background_operation.h"

void BrushRectFill::clickPress(MainEditor* editor, XY pos)
{
    heldDown = true;
    lastMousePos = pos;
    startPos = pos;
}

void BrushRectFill::clickRelease(MainEditor* editor, XY pos)
{
    if (heldDown) {
        heldDown = false;
        if (xyEqual(startPos, lastMousePos) && clickTimer.started && clickTimer.elapsedTime() < BRUSH_DOUBLE_CLICK_TIME) {
            XY tileDimensions = editor->tileDimensions;
            tileDimensions.x = tileDimensions.x == 0 ? editor->canvas.dimensions.x : tileDimensions.x;
            tileDimensions.y = tileDimensions.y == 0 ? editor->canvas.dimensions.y : tileDimensions.y;

            XY tileIndex = {
                startPos.x / tileDimensions.x,
                startPos.y / tileDimensions.y
            };
            SDL_Rect tileRect = editor->canvas.getTileRectAt(tileIndex, tileDimensions);
            g_startNewOperation([this, editor, tileRect]() {
                fillArea({ tileRect.x, tileRect.y }, { tileRect.x + tileRect.w - 1, tileRect.y + tileRect.h - 1}, editor);
            });

        } else {
            pos = g_shiftModifier ? getSnappedPoint(startPos, pos) : pos;
            g_startNewOperation([this, editor, pos]() {
                int minx = ixmin(pos.x, startPos.x);
                int maxx = ixmax(pos.x, startPos.x);
                int miny = ixmin(pos.y, startPos.y);
                int maxy = ixmax(pos.y, startPos.y);

                fillArea({ minx, miny }, { maxx, maxy }, editor);
            });
        }
        clickTimer.start();
    }
}

void BrushRectFill::renderOnCanvas(XY canvasDrawPoint, int scale)
{
    if (heldDown) {
        drawPixelRect(startPos, g_shiftModifier ? getSnappedPoint(startPos, lastMouseMotionPos) : lastMouseMotionPos, canvasDrawPoint, scale);
    }

    SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
    drawLocalPoint(canvasDrawPoint, lastMouseMotionPos, scale);
    SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
    drawPointOutline(canvasDrawPoint, lastMouseMotionPos, scale);
}

void BrushRectFill::fillArea(XY from, XY to, MainEditor* editor) {
    for (int x = from.x; x <= to.x; x++) {
        for (int y = from.y; y <= to.y; y++) {
            editor->SetPixel(XY{ x,y }, editor->getActiveColor());
        }
    }
    editor->getCurrentLayer()->bgOpFinished = true;
}
