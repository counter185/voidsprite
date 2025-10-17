#pragma once
#include "globals.h"
#include "BaseScreen.h"
#include "DrawableManager.h"

#define CLOSE_ON_SDL_QUIT if(evt.type==SDL_EVENT_QUIT){closePopup();return;}

class BasePopup :
    public BaseScreen
{
protected:
    bool usesWholeScreen = false;
public:
    virtual ~BasePopup() = default;
    Timer64 startTimer;

    virtual bool takesInput() { return true; }
    bool takesTouchEvents() override { return true; }

    void takeInput(SDL_Event evt) override {
        CLOSE_ON_SDL_QUIT;
        DrawableManager::processHoverEventInMultiple({ wxsManager }, evt, getPopupOrigin());
        if (!DrawableManager::processInputEventInMultiple({ wxsManager }, evt, getPopupOrigin())) {
            defaultInputAction(evt);
        }
    }

    void render() override {
        renderDefaultBackground();
        renderDrawables();
    }

    void setSize(XY size) {
        wxWidth = size.x;
        wxHeight = size.y;
    }
private:
    int nextActionButtonX = -1;
    bool actionButtonXInit = false;
protected:
    int wxWidth = 600;
    int wxHeight = 400;

    void renderDrawables() {
        wxsManager.renderAll(getPopupOrigin());
    }

    virtual void renderDefaultBackground();

    void renderPopupWindow();

    virtual XY getPopupOrigin() {
        return usesWholeScreen ? XY{0,0} : XY{ g_windowW / 2 - wxWidth / 2, g_windowH / 2 - wxHeight / 2 };
    }

    virtual void playPopupCloseVFX();
    void closePopup();

    virtual void defaultInputAction(SDL_Event evt) {}

    XY makeTitleAndDesc(std::string title = "", std::string desc = "");

    UIButton* actionButton(std::string text, int width = 120);
};

