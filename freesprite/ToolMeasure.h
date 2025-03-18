#pragma once
#include "BaseBrush.h"
#include "Timer64.h"
class ToolMeasure :
    public BaseBrush
{
    XY startPos = XY{ 0,0 };
    bool heldDown = false;
    XY lastMousePos = XY{ 0,0 };
    Timer64 clickTimer;

    virtual std::string getIconPath() { return VOIDSPRITE_ASSETS_PATH "assets/tool_measure.png"; }
    bool isReadOnly() override { return true; }
    std::string getName() { return "Measure"; }
    std::string getTooltip() override { return "Select an area with Mouse Left to measure its size in pixels"; }
    XY getSection() override { return XY{ 1,1 }; }

    void clickPress(MainEditor* editor, XY pos) override;
    void clickDrag(MainEditor* editor, XY from, XY to) override { (void) editor, (void) from, lastMousePos = to; }
    void clickRelease(MainEditor* editor, XY pos) override { (void) editor, (void) pos, heldDown = false; }
    void renderOnCanvas(XY canvasDrawPoint, int scale) override;
};

