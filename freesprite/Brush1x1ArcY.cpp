#include "Brush1x1ArcY.h"
#include "maineditor.h"

void Brush1x1ArcY::clickPress(MainEditor* editor, XY pos) {
	editor->SetPixel(pos, editor->getActiveColor());
}

void Brush1x1ArcY::clickDrag(MainEditor* editor, XY from, XY to) {
	rasterizeLine(from, to, [&](XY a)->void {
		editor->SetPixel(a, editor->getActiveColor());
		}, 2);
}