#include "ToolColorPicker.h"
#include "maineditor.h"

void ToolColorPicker::clickPress(MainEditor* editor, XY pos)
{
	editor->setActiveColor(editor->layer_getPixelAt(pos));
	editor->playColorPickerVFX(false);
}

void ToolColorPicker::clickDrag(MainEditor* editor, XY from, XY to)
{
	editor->setActiveColor(editor->layer_getPixelAt(to));
}
