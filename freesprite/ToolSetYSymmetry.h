#pragma once
#include "BaseBrush.h"
class ToolSetYSymmetry : public BaseBrush
{
    bool mouseHeld = false;
    XY lastPos = {0,0};

    std::string getIconPath() { return "assets/tool_setysym.png"; }
    bool isReadOnly() override { return true; }
    bool wantDoublePosPrecision() override { return true; }
    bool overrideRightClick() override { return true; }
    std::string getName() { return "Set Y symmetry"; }
    void clickPress(MainEditor* editor, XY pos) override;
    void clickDrag(MainEditor* editor, XY from, XY to) override;
    void clickRelease(MainEditor* editor, XY pos) override;
    void rightClickPress(MainEditor* editor, XY pos) override;
    void renderOnCanvas(XY canvasDrawPoint, int scale) override;
    void mouseMotion(MainEditor* editor, XY pos) override;
};

