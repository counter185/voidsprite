#include "Brush9SegmentRect.h"
#include "maineditor.h"
#include "Notification.h"

void Brush9SegmentRect::clickPress(MainEditor* editor, XY pos)
{
    heldDown = true;
    lastMousePos = pos;
    startPos = pos;
}

void Brush9SegmentRect::clickRelease(MainEditor* editor, XY pos)
{
    heldDown = false;
    pos = g_shiftModifier ? getSnappedPoint(startPos, pos) : pos;
    int minx = ixmin(pos.x, startPos.x);
    int maxx = ixmax(pos.x, startPos.x);
    int miny = ixmin(pos.y, startPos.y);
    int maxy = ixmax(pos.y, startPos.y);
    maxx++;
    maxy++;

    if (editor->isPalettized) {
        g_addNotification(ErrorNotification("Error", "Tool not supported in palettized editor."));
        return;
    }

    XY wh = XY{ maxx - minx, maxy - miny };
    for (int x = 0; x < wh.x; x++) {
        for (int y = 0; y < wh.y; y++) {
            XY fragment;
            int segXLineInRect1 = ixmin(wh.x / 2, segXLine1);
            int segXLineInRect2 = ixmax(wh.x / 2, wh.x - segXLine2) - 1;
            int segYLineInRect1 = ixmin(wh.y / 2, segYLine1);
            int segYLineInRect2 = ixmax(wh.y / 2, wh.y - segYLine2) - 1;
            fragment.y = y < segYLineInRect1 ? 0
                : y >= segYLineInRect2 ? 2
                : 1;
            fragment.x = x < segXLineInRect1 ? 0
                : x >= segXLineInRect2 ? 2
                : 1;
            XY inPatternPos = {
                fragment.x == 0 ? x
                : fragment.x == 2 ? fullPatternDims.x - (wh.x - x)
                : segXLine1 + (x - segXLine1) % (fullPatternDims.x - segXLine2 - segXLine1 - 1),
                fragment.y == 0 ? y
                : fragment.y == 2 ? fullPatternDims.y - (wh.y - y)
                : segYLine1 + (y - segYLine1) % (fullPatternDims.y - segYLine2 - segYLine1 - 1)
            };
            editor->SetPixel(XY{ minx + x, miny + y }, patternPixels[inPatternPos.y * fullPatternDims.x + inPatternPos.x]);
            /*editor->SetPixel(XY{minx + x, miny + y},
                0xFF000000 + ((0x7F * fragment.x) << 16) + (0x7F * fragment.y)
                );*/
        }
    }
}

void Brush9SegmentRect::rightClickPress(MainEditor* editor, XY pos)
{
    //open config popup here
}

void Brush9SegmentRect::renderOnCanvas(XY canvasDrawPoint, int scale)
{
    if (heldDown) {
        drawPixelRect(startPos, g_shiftModifier ? getSnappedPoint(startPos, lastMouseMotionPos) : lastMouseMotionPos, canvasDrawPoint, scale);
    }

    SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
    drawLocalPoint(canvasDrawPoint, lastMouseMotionPos, scale);
    SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
    drawPointOutline(canvasDrawPoint, lastMouseMotionPos, scale);
}
