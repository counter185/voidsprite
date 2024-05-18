#include "Brush3pxCircle.h"
#include "maineditor.h"

void Brush3pxCircle::clickPress(MainEditor* editor, XY pos)
{
	editor->SetPixel(xyAdd(pos, XY{-1,0}), 0xFF000000 | editor->pickedColor);
	editor->SetPixel(xyAdd(pos, XY{1,0}), 0xFF000000 | editor->pickedColor);
	editor->SetPixel(xyAdd(pos, XY{0,-1}), 0xFF000000 | editor->pickedColor);
	editor->SetPixel(xyAdd(pos, XY{0,1}), 0xFF000000 | editor->pickedColor);
	editor->SetPixel(pos, 0xFF000000 | editor->pickedColor);
}

void Brush3pxCircle::clickDrag(MainEditor* editor, XY from, XY to)
{
	rasterizeLine(from, to, [editor](XY pos) {
		editor->SetPixel(xyAdd(pos, XY{ -1,0 }), 0xFF000000 | editor->pickedColor);
		editor->SetPixel(xyAdd(pos, XY{ 1,0 }), 0xFF000000 | editor->pickedColor);
		editor->SetPixel(xyAdd(pos, XY{ 0,-1 }), 0xFF000000 | editor->pickedColor);
		editor->SetPixel(xyAdd(pos, XY{ 0,1 }), 0xFF000000 | editor->pickedColor);
		editor->SetPixel(pos, 0xFF000000 | editor->pickedColor);
	});
	//editor->DrawLine(from, to, 0xFF000000 | editor->pickedColor);
}
