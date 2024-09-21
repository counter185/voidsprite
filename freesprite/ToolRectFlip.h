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
    std::string getIconPath() override { return VOIDSPRITE_ASSETS_PATH "assets/tool_fliprect.png"; }
    bool overrideRightClick() override { return true; }
    bool isReadOnly() override { return true; }
    void clickPress(MainEditor* editor, XY pos) override;
    void clickRelease(MainEditor* editor, XY pos) override;
    void rightClickPress(MainEditor* editor, XY pos) override;
    void rightClickRelease(MainEditor* editor, XY pos) override;
    void renderOnCanvas(XY canvasDrawPoint, int scale);

    void doFlip(MainEditor* editor, XY startPos, XY endPos, bool yFlip);
};

