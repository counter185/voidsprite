#include "Brush1pxLinePathfind.h"
#include "maineditor.h"
#include "UtilPathfind.h"

void Brush1pxLinePathfind::clickPress(MainEditor* editor, XY pos)
{
	startPos = pos;
	dragging = true;
}

void Brush1pxLinePathfind::clickRelease(MainEditor* editor, XY pos)
{
	std::vector<Node> pathfindResult = genAStar(editor->getCurrentLayer(), startPos, pos);
	for (Node& n : pathfindResult) {
		editor->SetPixel(XY{ n.x, n.y }, editor->pickedColor);
	}
	//editor->DrawLine(startPos, pos, 0xFF000000 | editor->pickedColor);
	dragging = false;
}

void Brush1pxLinePathfind::renderOnCanvas(XY canvasDrawPoint, int scale)
{

	if (dragging) {
		rasterizeLine(startPos, lastMouseMotionPos, [&](XY a) {
			SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
			drawLocalPoint(canvasDrawPoint, a, scale);
			SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
			drawPointOutline(canvasDrawPoint, a, scale);
			});
	}
	else {
		SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
		drawLocalPoint(canvasDrawPoint, lastMouseMotionPos, scale);
		SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
		drawPointOutline(canvasDrawPoint, lastMouseMotionPos, scale);
	}
}
