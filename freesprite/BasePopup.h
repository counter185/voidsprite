#pragma once
#include "globals.h"
#include "BaseScreen.h"
#include "DrawableManager.h"

class BasePopup :
    public BaseScreen
{
public:
    virtual ~BasePopup() = default;
    Timer64 startTimer;

    virtual bool takesInput() { return true; }

    void takeInput(SDL_Event evt) override {
        DrawableManager::processHoverEventInMultiple({ wxsManager }, evt, getPopupOrigin());
        if (!DrawableManager::processInputEventInMultiple({ wxsManager }, evt, getPopupOrigin())) {
            defaultInputAction(evt);
        }
    }

    void render() override {
        renderDrawables();
    }

    void setSize(XY size) {
        wxWidth = size.x;
        wxHeight = size.y;
    }

protected:
    int wxWidth = 600;
    int wxHeight = 400;

    void renderDrawables() {
        wxsManager.renderAll(getPopupOrigin());
    }

    virtual void renderDefaultBackground(SDL_Color bgColor = SDL_Color{0,0,0,0xD0});

    virtual XY getPopupOrigin() {
        return XY{ g_windowW / 2 - wxWidth / 2, g_windowH / 2 - wxHeight / 2 };
    }

    XY getDefaultTitlePosition() {
        return xyAdd(getPopupOrigin(), XY{5,5});
    }
    XY getDefaultContentPosition() {
        return xyAdd(getPopupOrigin(), XY{ 5,50 });
    }

    void closePopup();

    virtual void defaultInputAction(SDL_Event evt) {}
};

