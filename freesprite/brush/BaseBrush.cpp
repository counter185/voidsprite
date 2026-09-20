#include "BaseBrush.h"
#include "../maineditor.h"

#include "Brush1x1.h"
#include "Brush1x1ArcX.h"
#include "Brush1x1ArcY.h"
#include "Brush1pxLine.h"
#include "BrushRect.h"
#include "BrushRectFill.h"
#include "BrushFill.h"
#include "Brush1pxLinePathfind.h"
#include "BrushCircle.h"
#include "BrushCurve.h"
#include "ToolRectMove.h"
#include "Brush9SegmentRect.h"
#include "Brush1x1ArcX.h"
#include "Brush1x1ArcY.h"
#include "BrushReplaceColor.h"
#include "ToolRectFlip.h"
#include "ToolRectRotate.h"
#include "ToolRectSwap.h"
#include "ToolText.h"
#include "ToolRectIsolate.h"
#include "ToolGuideline.h"
#include "BrushBezierLine.h"
#include "BrushDiamond.h"
#include "ToolSetXSymmetry.h"
#include "ToolSetYSymmetry.h"
#include "ToolMeasure.h"
#include "ToolComment.h"
#include "ToolColorPicker.h"
#include "ToolRectClone.h"
#include "BrushScatter.h"

void BaseBrush::renderOnCanvas(MainEditor* editor, int scale)
{
    renderOnCanvas(editor->canvas.currentDrawPoint, scale);
}

void BaseBrush::drawWholeSelectedPoint(XY canvasDrawPoint, XY onCanvasPoint, int scale)
{
    SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
    drawLocalPoint(canvasDrawPoint, onCanvasPoint, scale);
    if (scale > 2) {
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
        drawPointOutline(canvasDrawPoint, onCanvasPoint, scale);
    }
}

void BaseBrush::drawActiveColorPoint(MainEditor* e, XY onCanvasPoint)
{
    SDL_Rect r = e->canvas.canvasRectToScreenRect({ onCanvasPoint.x,onCanvasPoint.y,1,1 });
    SDL_Color c = uint32ToSDLColor(e->getActiveColor());
    SDL_SetRenderDrawColor(g_rd, c.r, c.g, c.b, 0xff);
    SDL_RenderFillRect(g_rd, &r);
}

void BaseBrush::drawSelectedPoint(MainEditor* e, XY onCanvasPoint)
{
    if (g_config.brushColorPreview) {
        drawActiveColorPoint(e, onCanvasPoint);
    }
    else {
        drawWholeSelectedPoint(e->canvas.currentDrawPoint, onCanvasPoint, e->canvas.scale);
    }
}

SDL_Rect BaseBrush::calcPixelRect(XY from, XY to, XY canvasDrawPoint, int scale)
{
    XY pointFrom = XY{ ixmin(from.x, to.x), ixmin(from.y, to.y) };
    XY pointTo = XY{ ixmax(from.x, to.x), ixmax(from.y, to.y) };
    SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x20);
    SDL_Rect r = { canvasDrawPoint.x + (pointFrom.x * scale), canvasDrawPoint.y + (pointFrom.y * scale), ((pointTo.x - pointFrom.x + 1) * scale), ((pointTo.y - pointFrom.y + 1) * scale) };
    return r;
}

void BaseBrush::drawPixelRect(XY from, XY to, XY canvasDrawPoint, int scale)
{
    SDL_Rect r = calcPixelRect(from, to, canvasDrawPoint, scale);
    SDL_RenderFillRect(g_rd, &r);
    SDL_SetRenderDrawColor(g_rd, 0x00, 0x00, 0x00, 0x80);
    SDL_Rect r2 = offsetRect(r, -1);
    SDL_RenderDrawRect(g_rd, &r2);
    SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
    SDL_RenderDrawRect(g_rd, &r);

    SDL_SetRenderDrawColor(g_rd, 0x00, 0x00, 0x00, 0x80);
    SDL_RenderDrawLine(g_rd, r.x, r.y + 1, r.x + r.w - 1, r.y + r.h);
    //SDL_RenderDrawLine(g_rd, r.x + 1, r.y, r.x + r.w, r.y + r.h - 1);
    SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
    SDL_RenderDrawLine(g_rd, r.x, r.y, r.x + r.w, r.y + r.h);
}

