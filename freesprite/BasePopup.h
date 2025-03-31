#pragma once
#include "globals.h"
#include "BaseScreen.h"
#include "DrawableManager.h"
#include "UILabel.h"
#include "UIButton.h"

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

    virtual void renderDefaultBackground(SDL_Color bgColor = SDL_Color{0,0,0,0xD0});

    virtual XY getPopupOrigin() {
        return XY{ g_windowW / 2 - wxWidth / 2, g_windowH / 2 - wxHeight / 2 };
    }

    void closePopup();

    virtual void defaultInputAction(SDL_Event evt) {}

    void makeTitleAndDesc(std::string title = "", std::string desc = "") {
        XY titlePos = { 5,5 };
        XY contentPos = { 5,50 };

        if (title != "") {
            UILabel* titleLbl = new UILabel(title);
            titleLbl->position = titlePos;
            titleLbl->fontsize = 22;
            wxsManager.addDrawable(titleLbl);
        }

        if (desc != "") {
            UILabel* descLbl = new UILabel(desc);
            descLbl->position = contentPos;
            wxsManager.addDrawable(descLbl);
        }
    }

    UIButton* actionButton(std::string text) {
        if (!actionButtonXInit) {
            nextActionButtonX = wxWidth - 130;
            actionButtonXInit = true;
        }
        UIButton* nbutton = new UIButton();
        nbutton->text = text;
        nbutton->position = XY{ nextActionButtonX, wxHeight - 40 };
        nextActionButtonX -= 130;
        nbutton->wxWidth = 120;
        wxsManager.addDrawable(nbutton);
        return nbutton;
    }
};

