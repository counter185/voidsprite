#pragma once
#include "globals.h"

#include "Panel.h"

struct NavbarSection {
    std::string name;
    std::vector<SDL_Scancode> order;
    std::map<SDL_Scancode, NamedOperation> actions;
    HotReloadableTexture* icon = NULL;
    UIButton* button = NULL;
};

inline NavbarSection makeNavbarSection(std::string name, HotReloadableTexture* icon, std::vector<std::pair<SDL_Scancode, NamedOperation>> actions) {
    NavbarSection sec{};
    sec.name = name;
    sec.icon = icon;
    for (auto& [k, op] : actions) {
        sec.actions[k] = op;
        sec.order.push_back(k);
    }
    return sec;
}

inline NavbarSection makeNavbarSection(std::string name, std::vector<std::pair<SDL_Scancode, NamedOperation>> actions) {
    return makeNavbarSection(name, NULL, actions);
}

class ScreenWideNavBar : public Panel
{
private:
    std::vector<UIButton*> submenuActionsNow;
protected:
    const SDL_Scancode SCANCODE_NONE = (SDL_Scancode)-1;
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
        openSubmenu(SCANCODE_NONE);
    }
    bool shouldMoveToFrontOnFocus() override { return true; }

    void tryPressHotkey(SDL_Scancode k);
    void openSubmenu(SDL_Scancode which);
    void doSubmenuAction(SDL_Scancode which);
    void updateCurrentSubmenu();

    bool takesMouseWheelEvents() override { return false; }
};
