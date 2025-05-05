#pragma once
#include "BaseBrush.h"
class ToolColorPicker :
    public BaseBrush
{
    std::string getIconPath() override { return "tool_colorpicker.png"; }
    bool isReadOnly() override { return true; }
    std::string getName() override { return TL("vsp.brush.pickcolor"); }
    std::string getTooltip() override { return TL("vsp.brush.pickcolor.desc"); }
    XY getSection() override { return XY{ 1,1 }; }

    void clickPress(MainEditor* editor, XY pos) override;
    void clickDrag(MainEditor* editor, XY from, XY to) override;
    void renderOnCanvas(XY canvasDrawPoint, int scale) override {
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
        drawLocalPoint(canvasDrawPoint, lastMouseMotionPos, scale);
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
        drawPointOutline(canvasDrawPoint, lastMouseMotionPos, scale);
    }
};

