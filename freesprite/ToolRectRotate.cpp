#include "globals.h"
#include "ToolRectRotate.h"
#include "mathops.h"
#include "Notification.h"
#include "maineditor.h"
#include "FontRenderer.h"

void ToolRectRotate::clickPress(MainEditor* editor, XY pos)
{
	dragging = true;
	dragRightClick = false;
	dragStart = pos;
}

void ToolRectRotate::clickRelease(MainEditor* editor, XY pos)
{
	if (dragging && !dragRightClick) {
		doRotate(editor, dragStart, pos, false);
		dragging = false;
	}
}

void ToolRectRotate::rightClickPress(MainEditor* editor, XY pos)
{
	dragging = true;
	dragRightClick = true;
	dragStart = pos;
}

void ToolRectRotate::rightClickRelease(MainEditor* editor, XY pos)
{
	if (dragging && dragRightClick) {
		doRotate(editor, dragStart, pos, true);
		dragging = false;
	}
}

void ToolRectRotate::renderOnCanvas(XY canvasDrawPoint, int scale)
{
	if (dragging) {
		XY point = lastMouseMotionPos;
		int xdist = lastMouseMotionPos.x - dragStart.x;
		point.x = dragStart.x + xdist;
		point.y = dragStart.y + xdist;
		XY pointFrom = XY{ ixmin(dragStart.x, point.x), ixmin(dragStart.y, point.y) };
		drawPixelRect(dragStart, point, canvasDrawPoint, scale);
		g_fnt->RenderString(dragRightClick ? "Rotate -90d" : "Rotate 90d", canvasDrawPoint.x + pointFrom.x * scale, canvasDrawPoint.y + pointFrom.y * scale);
	}

	SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
	drawLocalPoint(canvasDrawPoint, lastMouseMotionPos, scale);
}

void ToolRectRotate::doRotate(MainEditor* editor, XY startPos, XY endPos, bool ccl)
{
	XY point;
	int xdist = endPos.x - dragStart.x;
	point.x = dragStart.x + xdist;
	point.y = dragStart.y + xdist;
	XY minPoint = XY{ ixmin(startPos.x, point.x), ixmin(startPos.y, point.y) };
	XY maxPoint = XY{ ixmax(startPos.x, point.x), ixmax(startPos.y, point.y) };

	if ((maxPoint.x == minPoint.x) || (maxPoint.y == minPoint.y)) {
		g_addNotification(ErrorNotification("Rotate rect", "Invalid rect size"));
		return;
	}

	int regionW = maxPoint.x - minPoint.x + 1;
	int regionH = maxPoint.y - minPoint.y + 1;
	uint32_t* copy = (uint32_t*)tracked_malloc(4 * regionW * regionH, "Temp. mem.");
	if (copy == NULL) {
		g_addNotification(ErrorNotification("Error", "malloc failed"));
		return;
	}

	editor->commitStateToCurrentLayer();

	Layer* layer = editor->getCurrentLayer();
	for (int y = 0; y < regionH; y++) {
		for (int x = 0; x < regionW; x++) {
			copy[x + y * regionW] = layer->getPixelAt(XY{ x + minPoint.x, y + minPoint.y });
		}
	}

	for (int y = 0; y < regionH; y++) {
		for (int x = 0; x < regionW; x++) {
			if (ccl) {
				layer->setPixel(XY{ x + minPoint.x, y + minPoint.y }, copy[(regionW - y - 1) + x * regionW]);
			}
			else {
				layer->setPixel(XY{ x + minPoint.x, y + minPoint.y }, copy[y + (regionW - x - 1) * regionH]);
			}
		}
	}

	tracked_free(copy);
}
