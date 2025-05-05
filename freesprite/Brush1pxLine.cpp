#include "Brush1pxLine.h"
#include "maineditor.h"
#include "mathops.h"
#include "TooltipsLayer.h"

void Brush1pxLine::clickPress(MainEditor* editor, XY pos)
{
    startPos = pos;
    dragging = true;
}

void Brush1pxLine::clickRelease(MainEditor* editor, XY pos)
{
    editor->DrawLine(startPos, g_shiftModifier ? getSnappedPoint(startPos, pos) : pos, editor->getActiveColor());
    dragging = false;
}

void Brush1pxLine::renderOnCanvas(MainEditor* editor, int scale)
{
    XY canvasDrawPoint = editor->canvas.currentDrawPoint;
    if (dragging) {
        rasterizeLine(startPos, g_shiftModifier ? getSnappedPoint(startPos, lastMouseMotionPos) : lastMouseMotionPos, [&](XY a) {
            drawSelectedPoint(editor, a);
        });
    }
    else {
        drawSelectedPoint(editor, lastMouseMotionPos);
    }
}
