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
	editor->DrawLine(startPos, XY{pos.x, startPos.y}, editor->getActiveColor());
	editor->DrawLine(startPos, XY{startPos.x, pos.y}, editor->getActiveColor());
	editor->DrawLine(pos, XY{pos.x, startPos.y}, editor->getActiveColor());
	editor->DrawLine(pos, XY{startPos.x, pos.y}, editor->getActiveColor());
}

void BrushRect::renderOnCanvas(XY canvasDrawPoint, int scale)
{
	if (heldDown) {
		drawPixelRect(startPos, lastMouseMotionPos, canvasDrawPoint, scale);
	}

	SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
	drawLocalPoint(canvasDrawPoint, lastMouseMotionPos, scale);
	SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
	drawPointOutline(canvasDrawPoint, lastMouseMotionPos, scale);
}
