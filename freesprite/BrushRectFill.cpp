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
			editor->SetPixel(XY{ x,y }, 0xFF000000 | editor->pickedColor);
		}
	}
}

void BrushRectFill::renderOnCanvas(XY canvasDrawPoint, int scale)
{
	if (heldDown) {
		XY pointFrom = XY{ ixmin(startPos.x, lastMouseMotionPos.x), ixmin(startPos.y, lastMouseMotionPos.y) };
		XY pointTo = XY{ ixmax(startPos.x, lastMouseMotionPos.x), ixmax(startPos.y, lastMouseMotionPos.y) };
		//pointTo = xyAdd(pointTo, XY{ 1,1 });
		SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
		SDL_Rect r = { canvasDrawPoint.x + (pointFrom.x * scale), canvasDrawPoint.y + (pointFrom.y * scale), ((pointTo.x - pointFrom.x + 1) * scale), ((pointTo.y - pointFrom.y + 1) * scale) };
		SDL_RenderFillRect(g_rd, &r);
	}
}
