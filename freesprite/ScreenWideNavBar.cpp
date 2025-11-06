#include "ScreenWideNavBar.h"
#include "FontRenderer.h"
#include "UIButton.h"

ScreenWideNavBar::ScreenWideNavBar(BaseScreen* caller, std::map<SDL_Scancode, NavbarSection> actions, std::vector<SDL_Scancode> order) {
    wxHeight = 30;
    parent = caller;
    submenuOrder = order;
    keyBinds = actions;

    submenuPanel = new Panel();
    submenuPanel->enabled = false;
    subWidgets.addDrawable(submenuPanel);

    int x = 10;
    int xDist = 120;

    //determine the right width
    for (auto& editorSection : submenuOrder) {
        std::string fullSectionName = keyBinds[editorSection].name + frmt("({})", SDL_GetScancodeName(editorSection));
        int w = g_fnt->StatStringDimensions(fullSectionName).x + 10 + (keyBinds[editorSection].icon != NULL ? 30 : 0) + 10;
        xDist = ixmax(xDist, w);
    }

    position = XY{ 0,0 };
    for (auto& editorSection : submenuOrder) {
        UIButton* sectionButton = new UIButton();
        sectionButton->position = { x, 1 };
        sectionButton->text = keyBinds[editorSection].name;
        sectionButton->fill = Fill::Gradient(0x70424242, 0x70424242, 0x70000000, 0x70000000);
        sectionButton->colorTextFocused = sectionButton->colorTextUnfocused = SDL_Color{ 255,255,255,0xd0 };
        sectionButton->wxWidth = xDist - 10;
        if (keyBinds[editorSection].icon != NULL) {
            sectionButton->icon = keyBinds[editorSection].icon;
        }
        sectionButton->onClickCallback = [this, editorSection](UIButton* btn) {
            this->openSubmenu(editorSection);
        };
        keyBinds[editorSection].button = sectionButton;
        subWidgets.addDrawable(sectionButton);
        x += xDist;
    }
}

void ScreenWideNavBar::render(XY position) {
    wxWidth = g_windowW;

    //fill navbar background
    static Fill bgUnfocused = visualConfigFill("navbar/bg_unfocused");
    static Fill bgFocused = visualConfigFill("navbar/bg_focused");

    SDL_Rect r = SDL_Rect{ 0,0,g_windowW, wxHeight };
    (focused ? bgFocused : bgUnfocused).fill(r);

    if (focused) {
        if (currentSubmenuOpen == SCANCODE_NONE) {
            for (auto& [scancode, section] : keyBinds) {
                if (section.button != NULL) {
                    XY underButton = xyAdd(section.button->position, { 0, (int)(section.button->wxHeight * XM1PW3P1(focusTimer.percentElapsedTime(200))) });
                    SDL_Rect underButtonRect = { underButton.x, underButton.y, section.button->wxWidth, 60 };
                    Fill::ThreePointVerticalGradient(0xFF000000, 0xFF000000, 0xE0000000, 0xE0000000, 0x00000000, 0x00000000).fill(underButtonRect);
                    g_fnt->RenderString(frmt("[{}]", SDL_GetScancodeName(scancode)), underButton.x + 5, underButton.y, {200,200,200,(u8)(220 * focusTimer.percentElapsedTime(200))}, 26);
                }
            }
        }
        else {
            int i = 0;
            for (auto& [subScancode, action] : keyBinds[(SDL_Scancode)currentSubmenuOpen].actions) {
                if (i >= submenuActionsNow.size() || submenuPanel == NULL) {
                    logerr("[ScreenWideNavBar] this should not happen");
                }
                else {
                    UIButton* btn = submenuActionsNow[i];
                    XY buttonEndpoint = xyAdd(xyAdd(submenuPanel->position, btn->position), { (int)(btn->wxWidth - 30 + (30 * XM1PW3P1(submenuOpenTimer.percentElapsedTime(200)))), 0 });
                    SDL_Rect rightOfButtonRect = { buttonEndpoint.x, buttonEndpoint.y, 90, btn->wxHeight };
                    Fill::Gradient(0xFF000000, 0x00000000, 0xFF000000, 0x00000000).fill(rightOfButtonRect);
                    g_fnt->RenderString(frmt("[{}]", SDL_GetScancodeName(subScancode)), buttonEndpoint.x + 5, buttonEndpoint.y, { 230, 230, 230, (u8)(220 * submenuOpenTimer.percentElapsedTime(200)) }, 18);
                }
                i++;
            }
        }

        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xa0);
        drawLine(XY{ 0, wxHeight }, XY{ g_windowW, wxHeight }, XM1PW3P1(focusTimer.percentElapsedTime(600)));
    }
    submenuPanel->position.y = (int)(30 * XM1PW3P1(submenuOpenTimer.percentElapsedTime(200)));
    subWidgets.renderAll(position);
}

