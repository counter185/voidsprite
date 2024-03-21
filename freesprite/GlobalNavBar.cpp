#include "GlobalNavBar.h"

void GlobalNavBar::render(XY position)
{
	SDL_Rect r = SDL_Rect{0,0,g_windowW, wxHeight};
	SDL_SetRenderDrawColor(g_rd, 0, 0, 0, focused ? 0xa0 : 0x30);
	SDL_RenderFillRect(g_rd, &r);

	if (focused) {
		SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xa0);
		SDL_RenderDrawLine(g_rd, 0, wxHeight, g_windowW, wxHeight);
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
		wxs.tryFocusOnPoint(XY{ evt.button.x, evt.button.y }, position);
	}
	if (!wxs.anyFocused()) {

	}
	else {
		wxs.passInputToFocused(evt, gPosOffset);
	}
}

void GlobalNavBar::tryPressHotkey(SDL_Keycode k)
{
	if (currentSubmenuOpen == -1) {
		if (keyBinds.contains(k)) {
			openSubmenu(k);
		}
	}
	else {
		if (k == SDLK_ESCAPE) {
			openSubmenu(-1);
		}
		else {
			if (keyBinds[currentSubmenuOpen].actions.contains(k)) {
				keyBinds[currentSubmenuOpen].actions[k].function(parent);
				openSubmenu(-1);
			}
		}
	}
}

void GlobalNavBar::openSubmenu(SDL_Keycode which)
{
	currentSubmenuOpen = -1;
	updateCurrentSubmenu();
	if (which != -1) {
		currentSubmenuOpen = which;
		updateCurrentSubmenu();
	}
}

void GlobalNavBar::updateCurrentSubmenu()
{
	if (currentSubmenuOpen == -1) {
		subWxs.freeAllDrawables();
	}
	else {
		int y = wxHeight;
		int x = 10 + std::distance(keyBinds.begin(), keyBinds.find(currentSubmenuOpen)) * 120;
		for (auto& option : keyBinds[currentSubmenuOpen].actions) {
			UIButton* newBtn = new UIButton();
			newBtn->position = XY{ x, y };
			y += newBtn->wxHeight;
			newBtn->colorBGFocused = newBtn->colorBGUnfocused = SDL_Color{ 0,0,0,0xa0 };
			newBtn->text = option.second.name + std::format(" ({})", SDL_GetKeyName(option.first));
			subWxs.addDrawable(newBtn);
		}
	}
}
