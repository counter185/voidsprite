#include "Brush1pxLine.h"

void Brush1pxLine::clickPress(MainEditor* editor, XY pos)
{
    startPos = pos;
    dragging = true;
}

void Brush1pxLine::clickRelease(MainEditor* editor, XY pos)
{
    int size = (int)(editor->toolProperties["brush.pxline.size"]);
    bool roundB = editor->toolProperties["brush.pxline.round"] == 1;
    bool gradualSize = editor->toolProperties["brush.pxline.gradsize"] == 1;

    XY endPos = g_shiftModifier ? getSnappedPoint(startPos, pos) : pos;
    rasterizeLine(startPos, endPos, [&](XY p) {
        int tsize = size;
        if (gradualSize) {
            tsize = (int)round(size * (xyDistance(p, endPos) / xyDistance(startPos, endPos)));
        }
        rasterizePoint(p, tsize, [&](XY pp) {
            editor->SetPixel(pp, editor->getActiveColor());
        }, roundB);
    });
    dragging = false;
}

void Brush1pxLine::renderOnCanvas(MainEditor* editor, int scale)
{
    int size = (int)(editor->toolProperties["brush.pxline.size"]);
    bool round = editor->toolProperties["brush.pxline.round"] == 1;

    XY canvasDrawPoint = editor->canvas.currentDrawPoint;
    if (dragging) {
        /*std::map<u64, bool> ps;
        rasterizeLine(startPos, g_shiftModifier ? getSnappedPoint(startPos, lastMouseMotionPos) : lastMouseMotionPos, [&](XY a) {
            rasterizePoint(a, size, [&](XY p) {
                ps[encodeXY(p)] = true;
            }, round);
        });

        for (auto& [encxy, _] : ps) {
            drawSelectedPoint(editor, decodeXY(encxy));
        }*/
        //rasterizing the whole line with  thickness is
        //very slow

        XY endPos = g_shiftModifier ? getSnappedPoint(startPos, lastMouseMotionPos) : lastMouseMotionPos;
        rasterizeLine(startPos, endPos, [&](XY a) {
            if (xyEqual(a, startPos) || xyEqual(a, endPos)) {
                rasterizePoint(a, size, [&](XY p) {
                    drawSelectedPoint(editor, p);
                }, round);
            }
            else {
                drawSelectedPoint(editor, a);
            }
        });
    }
    else {
        rasterizePoint(lastMouseMotionPos, size, [&](XY p) {
            drawSelectedPoint(editor, p);
        }, round);
    }
}
