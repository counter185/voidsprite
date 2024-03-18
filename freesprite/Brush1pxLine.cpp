#include "Brush1pxLine.h"
#include "maineditor.h"

void Brush1pxLine::clickPress(MainEditor* editor, XY pos)
{
	startPos = pos;
}

void Brush1pxLine::clickRelease(MainEditor* editor, XY pos)
{
	editor->DrawLine(startPos, pos, 0xFF000000 | editor->pickedColor);
}
