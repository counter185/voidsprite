#include "Brush1pxLinePathfind.h"
#include "../UtilPathfind.h"
#include "../background_operation.h"

void Brush1pxLinePathfind::clickPress(MainEditor* editor, XY pos)
{
	startPos = pos;
	dragging = true;
}

void Brush1pxLinePathfind::clickRelease(MainEditor* editor, XY pos)
{
	g_startNewOperation([this, editor, pos](OperationProgressReport* progressReport) {
		progressReport->enterSection("Pathfinding...");
		std::vector<Node> pathfindResult = genAStar(editor->getCurrentLayer(), startPos, pos, progressReport);
		for (Node& n : pathfindResult) {
			editor->SetPixel(XY{ n.x, n.y }, editor->getActiveColor());
		}
		//editor->DrawLine(startPos, pos, 0xFF000000 | editor->pickedColor);
		dragging = false;
	});
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
