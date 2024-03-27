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
		SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
		SDL_Rect r = { canvasDrawPoint.x + (startPos.x * scale), canvasDrawPoint.y + (startPos.y*scale), ((lastMousePos.x - startPos.x + 1) * scale), ((lastMousePos.y-startPos.y + 1) * scale)};
		SDL_RenderFillRect(g_rd, &r);
	}
}
