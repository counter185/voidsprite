#pragma once
#include "drawable.h"
#include "DrawableManager.h"
#include "EventCallbackListener.h"

class ScrollingView :
    public Drawable, public EventCallbackListener
{
public:
    DrawableManager tabButtons;
    bool scrollVertically = true;
    bool scrollHorizontally = true;
    XY scrollOffset = XY{ 0,0 };
    //std::vector<Tab> tabs;
    //int buttonsHeight = 30;
    //int openTab = 0;
    //SDL_Color tabUnfocusedColor = SDL_Color{ 0,0,0,0x30 };
    //SDL_Color tabFocusedColor = SDL_Color{ 0,0,0,0xe0 };

    ScrollingView() {
        /*int buttonX = 0;
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
        updateTabButtons();*/
    }
    ~ScrollingView() {
        tabButtons.freeAllDrawables();
    }

    bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override {
        return tabButtons.mouseInAny(xyAdd(thisPositionOnScreen, scrollOffset), mousePos);// || tabs[openTab].wxs.mouseInAny(xyAdd(XY{ 0, buttonsHeight }, thisPositionOnScreen), mousePos);
    }

    void render(XY position) override {
        tabButtons.renderAll(xyAdd(position, scrollOffset));
        //tabs[openTab].wxs.renderAll(xyAdd(position, XY{ 0, buttonsHeight }));
    }
    void handleInput(SDL_Event evt, XY gPosOffset) override {
        if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == 1 && evt.button.state) {
            tabButtons.tryFocusOnPoint(XY{ evt.button.x, evt.button.y }, gPosOffset);
        }
        if (tabButtons.anyFocused()) {
            tabButtons.passInputToFocused(evt, gPosOffset);
        }
    }

    void eventButtonPressed(int evt_id) override {
        //openTab = evt_id;
        //updateTabButtons();
    }

};

