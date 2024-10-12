#include "BrushBezierLine.h"
#include "maineditor.h"

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
        rasterizeBezierCurve(points, [&](XY a) {
            editor->SetPixel(a, editor->getActiveColor());
        });
    }

    dragging = -1;
    points.clear();
}

void BrushBezierLine::renderOnCanvas(XY canvasDrawPoint, int scale)
{
    rasterizeBezierCurve(points, [&](XY a) {
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x50);
        drawLocalPoint(canvasDrawPoint, a, scale);
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x50);
        drawPointOutline(canvasDrawPoint, a, scale);
    });

    for (XY& p : points) {
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
        drawLocalPoint(canvasDrawPoint, p, scale);
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
        drawPointOutline(canvasDrawPoint, p, scale);
    }

    SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
    drawLocalPoint(canvasDrawPoint, lastMouseMotionPos, scale);
    SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
    drawPointOutline(canvasDrawPoint, lastMouseMotionPos, scale);
}
