#include "BrushRect.h"
#include "maineditor.h"

void BrushRect::clickPress(MainEditor* editor, XY pos)
{
	startPos = pos;
}

void BrushRect::clickRelease(MainEditor* editor, XY pos)
{
	editor->DrawLine(startPos, XY{pos.x, startPos.y}, 0xFF000000 | editor->pickedColor);
	editor->DrawLine(startPos, XY{startPos.x, pos.y}, 0xFF000000 | editor->pickedColor);
	editor->DrawLine(pos, XY{pos.x, startPos.y}, 0xFF000000 | editor->pickedColor);
	editor->DrawLine(pos, XY{startPos.x, pos.y}, 0xFF000000 | editor->pickedColor);
}
