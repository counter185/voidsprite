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

void Brush1pxLine::renderOnCanvas(XY canvasDrawPoint, int scale) 
{

	if (dragging) {
		rasterizeLine(startPos, g_shiftModifier ? getSnappedPoint(startPos, lastMouseMotionPos) : lastMouseMotionPos, [&](XY a) {
			SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
			drawLocalPoint(canvasDrawPoint, a, scale);
			SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
			drawPointOutline(canvasDrawPoint, a, scale);
			});
	}
	else {
		SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
		drawLocalPoint(canvasDrawPoint, lastMouseMotionPos, scale);
		SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
		drawPointOutline(canvasDrawPoint, lastMouseMotionPos, scale);
	}
}
