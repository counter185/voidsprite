#pragma once
#include "globals.h"
#include "Panel.h"
#include "UIButton.h"

class Tab {
public:
    std::string name;
    HotReloadableTexture* icon = NULL;
    Panel* tabPanel = NULL;

    void add(Drawable* d) { tabPanel->subWidgets.addDrawable(d); }
};

class TabbedView :
    public Panel
{
public:
    std::vector<UIButton*> tabButtons;
    std::vector<Tab> tabs;
    int buttonsHeight = 30;
    int openTab = 0;
    Fill tabUnfocusedFill = visualConfigFill("ui/tabs/bg_unfocused");
    Fill tabFocusedFill = visualConfigFill("ui/tabs/bg_focused");
    Timer64 tabSwitchTimer;
    bool nextTabSlideFromTheLeft = false;
    std::function<void(TabbedView*,int)> onTabSwitchedCallback = NULL;

    TabbedView(std::vector<Tab> tabN, int buttonWidth = 60);

    void render(XY position) override {
        //update opentab position
        tabs[openTab].tabPanel->position.x = (int)(200 * (nextTabSlideFromTheLeft ? -1 : 1) * (1.0 - XM1PW3P1(tabSwitchTimer.percentElapsedTime(250))));
        Panel::render(position);
    }

    void switchTab(int index) {
        if (openTab != index) {
            tabSwitchTimer.start();
            nextTabSlideFromTheLeft = openTab > index;
        }
        openTab = index;
        if (onTabSwitchedCallback != NULL) {
            onTabSwitchedCallback(this, openTab);
        }
        updateTabButtonsAndState();
    }

    void updateTabButtonsAndState() {
        for (int x = 0; x < tabs.size(); x++) {
            UIButton* btn = tabButtons[x];
            Panel* tabPanel = tabs[x].tabPanel;
            btn->fill = openTab == x ? tabFocusedFill : tabUnfocusedFill;
            tabPanel->enabled = openTab == x;
        }
    }
};

