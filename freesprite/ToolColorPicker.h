#pragma once
#include "BaseBrush.h"
class ToolColorPicker :
    public BaseBrush
{
    virtual std::string getIconPath() { return VOIDSPRITE_ASSETS_PATH "assets/tool_colorpicker.png"; }
    bool isReadOnly() override { return true; }
    std::string getName() { return "Color Picker"; }
    std::string getTooltip() override { return "Mouse Left to pick a color from the current layer."; }
    void clickPress(MainEditor* editor, XY pos) override;
    void clickDrag(MainEditor* editor, XY from, XY to) override;
    void renderOnCanvas(XY canvasDrawPoint, int scale) {
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
        drawLocalPoint(canvasDrawPoint, lastMouseMotionPos, scale);
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
        drawPointOutline(canvasDrawPoint, lastMouseMotionPos, scale);
    }
};

