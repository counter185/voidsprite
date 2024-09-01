#pragma once
#include "globals.h"
#include "drawable.h"
#include "DrawableManager.h"
#include "UIButton.h"
#include "EventCallbackListener.h"

struct Tab {
    std::string name;
    SDL_Texture* icon = NULL;
    DrawableManager wxs;
};

class TabbedView :
    public Drawable, EventCallbackListener
{
public:
    DrawableManager tabButtons;
    std::vector<Tab> tabs;
    int buttonsHeight = 30;
    int openTab = 0;
    SDL_Color tabUnfocusedColor = SDL_Color{ 0,0,0,0x30 };
    SDL_Color tabFocusedColor = SDL_Color{ 0,0,0,0xe0 };
    Timer64 tabSwitchTimer;
    bool nextTabSlideFromTheLeft = false;

    TabbedView(std::vector<Tab> tabN, int buttonWidth = 60) {
        int buttonX = 0;
        for (int x = 0; x < tabN.size(); x++) {
            UIButton* nbutton = new UIButton();
            nbutton->wxWidth = buttonWidth;
            nbutton->wxHeight = buttonsHeight;
            nbutton->position = XY{ buttonX, 0 };
            nbutton->text = tabN[x].name;
            nbutton->icon = tabN[x].icon;
            buttonX += nbutton->wxWidth;
            nbutton->setCallbackListener(x, this);
            tabButtons.addDrawable(nbutton);

            tabs.push_back(tabN[x]);
        }
        updateTabButtons();
        tabSwitchTimer.start();
    }
    ~TabbedView() {
        tabButtons.freeAllDrawables();
        for (Tab& t : tabs) {
            t.wxs.freeAllDrawables();
        }
    }

    bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override {
        return tabButtons.mouseInAny(thisPositionOnScreen, mousePos) || tabs[openTab].wxs.mouseInAny(xyAdd(XY{0, buttonsHeight}, thisPositionOnScreen), mousePos);
    }

    void focusOut() override {
		Drawable::focusOut();
		tabs[openTab].wxs.forceUnfocus();
	}

    void render(XY position) override {
        tabButtons.renderAll(position);
        tabs[openTab].wxs.renderAll(xyAdd(position, XY{(int)(200 * (nextTabSlideFromTheLeft ? -1 : 1) * (1.0-XM1PW3P1(tabSwitchTimer.percentElapsedTime(250)))), buttonsHeight}));
    }
    void handleInput(SDL_Event evt, XY gPosOffset) override {
        if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == 1 && evt.button.state) {
            if (!tabButtons.tryFocusOnPoint(XY{ evt.button.x, evt.button.y }, gPosOffset)) {
                tabs[openTab].wxs.tryFocusOnPoint(XY{ evt.button.x, evt.button.y }, xyAdd(XY{ 0,buttonsHeight }, gPosOffset));
            }
        }
        else if (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_TAB) {
            tabs[openTab].wxs.tryFocusOnNextTabbable();
        }
        if (tabButtons.anyFocused()) {
            tabButtons.passInputToFocused(evt, gPosOffset);
        }
        else if (tabs[openTab].wxs.anyFocused()) {
            tabs[openTab].wxs.passInputToFocused(evt, xyAdd(XY{ 0,buttonsHeight }, gPosOffset));
        }
    }

    void eventButtonPressed(int evt_id) override {
        if (openTab != evt_id) {
            tabSwitchTimer.start();
            if (openTab < evt_id) {
				nextTabSlideFromTheLeft = false;
			}
			else {
				nextTabSlideFromTheLeft = true;
			}
        }
        openTab = evt_id;
        updateTabButtons();
    }

    void updateTabButtons() {
        for (int x = 0; x < tabButtons.drawablesList.size(); x++) {
            UIButton* btn = (UIButton*)tabButtons.drawablesList[x];
            btn->colorBGFocused = btn->colorBGUnfocused = openTab == x ? tabFocusedColor : tabUnfocusedColor;
        }
    }
};

