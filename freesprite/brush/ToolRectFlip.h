#pragma once
#include "BaseBrush.h"

class ToolRectFlip :
    public BaseBrush
{
public:
    bool dragging = false;
    XY dragStart = XY{ 0,0 };
    bool dragRightClick = false;

    std::string getName() override { return "Flip rect"; }
    std::string getTooltip() override { return "Select an area with Mouse Left to flip it horizontally.\nSelect an area with Mouse Right to flip it vertically."; }
    std::string getIconPath() override { return "tool_fliprect.png"; }
    XY getSection() override { return XY{ 1,0 }; }

    bool overrideRightClick() override { return true; }
    bool isReadOnly() override { return true; }
    void clickPress(MainEditor* editor, XY pos) override;
    void clickRelease(MainEditor* editor, XY pos) override;
    void rightClickPress(MainEditor* editor, XY pos) override;
    void rightClickRelease(MainEditor* editor, XY pos) override;
    void renderOnCanvas(XY canvasDrawPoint, int scale) override;

    void doFlip(MainEditor* editor, XY startPos, XY endPos, bool yFlip);
};

