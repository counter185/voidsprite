#include "ToolColorPicker.h"
#include "maineditor.h"

void ToolColorPicker::clickPress(MainEditor* editor, XY pos)
{
	editor->colorPicker->setMainEditorColorRGB(editor->imgLayer->getPixelAt(pos));
}

void ToolColorPicker::clickDrag(MainEditor* editor, XY from, XY to)
{
	editor->colorPicker->setMainEditorColorRGB(editor->imgLayer->getPixelAt(to));
}
