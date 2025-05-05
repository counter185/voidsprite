#pragma once
#include "BaseBrush.h"
#include "Timer64.h"

class ToolGuideline :
    public BaseBrush
{
    bool mouseLeftHeld = false,
        mouseRightHeld = false;
    XY lastPos = { 0,0 };
    //MainEditor* lastEditor = NULL;
    Timer64 clickTimer;

    std::string getIconPath() override { return "tool_setguide.png"; }
    std::string getName() override { return TL("vsp.brush.guideline"); }
    std::string getTooltip() override { return TL("vsp.brush.guideline.desc"); }
    XY getSection() override { return XY{ 1,1 }; }

    bool isReadOnly() override { return true; }
    bool wantDoublePosPrecision() override { return true; }
    bool overrideRightClick() override { return true; }
    void clickPress(MainEditor* editor, XY pos) override;
    void clickDrag(MainEditor* editor, XY from, XY to) override;
    void clickRelease(MainEditor* editor, XY pos) override;
    void rightClickPress(MainEditor* editor, XY pos) override;
    void rightClickRelease(MainEditor* editor, XY pos) override;
    void renderOnCanvas(XY canvasDrawPoint, int scale) override;
};

