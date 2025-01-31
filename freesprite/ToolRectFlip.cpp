#include "globals.h"
#include "mathops.h"
#include "ToolRectFlip.h"
#include "Notification.h"
#include "maineditor.h"
#include "FontRenderer.h"

void ToolRectFlip::clickPress(MainEditor* editor, XY pos)
{
	dragging = true;
	dragRightClick = false;
	dragStart = pos;
}

void ToolRectFlip::clickRelease(MainEditor* editor, XY pos)
{
	if (dragging && !dragRightClick) {
		doFlip(editor, dragStart, pos, false);
		dragging = false;
	}
}

void ToolRectFlip::rightClickPress(MainEditor* editor, XY pos)
{
	dragging = true;
	dragRightClick = true;
	dragStart = pos;
}

void ToolRectFlip::rightClickRelease(MainEditor* editor, XY pos)
{
	if (dragging && dragRightClick) {
		doFlip(editor, dragStart, pos, true);
		dragging = false;
	}
}

void ToolRectFlip::renderOnCanvas(XY canvasDrawPoint, int scale) {
	if (dragging) {
		drawPixelRect(dragStart, lastMouseMotionPos, canvasDrawPoint, scale);
		XY pointFrom = XY{ ixmin(dragStart.x, lastMouseMotionPos.x), ixmin(dragStart.y, lastMouseMotionPos.y) };
		g_fnt->RenderString(dragRightClick ? "Flip Y" : "Flip X", canvasDrawPoint.x + pointFrom.x * scale, canvasDrawPoint.y + pointFrom.y * scale );
	}

	SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
	drawLocalPoint(canvasDrawPoint, lastMouseMotionPos, scale);
}

void ToolRectFlip::doFlip(MainEditor* editor, XY startPos, XY endPos, bool yFlip)
{
	XY minPoint = XY{ ixmin(startPos.x, endPos.x), ixmin(startPos.y, endPos.y) };
	XY maxPoint = XY{ ixmax(startPos.x, endPos.x), ixmax(startPos.y, endPos.y) };

	if ((!yFlip && maxPoint.x == minPoint.x) || (yFlip && maxPoint.y == minPoint.y)) {
		g_addNotification(ErrorNotification("Flip rect", "Nothing to flip!"));
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
			if (!yFlip) {
				layer->setPixel(XY{ x + minPoint.x, y + minPoint.y }, copy[(regionW - x - 1) + y * regionW]);
			}
			else {
				layer->setPixel(XY{ x + minPoint.x, y + minPoint.y }, copy[x + (regionH - y - 1)* regionW]);
			}
		}
	}

	tracked_free(copy);

}
