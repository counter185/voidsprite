#pragma once
#include "globals.h"

#include "Panel.h"

class ScreenWideNavBar : public Panel
{
public:
    BaseScreen* parent;
    Panel* submenuPanel;
    u32 currentSubmenuOpen = -1;
    Timer64 submenuOpenTimer;

    std::vector<SDL_Scancode> submenuOrder;
    std::map<SDL_Scancode, NavbarSection> keyBinds;

    ScreenWideNavBar(BaseScreen* caller, std::map<SDL_Scancode, NavbarSection> actions, std::vector<SDL_Scancode> order);

    void render(XY position) override;
    void handleInput(SDL_Event evt, XY gPosOffset) override;

    void focusOut() override {
        Panel::focusOut();
        openSubmenu((SDL_Scancode)-1);
    }
    bool shouldMoveToFrontOnFocus() override { return true; }

    void tryPressHotkey(SDL_Scancode k);
    void openSubmenu(SDL_Scancode which);
    void doSubmenuAction(SDL_Scancode which);
    void updateCurrentSubmenu();

    bool takesMouseWheelEvents() override { return false; }
};
