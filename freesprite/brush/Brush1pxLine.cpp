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

    XY endPos = pos;

    std::function<void(XY)> rasterizeFn = [&](XY p) {
        int tsize = size;
        if (gradualSize) {
            tsize = (int)round(size * (xyDistance(p, endPos) / xyDistance(startPos, endPos)));
        }
        rasterizePoint(p, tsize, [&](XY pp) {
            editor->SetPixel(pp, editor->getActiveColor());
            }, roundB);
        };

    if (g_shiftModifier) {
        auto snap = getSnappedPointV2(startPos, endPos);
        if (snap.aspect != 0) {
            snap.to = endPos;
        }

        rasterizeSnappedLine(snap, rasterizeFn);
    }
    else {
        rasterizeLine(startPos, endPos, rasterizeFn);
    }
    dragging = false;
}

void Brush1pxLine::renderOnCanvas(MainEditor* editor, int scale)
{
    int size = (int)(editor->toolProperties["brush.pxline.size"]);
    bool round = editor->toolProperties["brush.pxline.round"] == 1;

    XY canvasDrawPoint = editor->canvas.currentDrawPoint;
    if (dragging) {

        XY endPos = lastMouseMotionPos;

        std::function<void(XY)> rasterizeFn = [&](XY a) {
            if (xyEqual(a, startPos) || xyEqual(a, endPos)) {
                rasterizePoint(a, size, [&](XY p) {
                    drawSelectedPoint(editor, p);
                    }, round);
            }
            else {
                drawSelectedPoint(editor, a);
            }
        };

        if (g_shiftModifier) {
            auto snap = getSnappedPointV2(startPos, lastMouseMotionPos);
            
            g_ttp->addTooltip(Tooltip{ editor->canvas.canvasPointToScreenPoint(startPos), frmt("{}:{}", snap.divOne ? snap.aspect : 1, snap.divOne ? 1 : snap.aspect) });
            //getSnappedPoint(startPos, lastMouseMotionPos);
            if (snap.aspect != 0) {
                snap.to = lastMouseMotionPos;
            }

            endPos = snap.to;

            rasterizeSnappedLine(snap, rasterizeFn);
        }
        else {
            rasterizeLine(startPos, endPos, rasterizeFn);
        }
    }
    else {
        rasterizePoint(lastMouseMotionPos, size, [&](XY p) {
            drawSelectedPoint(editor, p);
        }, round);
    }
}
