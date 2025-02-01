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
	rasterizeEllipse(posMin, posMax, [&](XY a) {
		editor->SetPixel(a, editor->getActiveColor());
	});
	heldDown = false;
}

void BrushCircle::renderOnCanvas(XY canvasDrawPoint, int scale)
{
	if (heldDown) {
		XY posMin = { ixmin(lastMouseMotionPos.x, startPos.x), ixmin(lastMouseMotionPos.y, startPos.y) };
		XY posMax = { ixmax(lastMouseMotionPos.x, startPos.x), ixmax(lastMouseMotionPos.y, startPos.y) };
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

void BrushCircleArc::clickPress(MainEditor* editor, XY pos)
{
	startPos = pos;
	heldDown = true;
	rightClicked = false;
}

void BrushCircleArc::clickRelease(MainEditor* editor, XY pos)
{
	if (heldDown && !rightClicked) {
		XY posMin = { ixmin(pos.x, startPos.x), ixmin(pos.y, startPos.y) };
		XY posMax = { ixmax(pos.x, startPos.x), ixmax(pos.y, startPos.y) };
		rasterizeSplitEllipse(posMin, posMax, [&](XY a) {
			editor->SetPixel(a, editor->getActiveColor());
			});
		heldDown = false;
	}
}

void BrushCircleArc::renderOnCanvas(XY canvasDrawPoint, int scale)
{
	printf("held down: %i right clicked: %i\n", heldDown, rightClicked);
	if (heldDown) {
		XY posMin = { ixmin(lastMouseMotionPos.x, startPos.x), ixmin(lastMouseMotionPos.y, startPos.y) };
		XY posMax = { ixmax(lastMouseMotionPos.x, startPos.x), ixmax(lastMouseMotionPos.y, startPos.y) };
		(rightClicked ? &rasterizeSplitEllipseByY : &rasterizeSplitEllipse)(posMin, posMax, [&](XY a) {
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

void BrushCircleArc::rightClickPress(MainEditor* editor, XY pos)
{
	startPos = pos;
	heldDown = true;
	rightClicked = true;
}

void BrushCircleArc::rightClickRelease(MainEditor* editor, XY pos)
{
	if (heldDown && rightClicked) {
		editor->commitStateToCurrentLayer();
		XY posMin = { ixmin(pos.x, startPos.x), ixmin(pos.y, startPos.y) };
		XY posMax = { ixmax(pos.x, startPos.x), ixmax(pos.y, startPos.y) };
		rasterizeSplitEllipseByY(posMin, posMax, [&](XY a) {
			editor->SetPixel(a, editor->getActiveColor());
			});
		heldDown = false;
	}
}
