#pragma once
#include "BaseBrush.h"
#include "../Timer64.h"

class ToolMeasure :
    public BaseBrush
{
    XY startPos = XY{ 0,0 };
    bool heldDown = false;
    XY lastMousePos = XY{ 0,0 };
    Timer64 clickTimer;

    std::string getIconPath() override { return "tool_measure.png"; }
    bool isReadOnly() override { return true; }
    std::string getName() override { return TL("vsp.brush.measure"); }
    std::string getTooltip() override { return TL("vsp.brush.measure.desc"); }
    XY getSection() override { return XY{ 1,1 }; }

    void clickPress(MainEditor* editor, XY pos) override;
    void clickDrag(MainEditor* editor, XY from, XY to) override { lastMousePos = to; }
    void clickRelease(MainEditor* editor, XY pos) override { heldDown = false; }
    void renderOnCanvas(XY canvasDrawPoint, int scale) override;
};

