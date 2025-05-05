#include "BaseBrush.h"
#include "maineditor.h"

#include "Brush1x1.h"
#include "Brush1x1ArcX.h"
#include "Brush1x1ArcY.h"
#include "Brush3pxCircle.h"
#include "Brush1pxLine.h"
#include "BrushRect.h"
#include "BrushRectFill.h"
#include "BrushFill.h"
#include "Brush1pxLinePathfind.h"
#include "BrushCircle.h"
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

void BaseBrush::renderOnCanvas(MainEditor* editor, int scale)
{
    renderOnCanvas(editor->canvas.currentDrawPoint, scale);
}

void BaseBrush::drawWholeSelectedPoint(XY canvasDrawPoint, XY onCanvasPoint, int scale)
{
    SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
    drawLocalPoint(canvasDrawPoint, onCanvasPoint, scale);
    SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
    drawPointOutline(canvasDrawPoint, onCanvasPoint, scale);
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

void BaseBrush::drawPixelRect(XY from, XY to, XY canvasDrawPoint, int scale)
{
    XY pointFrom = XY{ ixmin(from.x, to.x), ixmin(from.y, to.y) };
    XY pointTo = XY{ ixmax(from.x, to.x), ixmax(from.y, to.y) };
    SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x20);
    SDL_Rect r = { canvasDrawPoint.x + (pointFrom.x * scale), canvasDrawPoint.y + (pointFrom.y * scale), ((pointTo.x - pointFrom.x + 1) * scale), ((pointTo.y - pointFrom.y + 1) * scale) };
    SDL_RenderFillRect(g_rd, &r);
    SDL_SetRenderDrawColor(g_rd, 0x00, 0x00, 0x00, 0x80);
    SDL_Rect r2 = r;
    r2.x++;
    r2.y++;
    r2.w -= 2;
    r2.h -= 2;
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
    g_brushes.push_back(new Brush3pxCircle());
    g_brushes.push_back(new Brush1pxLine());
    g_brushes.push_back(new Brush1pxLinePathfind());
    g_brushes.push_back(new BrushBezierLine());
    g_brushes.push_back(new BrushRect());
    g_brushes.push_back(new BrushRectFill());
    g_brushes.push_back(new Brush9SegmentRect());
    g_brushes.push_back(new BrushCircle());
    g_brushes.push_back(new BrushCircleArc());
    g_brushes.push_back(new BrushDiamond());
    g_brushes.push_back(new BrushFill());
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
}
