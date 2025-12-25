#include "BrushBezierLine.h"

void BrushBezierLine::clickPress(MainEditor* editor, XY pos)
{
    for (int x = 0; x < points.size(); x++) {
        XY p = points[x];
        if (xyEqual(pos, p)) {
            dragging = x;
            return;
        }
    }
    points.push_back(pos);
}

void BrushBezierLine::clickDrag(MainEditor* editor, XY from, XY to)
{
    if (dragging != -1) {
        points[dragging] = to;
    }
}

void BrushBezierLine::clickRelease(MainEditor* editor, XY pos)
{
    if (dragging != -1) {
        points[dragging] = pos;
    }
    dragging = -1;
}

void BrushBezierLine::rightClickPress(MainEditor* editor, XY pos)
{
    if (points.size() > 1) {
        editor->commitStateToCurrentLayer();
        int size = (int)(editor->toolProperties["brush.bezierline.size"]);
        bool roundB = editor->toolProperties["brush.bezierline.round"] == 1;
        bool gradualSize = editor->toolProperties["brush.bezierline.gradsize"] == 1;

        XY startPos = points.front();

        double longestDist = 0;
        XY endPos = startPos;
        for (auto& p : points) {
            double dist = xyDistance(startPos, p);
            if (dist > longestDist) {
                longestDist = dist;
                endPos = p;
            }
        }

        rasterizeBezierCurve(points, [&](XY a) {
            int tsize = size;
            if (gradualSize) {
                tsize = (int)round(size * dxmax(0, xyDistance(a, endPos) / xyDistance(startPos, endPos)));
            }
            rasterizePoint(a, tsize, [&](XY p) {
                editor->SetPixel(p, editor->getActiveColor());
            }, roundB);
        });
    }

    dragging = -1;
    points.clear();
}

void BrushBezierLine::renderOnCanvas(MainEditor* editor, int scale)
{
    XY canvasDrawPoint = editor->canvas.currentDrawPoint;

    rasterizeBezierCurve(points, [&](XY a) {
        drawSelectedPoint(editor, a);
    });

    for (XY& p : points) {
        drawSelectedPoint(editor, p);
    }

    for (int x = 0; x < (int)points.size() - 1; x++) {
        XY osp1 = xyAdd(canvasDrawPoint, { points[x].x * scale, points[x].y * scale});
        osp1 = xyAdd(osp1, { scale / 2,scale / 2 });

        XY osp2 = xyAdd(canvasDrawPoint, { points[x + 1].x * scale, points[x + 1].y * scale});
        osp2 = xyAdd(osp2, { scale / 2,scale / 2 });

        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0xa0);
        SDL_RenderDrawLine(g_rd, osp1.x, osp1.y, osp2.x, osp2.y);
    }

    drawSelectedPoint(editor, lastMouseMotionPos);
}
