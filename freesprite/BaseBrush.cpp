#include "BaseBrush.h"
#include "maineditor.h"

void BaseBrush::renderOnCanvas(MainEditor* editor, int scale)
{
	renderOnCanvas(editor->canvasCenterPoint, scale);
}
