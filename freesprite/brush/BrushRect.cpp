#include "BrushRect.h"

void BrushRect::clickPress(MainEditor* editor, XY pos)
{
    heldDown = true;
    lastMousePos = pos;
    startPos = pos;
}

void BrushRect::clickRelease(MainEditor* editor, XY pos)
{
    if (heldDown) {
        heldDown = false;

        int size = (int)(editor->toolProperties["brush.pxrect.size"]);
        bool round = editor->toolProperties["brush.pxrect.round"] == 1;

        pos = g_shiftModifier ? getSnappedPoint(startPos, pos) : pos;

        g_startNewOperation([this, editor, pos, round, size]() {
            for (auto& [start, end] :
                std::vector<std::pair<XY, XY>>{
                    {startPos, XY{ pos.x, startPos.y }},
                    {startPos, XY{ startPos.x, pos.y }},
                    {pos, XY{ pos.x, startPos.y }},
                    {pos, XY{ startPos.x, pos.y }}
                })
            {
                rasterizeLine(start, end, [&](XY p) {
                    rasterizePoint(p, size, [&](XY pp) {
                        editor->SetPixel(pp, editor->getActiveColor());
                    }, round);
                });
            }
        });
    }
}

void BrushRect::renderOnCanvas(MainEditor* editor, int scale)
{
    XY canvasDrawPoint = editor->canvas.currentDrawPoint;

    if (heldDown) {
        int size = (int)(editor->toolProperties["brush.pxrect.size"]);
        bool round = editor->toolProperties["brush.pxrect.round"] == 1;

        XY endPoint = g_shiftModifier ? getSnappedPoint(startPos, lastMouseMotionPos) : lastMouseMotionPos;
        drawPixelRect(startPos, endPoint, canvasDrawPoint, scale);

        for (XY& p : 
            std::vector<XY>{
                startPos, endPoint,
                {startPos.x, endPoint.y}, 
                {endPoint.x, startPos.y}
            }) 
        {
            rasterizePoint(p, size, [&](XY pp) {
                drawSelectedPoint(editor, pp);
            }, round);
        }
    }

    drawSelectedPoint(editor, lastMouseMotionPos);
}
