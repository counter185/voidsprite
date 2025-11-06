#pragma once
#include "drawable.h"
#include "DrawableManager.h"
#include "EventCallbackListener.h"

class UIDropdown :
    public Drawable, EventCallbackListener
{
private:
    XY lastPosOnScreen{};
public:
    std::string text;
    int wxWidth = 250, wxHeight = 30;
    SDL_Texture* icon = NULL;

    SDL_Color colorBGFocused = SDL_Color{ 0,0,0,0xff };
    SDL_Color colorBGUnfocused = SDL_Color{ 0,0,0,0xd0 };
    SDL_Color colorTextFocused = SDL_Color{ 255,255,255,0xff };
    SDL_Color colorTextUnfocused = SDL_Color{ 255,255,255,0xd0 };

    Timer64 lastClick;
    
    bool isOpen = false;
    bool setTextToSelectedItem = false;
    Timer64 openTimer;

    std::vector<std::string> items;
    std::vector<std::string> tooltips;

    std::function<void(UIDropdown*, int, std::string)> onDropdownItemSelectedCallback = NULL;

    UIDropdown(std::vector<std::string> items);
    UIDropdown(std::vector<std::pair<std::string, std::string>> items);

    bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override {
        return pointInBox(mousePos, SDL_Rect{ thisPositionOnScreen.x, thisPositionOnScreen.y, wxWidth, wxHeight });
    }
    void render(XY pos) override;
    void handleInput(SDL_Event evt, XY gPosOffset) override;
    XY getDimensions() override { 
        return XY{ wxWidth, wxHeight};
    };

    void eventButtonPressed(int evt_id) override;

    void renderDropdownIcon(XY pos);

    std::vector<UIButton*> genButtonsList(UIButton* (*customButtonGenFunction)(std::string name, std::string item) = NULL);

    bool takesMouseWheelEvents() override { return true; }
    bool takesTouchEvents() override { return true; }

    void click();
};

