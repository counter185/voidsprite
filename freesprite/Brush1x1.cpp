#include "Brush1x1.h"
#include "maineditor.h"

void Brush1x1::clickPress(MainEditor* editor, XY pos) {
	editor->SetPixel(pos, 0xFF000000 | editor->pickedColor);
}

void Brush1x1::clickDrag(MainEditor* editor, XY from, XY to) {
	editor->DrawLine(from, to, 0xFF000000 | editor->pickedColor);
}
