#include "Brush1x1ArcX.h"

void Brush1x1ArcX::clickPress(MainEditor* editor, XY pos) {
	editor->SetPixel(pos, editor->getActiveColor());
}

void Brush1x1ArcX::clickDrag(MainEditor* editor, XY from, XY to) {
	//editor->DrawLine(from, to, 0xFF000000 | editor->pickedColor);
	rasterizeLine(from, to, [&](XY a)->void {
		editor->SetPixel(a, editor->getActiveColor());
	}, 1);
}
