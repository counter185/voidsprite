#pragma once
#include "BaseBrush.h"
class ToolSetXSymmetry : public BaseBrush
{
    bool mouseHeld = false;
    XY lastPos = { 0,0 };
    MainEditor* lastEditor = NULL;

    std::string getIconPath() { return "assets/tool_setxsym.png"; }
    bool isReadOnly() override { return true; }
    bool wantDoublePosPrecision() override { return true; }
    bool overrideRightClick() override { return true; }
    std::string getName() { return "Set X symmetry"; }
    void clickPress(MainEditor* editor, XY pos) override;
    void clickDrag(MainEditor* editor, XY from, XY to) override;
    void clickRelease(MainEditor* editor, XY pos) override;
    void rightClickPress(MainEditor* editor, XY pos) override;
    void renderOnCanvas(XY canvasDrawPoint, int scale) override;
    void mouseMotion(MainEditor* editor, XY pos) override;
};

