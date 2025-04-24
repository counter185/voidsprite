#pragma once
#include "BaseBrush.h"
#include "Timer64.h"
class ToolSetYSymmetry : public BaseBrush
{
    bool mouseHeld = false;
    XY lastPos = {0,0};
    MainEditor* lastEditor = NULL;
    Timer64 clickTimer;

    std::string getIconPath() override { return "tool_setysym.png"; }
    std::string getName() override { return TL("vsp.brush.ysym"); }
    std::string getTooltip() override { return TL("vsp.brush.ysym.desc"); }
    XY getSection() override { return XY{ 1,1 }; }

    bool isReadOnly() override { return true; }
    bool wantDoublePosPrecision() override { return true; }
    bool overrideRightClick() override { return true; }
    void clickPress(MainEditor* editor, XY pos) override;
    void clickDrag(MainEditor* editor, XY from, XY to) override;
    void clickRelease(MainEditor* editor, XY pos) override;
    void rightClickPress(MainEditor* editor, XY pos) override;
    void renderOnCanvas(XY canvasDrawPoint, int scale) override;
    void mouseMotion(MainEditor* editor, XY pos) override;
};

