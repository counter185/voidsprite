#pragma once
#include "BaseBrush.h"

class ToolRectMove :
    public BaseBrush
{
    bool mouseDown = false;
    XY mouseDownPoint = XY{ 0,0 };

    uint32_t* clonedArea = NULL;
    bool clonedAreaIsIndexed = false;
    std::vector<u32> indexedPalette;
    SDL_Rect clonedAreaPointAndDimensions;
    SDL_Texture* cacheClonePreview = NULL;

    std::string getName() override { return "Move rect"; }
    std::string getTooltip() override { return "Select an area with Mouse Left to cut it.\nPaste it at a different location with Mouse Right."; }
    std::string getIconPath() override { return "tool_mover.png"; }
    XY getSection() override { return XY{ 1,0 }; }

    bool overrideRightClick() override { return true; }
    void clickPress(MainEditor* editor, XY pos) override;
    void clickRelease(MainEditor* editor, XY pos) override;
    void rightClickPress(MainEditor* editor, XY pos) override;
    void renderOnCanvas(MainEditor* editor, int scale) override;
};

