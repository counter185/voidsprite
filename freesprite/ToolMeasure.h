#pragma once
#include "BaseBrush.h"
class ToolMeasure :
    public BaseBrush
{
    XY startPos = XY{ 0,0 };
    bool heldDown = false;
    XY lastMousePos = XY{ 0,0 };

    virtual std::string getIconPath() { return VOIDSPRITE_ASSETS_PATH "assets/tool_measure.png"; }
    bool isReadOnly() override { return true; }
    std::string getName() { return "Measure"; }
    void clickPress(MainEditor* editor, XY pos) override;
    void clickDrag(MainEditor* editor, XY from, XY to) override { lastMousePos = to; }
    void clickRelease(MainEditor* editor, XY pos) override { heldDown = false; }
    void renderOnCanvas(XY canvasDrawPoint, int scale) override;
};

