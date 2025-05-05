#include "Brush1pxLine.h"
#include "maineditor.h"
#include "mathops.h"
#include "TooltipsLayer.h"

void Brush1pxLine::clickPress(MainEditor* editor, XY pos)
{
	startPos = pos;
	dragging = true;
}

void Brush1pxLine::clickRelease(MainEditor* editor, XY pos)
{
	editor->DrawLine(startPos, g_shiftModifier ? getSnappedPoint(startPos, pos) : pos, editor->getActiveColor());
	dragging = false;
}

void Brush1pxLine::renderOnCanvas(MainEditor* editor, int scale)
{
	XY canvasDrawPoint = editor->canvas.currentDrawPoint;
	if (dragging) {
		rasterizeLine(startPos, g_shiftModifier ? getSnappedPoint(startPos, lastMouseMotionPos) : lastMouseMotionPos, [&](XY a) {
			if (g_config.brushColorPreview) {
				SDL_Rect r = editor->canvas.canvasRectToScreenRect({ a.x,a.y,1,1 });
				SDL_Color c = uint32ToSDLColor(editor->getActiveColor());
				SDL_SetRenderDrawColor(g_rd, c.r, c.g, c.b, 0xff);
				SDL_RenderFillRect(g_rd, &r);
			}
			else {
				SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
				drawLocalPoint(canvasDrawPoint, a, scale);
				SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
				drawPointOutline(canvasDrawPoint, a, scale);
			}
		});
	}
	else {
		if (g_config.brushColorPreview) {
			SDL_Rect r = editor->canvas.canvasRectToScreenRect({ lastMouseMotionPos.x,lastMouseMotionPos.y,1,1 });
			SDL_Color c = uint32ToSDLColor(editor->getActiveColor());
			SDL_SetRenderDrawColor(g_rd, c.r, c.g, c.b, 0xff);
			SDL_RenderFillRect(g_rd, &r);
		}
		else {
			SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
			drawLocalPoint(canvasDrawPoint, lastMouseMotionPos, scale);
			SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
			drawPointOutline(canvasDrawPoint, lastMouseMotionPos, scale);
		}
	}
}
