#pragma once
#include "BaseBrush.h"
#include "../Timer64.h"

class ToolMeasure :
    public BaseBrush
{
    XY startPos = XY{ 0,0 };
    bool heldDown = false;
    XY lastMousePos = XY{ 0,0 };
    Timer64 clickTimer;
    XY lastOrigin = { -1,-1 }, lastEnd = { -1,-1 };

    std::string getIconPath() override { return "tool_measure.png"; }
    bool isReadOnly() override { return true; }
    std::string getName() override { return TL("vsp.brush.measure"); }
    std::string getTooltip() override { return TL("vsp.brush.measure.desc:v2"); }
    bool overrideRightClick() { return true; }
    XY getSection() override { return XY{ 1,1 }; }

    void clickPress(MainEditor* editor, XY pos) override;
    void clickDrag(MainEditor* editor, XY from, XY to) override;
    void clickRelease(MainEditor* editor, XY pos) override;
    void rightClickPress(MainEditor* editor, XY pos) override;
    void renderOnCanvas(MainEditor* editor, int scale) override;

    SDL_Rect getSelectedRegion();

    void editorPlaceGuidelinesAroundSelRegion(MainEditor* editor);
    void editorCropToSelRegion(MainEditor* editor);
};

