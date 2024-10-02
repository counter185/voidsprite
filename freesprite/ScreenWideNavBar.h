#pragma once
#include "globals.h"
#include "drawable.h"
#include "DrawableManager.h"
#include "mathops.h"
#include "UIButton.h"
#include "EventCallbackListener.h"
#include "Panel.h"

template <class T>
class ScreenWideNavBar : public Panel, public EventCallbackListener
{
public:
    T parent;
    Panel* submenuPanel;
    SDL_Keycode currentSubmenuOpen = -1;
    Timer64 submenuOpenTimer;

    std::vector<SDL_Keycode> submenuOrder;
    std::map<SDL_Keycode, NavbarSection<T>> keyBinds;

    ScreenWideNavBar(T caller, std::map<SDL_Keycode, NavbarSection<T>> actions, std::vector<SDL_Keycode> order) {
        wxHeight = 30;
        parent = caller;
        submenuOrder = order;
        keyBinds = actions;

        submenuPanel = new Panel();
        submenuPanel->enabled = false;
        subWidgets.addDrawable(submenuPanel);

        int x = 10;
        int xDist = 120;
        position = XY{ 0,0 };
        for (auto& editorSection : submenuOrder) {
            UIButton* sectionButton = new UIButton();
            sectionButton->position = { x, 1 };
            sectionButton->text = keyBinds[editorSection].name + std::format("({})", SDL_GetKeyName(editorSection));
            sectionButton->colorBGFocused = sectionButton->colorBGUnfocused = SDL_Color{ 0,0,0,0 };
            sectionButton->colorTextFocused = sectionButton->colorTextUnfocused = SDL_Color{ 255,255,255,0xd0 };
            sectionButton->wxWidth = xDist - 10;
            if (keyBinds[editorSection].icon != NULL) {
                sectionButton->icon = keyBinds[editorSection].icon;
            }
            sectionButton->setCallbackListener(editorSection, this);
            keyBinds[editorSection].button = sectionButton;
            subWidgets.addDrawable(sectionButton);
            x += xDist;
        }
    }

    void render(XY position) override {
        wxWidth = g_windowW;

        SDL_Rect r = SDL_Rect{ 0,0,g_windowW, wxHeight };
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, focused ? 0xa0 : 0x90);
        SDL_RenderFillRect(g_rd, &r);

        if (focused) {
            SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xa0);
            drawLine(XY{ 0, wxHeight }, XY{ g_windowW, wxHeight }, XM1PW3P1(focusTimer.percentElapsedTime(600)));
            //SDL_RenderDrawLine(g_rd, 0, wxHeight, g_windowW, wxHeight);
        }
        submenuPanel->position.y = (int)( 30 * XM1PW3P1(submenuOpenTimer.percentElapsedTime(200)));
        //subWxs.renderAll(xySubtract(position, { 0,  }));
        subWidgets.renderAll(position);
    }
    void handleInput(SDL_Event evt, XY gPosOffset) override {

        DrawableManager::processHoverEventInMultiple({ subWidgets }, evt, position);

        //special case here
        if (evt.type == SDL_KEYDOWN) {
            tryPressHotkey(evt.key.keysym.sym);
        }

        DrawableManager::processInputEventInMultiple({ subWidgets }, evt, position);

    }

    void focusOut() override {
        Panel::focusOut();
        openSubmenu(-1);
    }
    bool shouldMoveToFrontOnFocus() override { return true; }
    void eventButtonPressed(int evt_id) override {
        if (evt_id < 0) {
            SDL_Keycode subBtnID = -evt_id - 1;
            doSubmenuAction(subBtnID);
        }
        else {
            SDL_Keycode submenuID = evt_id;
            openSubmenu(submenuID);
        }
    }

    void tryPressHotkey(SDL_Keycode k) {
        if (currentSubmenuOpen == -1) {
            if (keyBinds.contains(k)) {
                //openSubmenu(k);
                keyBinds[k].button->click();
            }
        }
        else {
            if (k == SDLK_ESCAPE) {
                openSubmenu(-1);
            }
            else {
                doSubmenuAction(k);
            }
        }
    }
    void openSubmenu(SDL_Keycode which) {
        currentSubmenuOpen = -1;
        submenuOpenTimer.start();
        updateCurrentSubmenu();
        if (which != -1) {
            currentSubmenuOpen = which;
            updateCurrentSubmenu();
        }
    }
    void doSubmenuAction(SDL_Keycode which) {
        if (currentSubmenuOpen != -1 && keyBinds[currentSubmenuOpen].actions.contains(which)) {
            keyBinds[currentSubmenuOpen].actions[which].function(parent);
            openSubmenu(-1);
        }
    }
    void updateCurrentSubmenu() {
        if (currentSubmenuOpen == -1) {
            submenuPanel->subWidgets.freeAllDrawables();
            submenuPanel->enabled = false;
        }
        else {
            submenuPanel->enabled = true;
            int y = 0;
            int x = 10 + (std::find(submenuOrder.begin(), submenuOrder.end(), currentSubmenuOpen) - submenuOrder.begin()) * 120;
            submenuPanel->position = { x, 0 };

            for (auto& option : keyBinds[currentSubmenuOpen].actions) {
                UIButton* newBtn = new UIButton();
                std::vector<SDL_Keycode> order = keyBinds[currentSubmenuOpen].order;
                newBtn->position = XY{ 0, order.empty() ? y : (int)((std::find(order.begin(), order.end(), option.first) - order.begin()) * newBtn->wxHeight) };
                y += newBtn->wxHeight;
                newBtn->wxWidth = 280;
                newBtn->colorBGFocused = newBtn->colorBGUnfocused = SDL_Color{ 0,0,0,0xd0 };
                newBtn->text = option.second.name + std::format(" ({})", SDL_GetKeyName(option.first));
                newBtn->setCallbackListener(-1 - option.first, this);
                submenuPanel->subWidgets.addDrawable(newBtn);
            }
        }
    }
};