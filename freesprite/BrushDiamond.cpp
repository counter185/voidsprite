#include "BrushDiamond.h"
#include "maineditor.h"

void BrushDiamond::clickPress(MainEditor* editor, XY pos)
{
	startPos = pos;
	heldDown = true;
}

void BrushDiamond::clickRelease(MainEditor* editor, XY pos)
{
	XY posMin = { ixmin(pos.x, startPos.x), ixmin(pos.y, startPos.y) };
	XY posMax = { ixmax(pos.x, startPos.x), ixmax(pos.y, startPos.y) };
	rasterizeDiamond(posMin, posMax, [&](XY a) {
		editor->SetPixel(a, editor->getActiveColor());
		});
	heldDown = false;
}

void BrushDiamond::renderOnCanvas(XY canvasDrawPoint, int scale)
{
	if (heldDown) {
		SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
		drawLocalPoint(canvasDrawPoint, startPos, scale);
		SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
		drawPointOutline(canvasDrawPoint, startPos, scale);

		XY posMin = { ixmin(lastMouseMotionPos.x, startPos.x), ixmin(lastMouseMotionPos.y, startPos.y) };
		XY posMax = { ixmax(lastMouseMotionPos.x, startPos.x), ixmax(lastMouseMotionPos.y, startPos.y) };
		rasterizeDiamond(posMin, posMax, [&](XY a) {
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
