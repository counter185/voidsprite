#include "Brush1pxLine.h"

void Brush1pxLine::renderOnCanvas(MainEditor* editor, int scale)
{
    int size = (int)(editor->toolProperties["brush.pxline.size"]);
    bool round = editor->toolProperties["brush.pxline.round"] == 1;

    LineSelectBrush::defaultRenderOnCanvas(editor, size, round);
}

void Brush1pxLine::lineSelected(MainEditor* editor, XY start, XY end)
{
    int size = (int)(editor->toolProperties["brush.pxline.size"]);
    bool roundB = editor->toolProperties["brush.pxline.round"] == 1;
    bool gradualSize = editor->toolProperties["brush.pxline.gradsize"] == 1;

    std::function<void(XY)> rasterizeFn = [&](XY p) {
        int tsize = size;
        if (gradualSize) {
            tsize = (int)round(size * (xyDistance(p, end) / xyDistance(start, end)));
        }
        rasterizePoint(p, tsize, [&](XY pp) {
            editor->SetPixel(pp, editor->getActiveColor());
            }, roundB);
        };

    if (g_shiftModifier) {
        auto snap = getSnappedPointV2(start, end);
        if (snap.aspect != 0) {
            snap.to = end;
        }

        rasterizeSnappedLine(snap, rasterizeFn);
    }
    else {
        rasterizeLine(start, end, rasterizeFn);
    }
}
