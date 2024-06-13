#include "GlobalNavBar.h"

void GlobalNavBar::render(XY position)
{
	SDL_Rect r = SDL_Rect{0,0,g_windowW, wxHeight};
	SDL_SetRenderDrawColor(g_rd, 0, 0, 0, focused ? 0xa0 : 0x30);
	SDL_RenderFillRect(g_rd, &r);

	if (focused) {
		SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xa0);
		drawLine(XY{ 0, wxHeight }, XY{ g_windowW, wxHeight }, XM1PW3P1(focusTimer.percentElapsedTime(600)));
		//SDL_RenderDrawLine(g_rd, 0, wxHeight, g_windowW, wxHeight);
	}
	wxs.renderAll(position);
	subWxs.renderAll(position);
}

void GlobalNavBar::handleInput(SDL_Event evt, XY gPosOffset)
{
	//special case here
	if (evt.type == SDL_KEYDOWN) {
		tryPressHotkey(evt.key.keysym.sym);
	}

	if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == 1 && evt.button.state) {
		if (!wxs.tryFocusOnPoint(XY{ evt.button.x, evt.button.y }, position) && currentSubmenuOpen != -1) {
			subWxs.tryFocusOnPoint(XY{ evt.button.x, evt.button.y }, position);
		}
	}
	if (wxs.anyFocused()) {
		wxs.passInputToFocused(evt, gPosOffset);
	}
	else if (subWxs.anyFocused()) {
		subWxs.passInputToFocused(evt, gPosOffset);
	}
}

void GlobalNavBar::eventButtonPressed(int evt_id)
{
	if (evt_id < 0) {
		SDL_Keycode subBtnID = -evt_id - 1;
		doSubmenuAction(subBtnID);
	}
	else {
		SDL_Keycode submenuID = evt_id;
		openSubmenu(submenuID);
	}
}

void GlobalNavBar::tryPressHotkey(SDL_Keycode k)
{
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

void GlobalNavBar::openSubmenu(SDL_Keycode which)
{
	subWxs.forceUnfocus();
	currentSubmenuOpen = -1;
	updateCurrentSubmenu();
	if (which != -1) {
		currentSubmenuOpen = which;
		updateCurrentSubmenu();
	}
}

void GlobalNavBar::doSubmenuAction(SDL_Keycode which)
{
	if (currentSubmenuOpen != -1 && keyBinds[currentSubmenuOpen].actions.contains(which)) {
		keyBinds[currentSubmenuOpen].actions[which].function(parent);
		openSubmenu(-1);
	}
}

void GlobalNavBar::updateCurrentSubmenu()
{
	if (currentSubmenuOpen == -1) {
		subWxs.freeAllDrawables();
	}
	else {
		int y = wxHeight;
		int x = 10 + (std::find(submenuOrder.begin(), submenuOrder.end(), currentSubmenuOpen) - submenuOrder.begin()) * 120;

		for (auto& option : keyBinds[currentSubmenuOpen].actions) {
			UIButton* newBtn = new UIButton();
			std::vector<SDL_Keycode> order = keyBinds[currentSubmenuOpen].order;
			newBtn->position = XY{ x, order.empty() ? y : (int)(wxHeight + (std::find(order.begin(), order.end(), option.first) - order.begin()) * newBtn->wxHeight)};
			y += newBtn->wxHeight;
			newBtn->wxWidth = 280;
			newBtn->colorBGFocused = newBtn->colorBGUnfocused = SDL_Color{ 0,0,0,0xd0 };
			newBtn->text = option.second.name + std::format(" ({})", SDL_GetKeyName(option.first));
			newBtn->setCallbackListener(-1 - option.first, this);
			subWxs.addDrawable(newBtn);
		}
	}
}
