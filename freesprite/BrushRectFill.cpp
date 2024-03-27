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
		SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
		SDL_Rect r = { canvasDrawPoint.x + (startPos.x * scale), canvasDrawPoint.y + (startPos.y * scale), ((lastMousePos.x - startPos.x + 1) * scale), ((lastMousePos.y - startPos.y + 1) * scale) };
		SDL_RenderFillRect(g_rd, &r);
	}
}
