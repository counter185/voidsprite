#pragma once
#include "BaseBrush.h"

class ToolRectSwap : public BaseBrush
{
    bool mouseDown = false;
    XY mouseDownPoint = XY{ 0,0 };

    uint32_t* clonedArea = NULL;
    SDL_Rect clonedAreaPointAndDimensions;
    SDL_Texture* cacheClonePreview = NULL;

    std::string getName() override { return "Swap rect"; }
    std::string getTooltip() override { return "Select an area with Mouse Left.\nSwap the source and selected destination areas with Mouse Right."; }
    std::string getIconPath() override { return "tool_swapr.png"; }
    XY getSection() override { return XY{ 1,0 }; }

    bool overrideRightClick() override { return true; }
    void clickPress(MainEditor* editor, XY pos) override;
    void clickRelease(MainEditor* editor, XY pos) override;
    void rightClickPress(MainEditor* editor, XY pos) override;
    bool isReadOnly() override { return true; }
    void renderOnCanvas(MainEditor* editor, int scale) override;

};

