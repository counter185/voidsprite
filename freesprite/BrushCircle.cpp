#include "BrushCircle.h"
#include "mathops.h"
#include "maineditor.h"

void BrushCircle::clickPress(MainEditor* editor, XY pos)
{
	startPos = pos;
	heldDown = true;
}

void BrushCircle::clickRelease(MainEditor* editor, XY pos)
{
	XY posMin = { ixmin(pos.x, startPos.x), ixmin(pos.y, startPos.y)};
	XY posMax = { ixmax(pos.x, startPos.x), ixmax(pos.y, startPos.y) };
	/*editor->getCurrentLayer()->setPixel(posMin, editor->pickedColor);
	editor->getCurrentLayer()->setPixel(posMax, editor->pickedColor);
	editor->getCurrentLayer()->setPixel({posMin.x, posMax.y}, editor->pickedColor);
	editor->getCurrentLayer()->setPixel({posMax.x, posMin.y}, editor->pickedColor);*/
	rasterizeEllipse(posMin, posMax, [&](XY a) {
		editor->SetPixel(a, editor->getActiveColor());
	});
	heldDown = false;
}

void BrushCircle::renderOnCanvas(XY canvasDrawPoint, int scale)
{
	if (heldDown) {
		XY posMin = { ixmin(lastMousePos.x, startPos.x), ixmin(lastMousePos.y, startPos.y) };
		XY posMax = { ixmax(lastMousePos.x, startPos.x), ixmax(lastMousePos.y, startPos.y) };
		/*editor->getCurrentLayer()->setPixel(posMin, editor->pickedColor);
		editor->getCurrentLayer()->setPixel(posMax, editor->pickedColor);
		editor->getCurrentLayer()->setPixel({posMin.x, posMax.y}, editor->pickedColor);
		editor->getCurrentLayer()->setPixel({posMax.x, posMin.y}, editor->pickedColor);*/
		rasterizeEllipse(posMin, posMax, [&](XY a) {
			SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
			drawLocalPoint(canvasDrawPoint, a, scale);
			SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
			drawPointOutline(canvasDrawPoint, a, scale);
			});
	}
	SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
	drawLocalPoint(canvasDrawPoint, lastMouseMotionPos, scale);
	SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
	drawPointOutline(canvasDrawPoint, lastMouseMotionPos, scale);
}
