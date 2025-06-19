#pragma once
#include "BaseBrush.h"
#include "../EventCallbackListener.h"

class ToolText :
    public BaseBrush, EventCallbackListener
{
public:
    TTF_Font* font = NULL;
    std::string text = "";
    int textSize = 16;
    SDL_Surface* textSurface = NULL;
    SDL_Texture* cachedTextTexture = NULL;

    std::string getName() override { return "Text"; }
    std::string getIconPath() override { return "tool_text.png"; }
    XY getSection() override { return XY{ 0,2 }; }

    bool overrideRightClick() override { return true; }
    void clickPress(MainEditor* editor, XY pos) override;
    void rightClickPress(MainEditor* editor, XY pos) override;
    void renderOnCanvas(XY canvasDrawPoint, int scale) override;

    void eventPopupClosed(int evt_id, BasePopup* target) override;

    void renderText();
};

