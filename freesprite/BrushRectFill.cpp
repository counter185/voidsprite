#include "BrushRectFill.h"
#include "mathops.h"
#include "maineditor.h"

void BrushRectFill::clickPress(MainEditor* editor, XY pos)
{
	heldDown = true;
	lastMousePos = pos;
	startPos = pos;
}

void BrushRectFill::clickRelease(MainEditor* editor, XY pos)
{
	heldDown = false;
	int minx = ixmin(pos.x, startPos.x);
	int maxx = ixmax(pos.x, startPos.x);
	int miny = ixmin(pos.y, startPos.y);
	int maxy = ixmax(pos.y, startPos.y);

	for (int x = minx; x <= maxx; x++) {
		for (int y = miny; y <= maxy; y++) {
			editor->SetPixel(XY{ x,y }, editor->getActiveColor());
		}
	}
}

void BrushRectFill::renderOnCanvas(XY canvasDrawPoint, int scale)
{
	if (heldDown) {
		drawPixelRect(startPos, lastMouseMotionPos, canvasDrawPoint, scale);
	}

	SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
	drawLocalPoint(canvasDrawPoint, lastMouseMotionPos, scale);
	SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
	drawPointOutline(canvasDrawPoint, lastMouseMotionPos, scale);
}
