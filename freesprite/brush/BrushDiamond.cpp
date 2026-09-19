#include "BrushDiamond.h"

void BrushDiamond::clickPress(MainEditor* editor, XY pos)
{
    startPos = pos;
    heldDown = true;
}

void BrushDiamond::clickRelease(MainEditor* editor, XY pos)
{
    pos = g_shiftModifier ? getSnappedPoint(startPos, pos) : pos;

    XY posMin = { ixmin(pos.x, startPos.x), ixmin(pos.y, startPos.y) };
    XY posMax = { ixmax(pos.x, startPos.x), ixmax(pos.y, startPos.y) };

    int size = (int)(editor->toolProperties["brush.diamond.size"]);
    bool round = editor->toolProperties["brush.diamond.round"] == 1;

    rasterizeDiamond(posMin, posMax, [&](XY a) {
        rasterizePoint(a, size, [&](XY p) {
            editor->SetPixel(p, editor->getActiveColor());
        }, round);
    });
    heldDown = false;
}

void BrushDiamond::renderOnCanvas(MainEditor* editor, int scale)
{
    if (heldDown) {
        drawSelectedPoint(editor, startPos);

        XY pos = lastMouseMotionPos;
        pos = g_shiftModifier ? getSnappedPoint(startPos, pos) : pos;

        XY posMin = { ixmin(pos.x, startPos.x), ixmin(pos.y, startPos.y) };
        XY posMax = { ixmax(pos.x, startPos.x), ixmax(pos.y, startPos.y) };
        rasterizeDiamond(posMin, posMax, [&](XY a) {
            drawSelectedPoint(editor, a);
        });
    }
    drawSelectedPoint(editor, lastMouseMotionPos);
}