void g_loadBrushes()
{
    g_brushes.push_back(new Brush1x1());
    g_brushes.push_back(new Brush1x1PixelPerfect());
    g_brushes.push_back(new Brush1x1Burst());
    g_brushes.push_back(new Brush1x1ArcX());
    g_brushes.push_back(new Brush1x1ArcY());
    g_brushes.push_back(new Brush1pxLine());
    g_brushes.push_back(new Brush1pxLinePathfind());
    g_brushes.push_back(new BrushScatter());
    g_brushes.push_back(new BrushCurve());
    g_brushes.push_back(new BrushBezierLine());
    g_brushes.push_back(new BrushRect());
    g_brushes.push_back(new BrushRectFill());
    g_brushes.push_back(new Brush9SegmentRect());
    g_brushes.push_back(new BrushCircle());
    g_brushes.push_back(new BrushCircleArc());
    g_brushes.push_back(new BrushDiamond());
    g_brushes.push_back(new BrushFill());
    g_brushes.push_back(new BrushRaycastFill());
    g_brushes.push_back(new BrushReplaceColor());
    g_brushes.push_back(new ToolColorPicker());
    g_brushes.push_back(new ToolRectIsolate());
    g_brushes.push_back(new ToolRectClone());
    g_brushes.push_back(new ToolRectMove());
    g_brushes.push_back(new ToolRectSwap());
    g_brushes.push_back(new ToolRectFlip());
    g_brushes.push_back(new ToolRectRotate());
    g_brushes.push_back(new ToolComment());
    g_brushes.push_back(new ToolGuideline());
    g_brushes.push_back(new ToolSetXSymmetry());
    g_brushes.push_back(new ToolSetYSymmetry());
    g_brushes.push_back(new ToolMeasure());
    g_brushes.push_back(new ToolText());

    for (auto extbrush : g_pluginBrushes) {
        g_brushes.push_back(extbrush);
    }
}

void LineSelectBrush::renderLineOnCanvas(MainEditor* editor, XY from, XY to, int pointSize, bool roundPoint)
{
    rasterizeLine(from, to, [&](XY a) {
        if (xyEqual(a, startPos) || xyEqual(a, to)) {
            rasterizePoint(a, pointSize, [&](XY p) {
                drawSelectedPoint(editor, p);
                }, roundPoint);
        }
        else {
            drawSelectedPoint(editor, a);
        }
    });
}

void LineSelectBrush::renderSnappedLineOnCanvas(MainEditor* editor, PointSnapResult snap, int pointSize, bool roundPoint)
{
    XY endPoint = snap.to;

    if (snap.aspect != 0) {
        snap.to = lastMouseMotionPos;
    }

    rasterizeSnappedLine(snap, [&](XY a) {
        if (xyEqual(a, startPos) || xyEqual(a, endPoint)) {
            rasterizePoint(a, pointSize, [&](XY p) {
                drawSelectedPoint(editor, p);
            }, roundPoint);
        }
        else {
            drawSelectedPoint(editor, a);
        }
    });
}

void LineSelectBrush::clickRelease(MainEditor* editor, XY pos)
{ 
    if (dragging) { 
        pos = (!snapManuallyOnRelease() && g_shiftModifier) ? getSnappedPoint(startPos, pos) : pos;
        lineSelected(editor, startPos, pos); 
    } 
    dragging = false; 
}

void LineSelectBrush::renderOnCanvas(MainEditor* editor, int scale)
{
    defaultRenderOnCanvas(editor, 1, false);
}

void LineSelectBrush::defaultRenderOnCanvas(MainEditor* editor, int pointSize, bool roundPoint)
{
    if (dragging) {
        if (g_shiftModifier) {
            auto snap = getSnappedPointV2(startPos, lastMouseMotionPos);

            g_ttp->addTooltip(Tooltip{ editor->canvas.canvasPointToScreenPoint(startPos), frmt("{}:{}", snap.divOne ? snap.aspect : 1, snap.divOne ? 1 : snap.aspect) });

            renderSnappedLineOnCanvas(editor, snap, pointSize, roundPoint);
        }
        else {
            renderLineOnCanvas(editor, startPos, lastMouseMotionPos, pointSize, roundPoint);
        }
    }
    else {
        rasterizePoint(lastMouseMotionPos, pointSize, [&](XY p) {drawSelectedPoint(editor, p); }, roundPoint);
    }
}
