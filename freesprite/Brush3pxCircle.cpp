#include "Brush3pxCircle.h"
#include "maineditor.h"

void Brush3pxCircle::clickPress(MainEditor* editor, XY pos)
{
	editor->SetPixel(xyAdd(pos, XY{-1,0}), editor->getActiveColor());
	editor->SetPixel(xyAdd(pos, XY{1,0}), editor->getActiveColor());
	editor->SetPixel(xyAdd(pos, XY{0,-1}), editor->getActiveColor());
	editor->SetPixel(xyAdd(pos, XY{0,1}), editor->getActiveColor());
	editor->SetPixel(pos, editor->getActiveColor());
}

void Brush3pxCircle::clickDrag(MainEditor* editor, XY from, XY to)
{
	rasterizeLine(from, to, [editor](XY pos) {
		editor->SetPixel(xyAdd(pos, XY{ -1,0 }), editor->getActiveColor());
		editor->SetPixel(xyAdd(pos, XY{ 1,0 }), editor->getActiveColor());
		editor->SetPixel(xyAdd(pos, XY{ 0,-1 }), editor->getActiveColor());
		editor->SetPixel(xyAdd(pos, XY{ 0,1 }), editor->getActiveColor());
		editor->SetPixel(pos, editor->getActiveColor());
	});
	//editor->DrawLine(from, to, 0xFF000000 | editor->pickedColor);
}

void Brush3pxCircle::renderOnCanvas(XY canvasDrawPoint, int scale) {
	for (int y = -1; y <= 1; y++) {
		for (int x = -1; x <= 1; x++) {
			if (x == 0 || y == 0) {
				SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
				drawLocalPoint(canvasDrawPoint, xyAdd(lastMouseMotionPos, { x,y }), scale);
				SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
				drawPointOutline(canvasDrawPoint, xyAdd(lastMouseMotionPos, { x,y }), scale);
			}
		}
	}
}
