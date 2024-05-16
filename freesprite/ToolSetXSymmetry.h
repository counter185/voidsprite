#pragma once
#include "BaseBrush.h"
class ToolSetXSymmetry : public BaseBrush
{
    virtual std::string getIconPath() { return "assets/tool_setxsym.png"; }
    bool isReadOnly() override { return true; }
    bool wantDoublePosPrecision() override { return true; }
    bool overrideRightClick() override { return true; }
    std::string getName() { return "Set X symmetry"; }
    void clickPress(MainEditor* editor, XY pos) override;
    void clickDrag(MainEditor* editor, XY from, XY to) override;
    void rightClickPress(MainEditor* editor, XY pos) override;
};

