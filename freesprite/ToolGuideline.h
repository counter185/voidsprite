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

    std::string getIconPath() { return VOIDSPRITE_ASSETS_PATH "assets/tool_setguide.png"; }
    std::string getTooltip() override { return "Mouse Left to place/edit vertical guidelines.\nMouse Right to place/edit horizontal guidelines.\nUse Eraser mode to remove guidelines."; }
    bool isReadOnly() override { return true; }
    bool wantDoublePosPrecision() override { return true; }
    bool overrideRightClick() override { return true; }
    std::string getName() { return "Guideline tool"; }
    void clickPress(MainEditor* editor, XY pos) override;
    void clickDrag(MainEditor* editor, XY from, XY to) override;
    void clickRelease(MainEditor* editor, XY pos) override;
    void rightClickPress(MainEditor* editor, XY pos) override;
    void rightClickRelease(MainEditor* editor, XY pos) override;
    void renderOnCanvas(XY canvasDrawPoint, int scale) override;
};

