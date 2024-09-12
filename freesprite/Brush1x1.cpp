#include "Brush1x1.h"
#include "maineditor.h"
#include "MainEditorPalettized.h"

void Brush1x1::clickPress(MainEditor* editor, XY pos) {
	editor->SetPixel(pos, editor->getActiveColor());
}

void Brush1x1::clickDrag(MainEditor* editor, XY from, XY to) {
	editor->DrawLine(from, to, editor->getActiveColor());
}
