#include "Brush1x1.h"

void Brush1x1::clickPress(MainEditor* editor, XY pos) {
    bool pressureSize = editor->toolProperties["brush.squarepixel.pressuresens"] == 1;
    int size = (int)(round(editor->toolProperties["brush.squarepixel.size"]
        * (pressureSize ? editor->penPressure : 1.0)));
    bool round = editor->toolProperties["brush.squarepixel.round"] == 1;

    rasterizePoint(pos, size, [&](XY p) {
        editor->SetPixel(p, editor->getActiveColor());
    }, round);
}

void Brush1x1::clickDrag(MainEditor* editor, XY from, XY to) {
    rasterizeLine(from, to, [&](XY a) { clickPress(editor, a); });
}

void Brush1x1::renderOnCanvas(MainEditor* editor, int scale) {
    XY canvasDrawPoint = editor->canvas.currentDrawPoint;
    bool pressureSize = editor->toolProperties["brush.squarepixel.pressuresens"] == 1;
    int size = (int)(round(editor->toolProperties["brush.squarepixel.size"]
        * (pressureSize ? editor->penPressure : 1.0)));
    bool round = editor->toolProperties["brush.squarepixel.round"] == 1;

    rasterizePoint(lastMouseMotionPos, size, [&](XY p) {
        drawSelectedPoint(editor, p);
    }, round);
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
