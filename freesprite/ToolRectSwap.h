#pragma once
#include "BaseBrush.h"
#include "mathops.h"

class ToolRectSwap : public BaseBrush
{
    bool mouseDown = false;
    XY mouseDownPoint = XY{ 0,0 };

    uint32_t* clonedArea = NULL;
    SDL_Rect clonedAreaPointAndDimensions;
    SDL_Texture* cacheClonePreview = NULL;

    std::string getName() override { return "Swap rect"; }
    std::string getIconPath() override { return "assets/tool_swapr.png"; }
    bool overrideRightClick() override { return true; }
    void clickPress(MainEditor* editor, XY pos) override;
    void clickRelease(MainEditor* editor, XY pos) override;
    void rightClickPress(MainEditor* editor, XY pos) override;
    bool isReadOnly() { return true; }
    void renderOnCanvas(XY canvasDrawPoint, int scale) {
        if (mouseDown) {
            XY pointFrom = XY{ ixmin(mouseDownPoint.x, lastMouseMotionPos.x), ixmin(mouseDownPoint.y, lastMouseMotionPos.y) };
            XY pointTo = XY{ ixmax(mouseDownPoint.x, lastMouseMotionPos.x), ixmax(mouseDownPoint.y, lastMouseMotionPos.y) };
            pointTo = xyAdd(pointTo, XY{ 1,1 });
            SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x50);
            SDL_Rect cAreaRect = SDL_Rect{
                canvasDrawPoint.x + pointFrom.x * scale,
                canvasDrawPoint.y + pointFrom.y * scale,
                (pointTo.x - pointFrom.x) * scale,
                (pointTo.y - pointFrom.y) * scale
            };
            SDL_RenderDrawRect(g_rd, &cAreaRect);
        }
        else if (clonedArea != NULL) {
            SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
            SDL_Rect cAreaRect = SDL_Rect{
                canvasDrawPoint.x + clonedAreaPointAndDimensions.x * scale,
                canvasDrawPoint.y + clonedAreaPointAndDimensions.y * scale,
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

