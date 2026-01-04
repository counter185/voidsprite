#include "BrushCurve.h"

void BrushCurve::clickPress(MainEditor* editor, XY pos)
{
    dragging = true;
    switch (state) {
        case BRUSHCURVE_START:
            startPos = pos;
            break;
        case BRUSHCURVE_PLACEP1:
            p1Pos = pos;
            state = BRUSHCURVE_PLACEP2;
            break;
        case BRUSHCURVE_PLACEP2:
            p2Pos = pos;
            putLine(editor);
            state = BRUSHCURVE_START;
            dragging = false;
            break;
    }
}

void BrushCurve::clickRelease(MainEditor* editor, XY pos)
{
    if (dragging) {
        dragging = false;
        switch (state) {
            case BRUSHCURVE_START:
                endPos = pos;
                state = BRUSHCURVE_PLACEP1;
                break;
        }
    }
}

void BrushCurve::renderOnCanvas(MainEditor* editor, int scale)
{
    XY dragPoint = lastMouseMotionPos;
    switch (state) {
        case BRUSHCURVE_START:
            if (dragging) {
                rasterizeLine(startPos, dragPoint, [&](XY p) {
                    drawSelectedPoint(editor, p);
                });
            }
            else {
                drawSelectedPoint(editor, dragPoint);
            }
            break;
        case BRUSHCURVE_PLACEP1:
        {
            std::vector<XY> bezierPoints = { startPos, dragPoint, dragPoint, endPos };
            rasterizeBezierCurve(bezierPoints, [&](XY p) {
                drawSelectedPoint(editor, p);
            });
        }
            break;
        case BRUSHCURVE_PLACEP2:
        {
            std::vector<XY> bezierPoints = { startPos, p1Pos, dragPoint, endPos };
            rasterizeBezierCurve(bezierPoints, [&](XY p) {
                drawSelectedPoint(editor, p);
            });
        }
            break;
    }
}

void BrushCurve::putLine(MainEditor* editor)
{
    bool roundC = editor->toolProperties["brush.pxline.round"] == 1;
    bool gradSize = editor->toolProperties["brush.pxline.gradsize"] == 1;
    int size = (int)(editor->toolProperties["brush.pxline.size"]);

    std::vector<XY> bezierPoints = { startPos, p1Pos, p2Pos, endPos };
    std::vector<XY> outPoints;
    rasterizeBezierCurve(bezierPoints, [&](XY a) {
        outPoints.push_back(a);
    });

    double maxDistance = 0;
    for (auto& p : outPoints) {
        double dist = xyDistance(startPos, p);
        if (dist > maxDistance) {
            maxDistance = dist;
        }
    }

    for (XY& p : outPoints) {
        int tsize = gradSize ? (int)round(size * (1.0 - (xyDistance(startPos, p) / maxDistance))) : size;

        rasterizePoint(p, tsize, [&](XY pp) {
            editor->SetPixel(pp, editor->getActiveColor());
        }, roundC);
    }
}
