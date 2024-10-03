#pragma once
#include "BaseBrush.h"
#include "Timer64.h"

class ToolRectIsolate :
    public BaseBrush
{
public:
    XY startPos = XY{ 0,0 };
    bool heldDown = false;
    XY lastMousePos = XY{ 0,0 };
    Timer64 clickTimer;

    virtual std::string getIconPath() { return VOIDSPRITE_ASSETS_PATH "assets/tool_rectisolate.png"; }
    bool isReadOnly() override { return true; }
    bool overrideRightClick() override { return true; }
    std::string getName() { return "Isolate rect"; }
    std::string getTooltip() override { return "Select an area with Mouse Left to lock all other brushes to this area.\nDeselect this area with Mouse Right."; }
    void clickPress(MainEditor* editor, XY pos) override;
    void clickDrag(MainEditor* editor, XY from, XY to) override { lastMousePos = to; }
    void clickRelease(MainEditor* editor, XY pos) override;
    void renderOnCanvas(XY canvasDrawPoint, int scale) override;
    void rightClickPress(MainEditor* editor, XY pos) override;
};

