#include "Brush1x1.h"
#include "maineditor.h"
#include "MainEditorPalettized.h"

void Brush1x1::clickPress(MainEditor* editor, XY pos) {
    int size = (int)(editor->toolProperties["brush.squarepixel.size"]
        * (editor->toolProperties["brush.squarepixel.pressuresens"] == 1 ? editor->penPressure : 1.0));
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            editor->SetPixel(xyAdd(pos, {i,j}), editor->getActiveColor());
        }
    }
}

void Brush1x1::clickDrag(MainEditor* editor, XY from, XY to) {
    rasterizeLine(from, to, [&](XY a) { clickPress(editor, a); });
}

void Brush1x1::renderOnCanvas(MainEditor* editor, int scale) {
    XY canvasDrawPoint = editor->canvas.currentDrawPoint;
    int size = (int)(editor->toolProperties["brush.squarepixel.size"]);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            XY p = xyAdd(lastMouseMotionPos, { i,j });

            drawSelectedPoint(editor, p);
        }
    }
}

void Brush1x1PixelPerfect::clickPress(MainEditor* editor, XY pos)
{
    dragging = true;
    lastPoint = pos;
    lastTrailPoint = pos;
    hasTrailPoint = false;
    editor->SetPixel(pos, editor->getActiveColor());
}

void Brush1x1PixelPerfect::clickDrag(MainEditor* editor, XY from, XY to)
{
    XY diff = xySubtract(to, lastPoint);
    if (abs(diff.x) > 1 || abs(diff.y) > 1) {
        //editor->SetPixel(lastTrailPoint, editor->getActiveColor());
        editor->DrawLine(lastPoint, lastTrailPoint, editor->getActiveColor());
        lastPoint = lastTrailPoint;
        lastTrailPoint = to;

        diff = xySubtract(to, lastPoint);
    }

    if (!xyEqual(diff, { 0,0 })
        && !xyEqual({ abs(diff.x), diff.y }, {1,0})
        && !xyEqual({ diff.x, abs(diff.y) }, {0,1})
        ) 
    {
        //editor->SetPixel(to, editor->getActiveColor());
        editor->DrawLine(lastPoint, to, editor->getActiveColor());
        lastPoint = to;
    }
    else {
        hasTrailPoint = true;
        lastTrailPoint = to;
    }
}

void Brush1x1PixelPerfect::clickRelease(MainEditor* editor, XY pos)
{
    if (dragging) {
        editor->SetPixel(pos, editor->getActiveColor());
    }
    hasTrailPoint = false;
    dragging = false;
}

void Brush1x1Burst::clickPress(MainEditor* editor, XY pos)
{
    dragging = true;
    lastPoint = pos;
    lastTrailPoint = pos;
    hasTrailPoint = false;
    editor->SetPixel(pos, editor->getActiveColor());
}

void Brush1x1Burst::clickDrag(MainEditor* editor, XY from, XY to)
{
    XY diff = xySubtract(to, lastPoint);
    if (abs(diff.x) > 1 || abs(diff.y) > 1 || rightHeld) {
        //editor->SetPixel(lastTrailPoint, editor->getActiveColor());
        editor->DrawLine(lastPoint, lastTrailPoint, editor->getActiveColor());
        //lastPoint = lastTrailPoint;
        //lastTrailPoint = to;

        diff = xySubtract(to, lastPoint);
    }

    if (!rightHeld
        && !xyEqual(diff, { 0,0 })
        && !xyEqual({ abs(diff.x), diff.y }, { 1,0 })
        && !xyEqual({ diff.x, abs(diff.y) }, { 0,1 })
        )
    {
        //editor->SetPixel(to, editor->getActiveColor());
        editor->DrawLine(lastPoint, to, editor->getActiveColor());
        lastPoint = to;
    }
    else {
        hasTrailPoint = true;
        lastTrailPoint = to;
    }
}

void Brush1x1Burst::clickRelease(MainEditor* editor, XY pos)
{
    if (dragging) {
        editor->SetPixel(pos, editor->getActiveColor());
    }
    hasTrailPoint = false;
    dragging = false;
}

void Brush1x1Burst::rightClickPress(MainEditor* editor, XY pos)
{
    rightHeld = true;
}

void Brush1x1Burst::rightClickRelease(MainEditor* editor, XY pos)
{
    rightHeld = false;
}
