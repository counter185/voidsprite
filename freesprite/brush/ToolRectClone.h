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
    void renderOnCanvas(XY canvasDrawPoint, int scale) override {
        if (mouseDown) {
            drawPixelRect(mouseDownPoint, lastMouseMotionPos, canvasDrawPoint, scale);
        }
        else if (clonedArea != NULL) {
            SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
            SDL_Rect cAreaRect = SDL_Rect{
                canvasDrawPoint.x + clonedAreaPointAndDimensions.x*scale, 
                canvasDrawPoint.y + clonedAreaPointAndDimensions.y*scale,
                clonedAreaPointAndDimensions.w * scale,
                clonedAreaPointAndDimensions.h * scale
            };
            SDL_RenderDrawRect(g_rd, &cAreaRect);

            SDL_Rect previewRect = SDL_Rect{
                canvasDrawPoint.x + lastMouseMotionPos.x * scale,
                canvasDrawPoint.y + lastMouseMotionPos.y * scale,
                clonedAreaPointAndDimensions.w * scale,
                clonedAreaPointAndDimensions.h * scale
            };
            SDL_RenderCopy(g_rd, cacheClonePreview, NULL, &previewRect);
            SDL_RenderDrawRect(g_rd, &previewRect);
        }

        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
        drawLocalPoint(canvasDrawPoint, lastMouseMotionPos, scale);
    }

};

