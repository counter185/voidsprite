#pragma once
#include "BaseBrush.h"
#include "../Timer64.h"

class ToolRectClone :
    public BaseBrush
{
    bool mouseDown = false;
    XY mouseDownPoint = XY{ 0,0 };
    Timer64 lastClickTimer;
    XY prevReleasePoint = XY{0, 0};

    u32* clonedArea = NULL;
    bool clonedAreaIsIndexed = false;
    std::vector<u32> indexedPalette;
    SDL_Rect clonedAreaPointAndDimensions;
    SDL_Texture* cacheClonePreview = NULL;

    std::string getName() override { return "Clone rect"; }
    std::string getTooltip() override { return "Select an area with Mouse Left.\nPaste it at a different location with Mouse Right."; }
    std::string getIconPath() override { return "tool_cloner.png"; }
    XY getSection() override { return XY{ 1,0 }; }

    bool overrideRightClick() override { return true; }
    void clickPress(MainEditor* editor, XY pos) override;
    void clickRelease(MainEditor* editor, XY pos) override;
    void rightClickPress(MainEditor* editor, XY pos) override;
    bool isReadOnly() override { return true; }
    void renderOnCanvas(MainEditor* editor, int scale) override;
};

