#pragma once
#include "globals.h"
#include "drawable.h"
#include "DrawableManager.h"
#include "UIButton.h"
#include "EventCallbackListener.h"

struct Tab {
    std::string name;
    HotReloadableTexture* icon = NULL;
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
    Fill tabUnfocusedFill = visualConfigFill("ui/tabs/bg_unfocused");
    Fill tabFocusedFill = visualConfigFill("ui/tabs/bg_focused");
    Timer64 tabSwitchTimer;
    bool nextTabSlideFromTheLeft = false;
    std::function<void(TabbedView*,int)> onTabSwitchedCallback = NULL;

    TabbedView(std::vector<Tab> tabN, int buttonWidth = 60);
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
    void mouseHoverOut() override {
        Drawable::mouseHoverOut();
        tabButtons.forceUnhover();
        tabs[openTab].wxs.forceUnhover();
    }

    void render(XY position) override {
        tabButtons.renderAll(position);
        tabs[openTab].wxs.renderAll(xyAdd(position, XY{(int)(200 * (nextTabSlideFromTheLeft ? -1 : 1) * (1.0-XM1PW3P1(tabSwitchTimer.percentElapsedTime(250)))), buttonsHeight}));
    }

    void mouseHoverMotion(XY mousePos, XY gPosOffset) override {
        //todo clean this up.....
        std::vector<std::reference_wrapper<DrawableManager>> wxss = { tabButtons, tabs[openTab].wxs };
        bool hoverEventAtBar = false;
        gPosOffset = xyAdd(gPosOffset, position);
        if (tabButtons.processHoverEvent(gPosOffset, mousePos)) {
            hoverEventAtBar = true;
        }
        if (hoverEventAtBar) {
            tabs[openTab].wxs.forceUnhover();
        }
        else {
            tabs[openTab].wxs.processHoverEvent(xyAdd(gPosOffset, XY{ 0,buttonsHeight }), mousePos);
        }
    }

    void mouseWheelEvent(XY mousePos, XY gPosOffset, XYf direction) override {
        std::vector<std::reference_wrapper<DrawableManager>> wxss = { tabButtons, tabs[openTab].wxs };
        gPosOffset = xyAdd(gPosOffset, position);
        tabs[openTab].wxs.processMouseWheelEvent(xyAdd(gPosOffset, XY{ 0,buttonsHeight }), mousePos, direction);
    }

    void handleInput(SDL_Event evt, XY gPosOffset) override {

        if (evt.type == SDL_MOUSEBUTTONDOWN && (evt.button.button == 1 || evt.button.button == 3) && evt.button.down) {
            if (!tabButtons.tryFocusOnPoint(XY{ (int)evt.button.x, (int)evt.button.y }, gPosOffset)) {
                tabs[openTab].wxs.tryFocusOnPoint(XY{ (int)evt.button.x, (int)evt.button.y }, xyAdd(XY{ 0,buttonsHeight }, gPosOffset));
            }
        }
        else if (evt.type == SDL_EVENT_FINGER_DOWN) {
            XY touchPos = {(int)(evt.tfinger.x * g_windowW), (int)(evt.tfinger.y * g_windowH)};
            if (!tabButtons.tryFocusOnPoint(touchPos, gPosOffset)) {
                tabs[openTab].wxs.tryFocusOnPoint(touchPos, xyAdd(XY{ 0,buttonsHeight }, gPosOffset));
            }
        }
        else if (evt.type == SDL_KEYDOWN && evt.key.scancode == SDL_SCANCODE_TAB) {
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
        if (onTabSwitchedCallback != NULL) {
            onTabSwitchedCallback(this, openTab);
        }
        updateTabButtons();
    }

    void updateTabButtons() {
        for (int x = 0; x < tabButtons.drawablesList.size(); x++) {
            UIButton* btn = (UIButton*)tabButtons.drawablesList[x];
            btn->fill = openTab == x ? tabFocusedFill : tabUnfocusedFill;
        }
    }

    bool takesMouseWheelEvents() override { return true; }
    bool takesTouchEvents() override { return true; }
    bool shouldMoveToFrontOnFocus() override { return true; }

    XY getDimensions() override { 
        XY ret = { 0,0 };
        for (Drawable*& a : tabButtons.drawablesList) {
            XY aPos = a->position;
            XY aDim = a->getRenderDimensions();
            if (aPos.x + aDim.x > ret.x) {
                ret.x = aPos.x + aDim.x;
            }
            if (aPos.y + aDim.y > ret.y) {
                ret.y = aPos.y + aDim.y;
            }
        }
        return ret;
    };
};

