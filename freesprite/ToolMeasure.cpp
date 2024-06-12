#include "ToolMeasure.h"
#include "mathops.h"
#include "FontRenderer.h"

void ToolMeasure::clickPress(MainEditor* editor, XY pos)
{
	heldDown = true;
	lastMousePos = pos;
	startPos = pos;
}

void ToolMeasure::renderOnCanvas(XY canvasDrawPoint, int scale)
{
	if (heldDown) {
		XY pointFrom = XY{ ixmin(startPos.x, lastMouseMotionPos.x), ixmin(startPos.y, lastMouseMotionPos.y) };
		XY pointTo = XY{ ixmax(startPos.x, lastMouseMotionPos.x), ixmax(startPos.y, lastMouseMotionPos.y) };
		SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x20);
		SDL_Rect r = { canvasDrawPoint.x + (pointFrom.x * scale), canvasDrawPoint.y + (pointFrom.y * scale), ((pointTo.x - pointFrom.x + 1) * scale), ((pointTo.y - pointFrom.y + 1) * scale) };
		SDL_RenderFillRect(g_rd, &r);
		SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
		SDL_RenderDrawRect(g_rd, &r);

		SDL_RenderDrawLine(g_rd, r.x, r.y, r.x + r.w, r.y + r.h);

		g_fnt->RenderString(std::format("{}px x {}px", pointTo.x - pointFrom.x + 1, pointTo.y - pointFrom.y + 1), canvasDrawPoint.x + lastMousePos.x * scale + 5, canvasDrawPoint.y + lastMousePos.y * scale + 22);
	}

	SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
	drawLocalPoint(canvasDrawPoint, lastMouseMotionPos, scale);
	SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
	drawPointOutline(canvasDrawPoint, lastMouseMotionPos, scale);
	
}
