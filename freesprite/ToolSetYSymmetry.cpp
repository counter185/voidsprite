#include "ToolSetYSymmetry.h"
#include "maineditor.h"

void ToolSetYSymmetry::clickPress(MainEditor* editor, XY pos)
{
	editor->symmetryEnabled[1] = true;
	editor->symmetryPositions.y = pos.y;
}

void ToolSetYSymmetry::clickDrag(MainEditor* editor, XY from, XY to)
{
	editor->symmetryEnabled[1] = true;
	editor->symmetryPositions.y = to.y;
}

void ToolSetYSymmetry::rightClickPress(MainEditor* editor, XY pos)
{
	editor->symmetryEnabled[1] = !editor->symmetryEnabled[1];
}
