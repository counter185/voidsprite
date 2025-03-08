#include "ToolRectIsolate.h"
#include "maineditor.h"

void ToolRectIsolate::clickPress(MainEditor* editor, XY pos)
{
	heldDown = true;
	startPos = pos;
	lastMousePos = pos;
}

void ToolRectIsolate::clickRelease(MainEditor* editor, XY pos) { 
	if (heldDown) {
        heldDown = false;
        if (xyEqual(startPos, lastMousePos) && clickTimer.started && clickTimer.elapsedTime() < 600) {
            if (!g_ctrlModifier) {
                editor->isolatedFragment.clear();
            }
            editor->isolatedFragment.addOtherMap(editor->getCurrentLayer()->wandSelectAt(pos));
		} else {
            XY p1 = startPos;
            XY p2 = lastMousePos;
            XY minpoint = XY{ixmin(p1.x, p2.x), ixmin(p1.y, p2.y)};
            XY maxpoint = XY{ixmax(p1.x, p2.x), ixmax(p1.y, p2.y)};
            if (!g_ctrlModifier) {
                editor->isolatedFragment.clear();
            }
            editor->isolatedFragment.addRect(
                {minpoint.x, minpoint.y, maxpoint.x - minpoint.x + 1, maxpoint.y - minpoint.y + 1});
        }
        editor->isolateEnabled = true;
        clickTimer.start();
	}
}

void ToolRectIsolate::renderOnCanvas(XY canvasDrawPoint, int scale)
{
	if (heldDown) {
		drawPixelRect(startPos, lastMousePos, canvasDrawPoint, scale);
	}
}

void ToolRectIsolate::rightClickPress(MainEditor* editor, XY pos)
{
	editor->isolateEnabled = false;
}
