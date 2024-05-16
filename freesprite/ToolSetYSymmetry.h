#pragma once
#include "BaseBrush.h"
class ToolSetYSymmetry : public BaseBrush
{
    virtual std::string getIconPath() { return "assets/tool_setysym.png"; }
    bool isReadOnly() override { return true; }
    bool wantDoublePosPrecision() override { return true; }
    bool overrideRightClick() override { return true; }
    std::string getName() { return "Set Y symmetry"; }
    void clickPress(MainEditor* editor, XY pos) override;
    void clickDrag(MainEditor* editor, XY from, XY to) override;
    void rightClickPress(MainEditor* editor, XY pos) override;
};

