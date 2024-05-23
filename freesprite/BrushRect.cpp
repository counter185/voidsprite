#include "BrushRect.h"
#include "maineditor.h"

void BrushRect::clickPress(MainEditor* editor, XY pos)
{
	heldDown = true;
	lastMousePos = pos;
	startPos = pos;
}

void BrushRect::clickRelease(MainEditor* editor, XY pos)
{
	heldDown = false;
	editor->DrawLine(startPos, XY{pos.x, startPos.y}, 0xFF000000 | editor->pickedColor);
	editor->DrawLine(startPos, XY{startPos.x, pos.y}, 0xFF000000 | editor->pickedColor);
	editor->DrawLine(pos, XY{pos.x, startPos.y}, 0xFF000000 | editor->pickedColor);
	editor->DrawLine(pos, XY{startPos.x, pos.y}, 0xFF000000 | editor->pickedColor);
}

void BrushRect::renderOnCanvas(XY canvasDrawPoint, int scale)
{
	if (heldDown) {
		XY pointFrom = XY{ ixmin(startPos.x, lastMouseMotionPos.x), ixmin(startPos.y, lastMouseMotionPos.y) };
		XY pointTo = XY{ ixmax(startPos.x, lastMouseMotionPos.x), ixmax(startPos.y, lastMouseMotionPos.y) };
		SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
		SDL_Rect r = { canvasDrawPoint.x + (pointFrom.x * scale), canvasDrawPoint.y + (pointFrom.y * scale), ((pointTo.x - pointFrom.x + 1) * scale), ((pointTo.y - pointFrom.y + 1) * scale) };
		SDL_RenderFillRect(g_rd, &r);
	}

	SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
	drawLocalPoint(canvasDrawPoint, lastMouseMotionPos, scale);
	SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
	drawPointOutline(canvasDrawPoint, lastMouseMotionPos, scale);
}
