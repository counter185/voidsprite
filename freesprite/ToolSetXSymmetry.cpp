#include "ToolSetXSymmetry.h"
#include "maineditor.h"

void ToolSetXSymmetry::clickPress(MainEditor* editor, XY pos)
{
	editor->symmetryEnabled[0] = true;
	editor->symmetryPositions.x = pos.x;
}

void ToolSetXSymmetry::clickDrag(MainEditor* editor, XY from, XY to)
{
	editor->symmetryEnabled[0] = true;
	editor->symmetryPositions.x = to.x;
}

void ToolSetXSymmetry::rightClickPress(MainEditor* editor, XY pos)
{
	editor->symmetryEnabled[0] = !editor->symmetryEnabled[0];
}
