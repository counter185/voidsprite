#pragma once
#include "globals.h"
#include "mathops.h"
#include "drawable.h"

#define FILL_BUTTON_CHECKED_DEFAULT Fill::Solid(0xD0000000)

class UIButton : public Drawable
{
public:
    std::string text;
    std::string tooltip;
    int wxWidth = 250, wxHeight = 30;
    SDL_Texture* icon = NULL;
    bool fullWidthIcon = false;
    bool instantTooltip = false;
    
    Fill fill = FILL_BUTTON_CHECKED_DEFAULT;
    SDL_Color colorTextFocused = SDL_Color{ 255,255,255,0xff };
    SDL_Color colorTextUnfocused = SDL_Color{ 255,255,255,0xd0 };

    SDL_Color colorBorder = SDL_Color{ 0xff,0xff,0xff,0x30 };

    Timer64 lastClick;

    std::function<void(UIButton*)> onClickCallback = NULL;
    std::function<void(UIButton*)> onRightClickCallback = NULL;

    UIButton(std::string text = "", std::string tooltip = "") : Drawable() 
    {
        this->text = text;
        this->tooltip = tooltip;
    }

    bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override {
        return pointInBox(mousePos, SDL_Rect{ thisPositionOnScreen.x, thisPositionOnScreen.y, wxWidth, wxHeight });
    }
    void render(XY pos) override;
    void focusIn() override;
    void handleInput(SDL_Event evt, XY gPosOffset) override;
    XY getDimensions() override { return XY{ wxWidth, wxHeight }; };

    void renderAnimations(XY pos);
    virtual void renderTooltip(XY pos);

    virtual void click();
    virtual void rightClick();
};

