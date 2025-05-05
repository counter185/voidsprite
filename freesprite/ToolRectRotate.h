#pragma once
#include "BaseBrush.h"
class ToolRectRotate :
    public BaseBrush
{
public:
    bool dragging = false;
    XY dragStart = XY{ 0,0 };
    bool dragRightClick = false;

    std::string getName() override { return "Rotate rect"; }
    std::string getIconPath() override { return "tool_rotrect.png"; }
    XY getSection() override { return XY{ 1,0 }; }

    bool overrideRightClick() override { return true; }
    bool isReadOnly() override { return true; }
    void clickPress(MainEditor* editor, XY pos) override;
    void clickRelease(MainEditor* editor, XY pos) override;
    void rightClickPress(MainEditor* editor, XY pos) override;
    void rightClickRelease(MainEditor* editor, XY pos) override;
    void renderOnCanvas(XY canvasDrawPoint, int scale);

    void doRotate(MainEditor* editor, XY startPos, XY endPos, bool ccl);
};

