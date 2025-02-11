#include "BrushRectFill.h"
#include "mathops.h"
#include "maineditor.h"
#include "background_operation.h"

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
        pos = g_shiftModifier ? getSnappedPoint(startPos, pos) : pos;
        g_startNewOperation([this, editor, pos]() {
            int minx = ixmin(pos.x, startPos.x);
            int maxx = ixmax(pos.x, startPos.x);
            int miny = ixmin(pos.y, startPos.y);
            int maxy = ixmax(pos.y, startPos.y);

            for (int x = minx; x <= maxx; x++) {
                for (int y = miny; y <= maxy; y++) {
                    editor->SetPixel(XY{ x,y }, editor->getActiveColor());
                }
            }
            editor->getCurrentLayer()->bgOpFinished = true;
        });
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
