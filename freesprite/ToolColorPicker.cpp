#include "ToolColorPicker.h"
#include "maineditor.h"

void ToolColorPicker::clickPress(MainEditor* editor, XY pos)
{
    editor->lastColorPickWasFromWholeImage = false;
    editor->setActiveColor(editor->layer_getPixelAt(pos));
}

void ToolColorPicker::clickDrag(MainEditor* editor, XY from, XY to)
{
    (void) from;

    editor->setActiveColor(editor->layer_getPixelAt(to), false);
}
