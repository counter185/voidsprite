#pragma once
#include "BaseBrush.h"
class ToolColorPicker :
    public BaseBrush
{
    virtual std::string getIconPath() { return "assets/tool_colorpicker.png"; }
    std::string getName() { return "Color Picker"; }
    void clickPress(MainEditor* editor, XY pos) override;
    void clickDrag(MainEditor* editor, XY from, XY to) override;
};

