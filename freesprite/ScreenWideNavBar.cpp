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
        std::string fullSectionName = keyBinds[editorSection].name + std::format("({})", SDL_GetScancodeName(editorSection));
        int w = g_fnt->StatStringDimensions(fullSectionName).x + 10 + (keyBinds[editorSection].icon != NULL ? 30 : 0) + 10;
        xDist = ixmax(xDist, w);
    }

    position = XY{ 0,0 };
    for (auto& editorSection : submenuOrder) {
        UIButton* sectionButton = new UIButton();
        sectionButton->position = { x, 1 };
        sectionButton->text = keyBinds[editorSection].name + std::format("({})", SDL_GetScancodeName(editorSection));
        sectionButton->fill = Fill::Gradient(0x70424242, 0x70424242, 0x70000000, 0x70000000);
        sectionButton->colorTextFocused = sectionButton->colorTextUnfocused = SDL_Color{ 255,255,255,0xd0 };
        sectionButton->wxWidth = xDist - 10;
        if (keyBinds[editorSection].icon != NULL) {
            sectionButton->icon = keyBinds[editorSection].icon;
        }
		sectionButton->onClickCallback = [=](UIButton* btn) {
			openSubmenu(editorSection);
		};
        keyBinds[editorSection].button = sectionButton;
        subWidgets.addDrawable(sectionButton);
        x += xDist;
    }
}

void ScreenWideNavBar::render(XY position) {
    wxWidth = g_windowW;

    SDL_Rect r = SDL_Rect{ 0,0,g_windowW, wxHeight };
    SDL_SetRenderDrawColor(g_rd, 0, 0, 0, focused ? 0xa0 : 0x90);
    SDL_RenderFillRect(g_rd, &r);

    if (focused) {
        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xa0);
        drawLine(XY{ 0, wxHeight }, XY{ g_windowW, wxHeight }, XM1PW3P1(focusTimer.percentElapsedTime(600)));
        //SDL_RenderDrawLine(g_rd, 0, wxHeight, g_windowW, wxHeight);
    }
    submenuPanel->position.y = (int)(30 * XM1PW3P1(submenuOpenTimer.percentElapsedTime(200)));
    //subWxs.renderAll(xySubtract(position, { 0,  }));
    subWidgets.renderAll(position);
}

inline void ScreenWideNavBar::handleInput(SDL_Event evt, XY gPosOffset) {

    DrawableManager::processHoverEventInMultiple({ subWidgets }, evt, position);

    //special case here
    if (evt.type == SDL_KEYDOWN) {
        tryPressHotkey(evt.key.scancode);
    }

    DrawableManager::processInputEventInMultiple({ subWidgets }, evt, position);

}

void ScreenWideNavBar::tryPressHotkey(SDL_Scancode k) {
    if (currentSubmenuOpen == -1) {
        if (keyBinds.contains(k)) {
            //openSubmenu(k);
            keyBinds[k].button->click();

        }
    }
    else {
        if (k == SDL_SCANCODE_ESCAPE) {
            openSubmenu((SDL_Scancode)-1);
        }
        else {
            doSubmenuAction(k);
        }
    }
}

void ScreenWideNavBar::openSubmenu(SDL_Scancode which) {
    if (currentSubmenuOpen == which) {
        currentSubmenuOpen = (SDL_Scancode)-1;
        updateCurrentSubmenu();
        return;
    }
    currentSubmenuOpen = (SDL_Scancode)-1;
    submenuOpenTimer.start();
    updateCurrentSubmenu();
    if (which != -1) {
        currentSubmenuOpen = which;
        updateCurrentSubmenu();
    }
}

void ScreenWideNavBar::doSubmenuAction(SDL_Scancode which) {
    if (currentSubmenuOpen != -1 && keyBinds[(SDL_Scancode)currentSubmenuOpen].actions.contains(which)) {
        keyBinds[(SDL_Scancode)currentSubmenuOpen].actions[which].function();
        openSubmenu((SDL_Scancode)-1);
        parentManager->forceUnfocus();
    }
}

void ScreenWideNavBar::updateCurrentSubmenu() {
    if (currentSubmenuOpen == -1) {
        submenuPanel->subWidgets.freeAllDrawables();
        submenuPanel->enabled = false;
    }
    else {
        submenuPanel->enabled = true;
        int y = 0;
        int x = 10 + (std::find(submenuOrder.begin(), submenuOrder.end(), currentSubmenuOpen) - submenuOrder.begin()) * 120;
        submenuPanel->position = { x, 0 };

        for (auto& option : keyBinds[(SDL_Scancode)currentSubmenuOpen].actions) {
            UIButton* newBtn = new UIButton();
            std::vector<SDL_Scancode> order = keyBinds[(SDL_Scancode)currentSubmenuOpen].order;
            newBtn->position = XY{ 0, order.empty() ? y : (int)((std::find(order.begin(), order.end(), option.first) - order.begin()) * newBtn->wxHeight) };
            y += newBtn->wxHeight;
            newBtn->wxWidth = 320;
            newBtn->fill = Fill::Gradient(0xEA121212, 0xEA121212, 0xEA000000, 0xEA000000);
            newBtn->text = option.second.name + std::format(" ({})", SDL_GetScancodeName((SDL_Scancode)option.first));
			newBtn->onClickCallback = [=](UIButton*) {
				doSubmenuAction((SDL_Scancode)option.first);
			};
            submenuPanel->subWidgets.addDrawable(newBtn);
        }
    }
}
