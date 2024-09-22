#pragma once
#include "globals.h"
#include "drawable.h"
#include "DrawableManager.h"
#include "mathops.h"
#include "UIButton.h"
#include "EventCallbackListener.h"

template <class T>
class ScreenWideNavBar : public Drawable, public EventCallbackListener
{
public:
	T parent;
	int wxHeight = 30;
	DrawableManager wxs;
	DrawableManager subWxs;
	SDL_Keycode currentSubmenuOpen = -1;
	Timer64 submenuOpenTimer;

	std::vector<SDL_Keycode> submenuOrder;
	std::map<SDL_Keycode, NavbarSection<T>> keyBinds;

	ScreenWideNavBar(T caller, std::map<SDL_Keycode, NavbarSection<T>> actions, std::vector<SDL_Keycode> order) {
		parent = caller;
		submenuOrder = order;
		keyBinds = actions;

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
			wxs.addDrawable(sectionButton);
			x += xDist;
		}
	}

	bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override
	{ 
		return mousePos.y <= wxHeight || subWxs.mouseInAny(thisPositionOnScreen, mousePos);
	}
	void render(XY position) override {
		SDL_Rect r = SDL_Rect{ 0,0,g_windowW, wxHeight };
		SDL_SetRenderDrawColor(g_rd, 0, 0, 0, focused ? 0xa0 : 0x90);
		SDL_RenderFillRect(g_rd, &r);

		if (focused) {
			SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xa0);
			drawLine(XY{ 0, wxHeight }, XY{ g_windowW, wxHeight }, XM1PW3P1(focusTimer.percentElapsedTime(600)));
			//SDL_RenderDrawLine(g_rd, 0, wxHeight, g_windowW, wxHeight);
		}
		subWxs.renderAll(xySubtract(position, { 0, (int)(30 * (1.0f - XM1PW3P1(submenuOpenTimer.percentElapsedTime(200)))) }));
		wxs.renderAll(position);
	}
	void handleInput(SDL_Event evt, XY gPosOffset) override {

		DrawableManager::processHoverEventInMultiple({ wxs, subWxs }, evt, position);

		//special case here
		if (evt.type == SDL_KEYDOWN) {
			tryPressHotkey(evt.key.keysym.sym);
		}

		std::vector<std::reference_wrapper<DrawableManager>> inputTargets = {wxs};
		if (currentSubmenuOpen != -1) {
			inputTargets.push_back(subWxs);
		}
		DrawableManager::processInputEventInMultiple(inputTargets, evt, position);

		/*if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == 1 && evt.button.state) {
			if (!wxs.tryFocusOnPoint(XY{ evt.button.x, evt.button.y }, position) && currentSubmenuOpen != -1) {
				subWxs.tryFocusOnPoint(XY{ evt.button.x, evt.button.y }, position);
			}
		}
		if (wxs.anyFocused()) {
			wxs.passInputToFocused(evt, gPosOffset);
		}
		else if (subWxs.anyFocused()) {
			subWxs.passInputToFocused(evt, gPosOffset);
		}*/
	}

	void focusOut() override {
		Drawable::focusOut();
		openSubmenu(-1);
	}
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
		subWxs.forceUnfocus();
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
			subWxs.freeAllDrawables();
		}
		else {
			int y = wxHeight;
			int x = 10 + (std::find(submenuOrder.begin(), submenuOrder.end(), currentSubmenuOpen) - submenuOrder.begin()) * 120;

			for (auto& option : keyBinds[currentSubmenuOpen].actions) {
				UIButton* newBtn = new UIButton();
				std::vector<SDL_Keycode> order = keyBinds[currentSubmenuOpen].order;
				newBtn->position = XY{ x, order.empty() ? y : (int)(wxHeight + (std::find(order.begin(), order.end(), option.first) - order.begin()) * newBtn->wxHeight) };
				y += newBtn->wxHeight;
				newBtn->wxWidth = 280;
				newBtn->colorBGFocused = newBtn->colorBGUnfocused = SDL_Color{ 0,0,0,0xd0 };
				newBtn->text = option.second.name + std::format(" ({})", SDL_GetKeyName(option.first));
				newBtn->setCallbackListener(-1 - option.first, this);
				subWxs.addDrawable(newBtn);
			}
		}
	}
};