inline void ScreenWideNavBar::handleInput(SDL_Event evt, XY gPosOffset) {

    DrawableManager::processHoverEventInMultiple({ subWidgets }, evt, position);

    //special case here
    if (evt.type == SDL_KEYDOWN) {
        tryPressHotkey(evt.key.scancode);
    }
    else if (focused && evt.type == SDL_EVENT_WINDOW_FOCUS_LOST && SDL_GetWindowID(g_wd) == evt.window.windowID) {
        parentManager->forceUnfocus();
    }

    DrawableManager::processInputEventInMultiple({ subWidgets }, evt, position);

}

void ScreenWideNavBar::tryPressHotkey(SDL_Scancode k) {
    if (currentSubmenuOpen == SCANCODE_NONE) {
        if (keyBinds.contains(k)) {
            //openSubmenu(k);
            keyBinds[k].button->click();
        }
        else if (k == SDL_SCANCODE_ESCAPE) {
            parentManager->forceUnfocus();
        }
    }
    else {
        if (k == SDL_SCANCODE_ESCAPE) {
            openSubmenu(SCANCODE_NONE);
        }
        else {
            doSubmenuAction(k);
        }
    }
}

void ScreenWideNavBar::openSubmenu(SDL_Scancode which) {
    if (currentSubmenuOpen == which) {
        currentSubmenuOpen = SCANCODE_NONE;
        updateCurrentSubmenu();
        return;
    }
    currentSubmenuOpen = SCANCODE_NONE;
    submenuOpenTimer.start();
    updateCurrentSubmenu();
    if (which != SCANCODE_NONE) {
        currentSubmenuOpen = which;
        updateCurrentSubmenu();
    }
}

void ScreenWideNavBar::doSubmenuAction(SDL_Scancode which) {
    if (currentSubmenuOpen != SCANCODE_NONE && keyBinds[(SDL_Scancode)currentSubmenuOpen].actions.contains(which)) {
        auto& action = keyBinds[(SDL_Scancode)currentSubmenuOpen].actions[which];
        if (action.function != NULL) {
            action.function();
            openSubmenu(SCANCODE_NONE);
            parentManager->forceUnfocus();
        }
    }
}

void ScreenWideNavBar::updateCurrentSubmenu() {
    if (currentSubmenuOpen == SCANCODE_NONE) {
        submenuPanel->subWidgets.freeAllDrawables();
        submenuActionsNow.clear();
        submenuPanel->enabled = false;
    }
    else {
        submenuPanel->enabled = true;
        auto submenuNow = keyBinds[(SDL_Scancode)currentSubmenuOpen];
        submenuActionsNow.clear();
        XY submenuOrigin = { submenuNow.button->position.x, 0 };

        //find the button width
        int buttonW = 30;
        for (auto& [scancode, action] : submenuNow.actions) {
            buttonW = ixmax(g_fnt->StatStringDimensions(action.name, 18).x + 40, buttonW);
        }
        if ((submenuOrigin.x + buttonW) > g_windowW) {
            submenuOrigin.x = g_windowW - buttonW;
        }
        submenuPanel->position = { submenuOrigin.x, 0 };

        //generate the buttons
        int outOfOrderY = submenuNow.order.size() * 30;
        for (auto& [scancode, action] : submenuNow.actions) {
            if (action.function != NULL) {
                UIButton* newBtn = new UIButton(action.name);
                newBtn->wxHeight = 30;
                std::vector<SDL_Scancode> order = submenuNow.order;
                int indexInOrder = (std::find(order.begin(), order.end(), scancode) - order.begin());
                int yPosition = indexInOrder * 30;
                //handle bindings that aren't in the order list
                if (indexInOrder == order.size()) {
                    yPosition = outOfOrderY;
                    outOfOrderY += 30;
                }
                newBtn->position = XY{ 0, order.empty() ? submenuOrigin.y : (int)(yPosition) };
                submenuOrigin.y += newBtn->wxHeight;
                newBtn->wxWidth = buttonW;
                newBtn->fill = Fill::Gradient(0xEA121212, 0xEA121212, 0xEA000000, 0xEA000000);
                newBtn->onClickCallback = [this, scancode](UIButton*) {
                    this->doSubmenuAction((SDL_Scancode)scancode);
                    };
                submenuActionsNow.push_back(newBtn);
                submenuPanel->subWidgets.addDrawable(newBtn);
            }
        }
    }
}
