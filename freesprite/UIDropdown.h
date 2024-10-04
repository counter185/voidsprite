#pragma once
#include "drawable.h"
#include "DrawableManager.h"
#include "EventCallbackListener.h"

class UIDropdown :
    public Drawable, EventCallbackListener
{
public:
	std::string text;
	int wxWidth = 250, wxHeight = 30;
	SDL_Texture* icon = NULL;

	SDL_Color colorBGFocused = SDL_Color{ 0,0,0,0xff };
	SDL_Color colorBGUnfocused = SDL_Color{ 0,0,0,0xd0 };
	SDL_Color colorTextFocused = SDL_Color{ 255,255,255,0xff };
	SDL_Color colorTextUnfocused = SDL_Color{ 255,255,255,0xd0 };

	Timer64 lastClick;
	
	bool isOpen = false;
	bool setTextToSelectedItem = false;
	Timer64 openTimer;
	int menuYOffset = 0;
	int menuHeight = 0;

	DrawableManager wxs;

	std::vector<std::string> items;
	std::vector<std::string> tooltips;

	UIDropdown(std::vector<std::string> items);
	UIDropdown(std::vector<std::pair<std::string, std::string>> items);

	bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override {
		return pointInBox(mousePos, SDL_Rect{ thisPositionOnScreen.x, thisPositionOnScreen.y, wxWidth, wxHeight }) || (isOpen && wxs.mouseInAny(xyAdd({0, menuYOffset}, thisPositionOnScreen), mousePos));
	}
	void render(XY pos) override;
	void focusIn() override;
	void focusOut() override;
	void mouseHoverOut() override;
	void mouseHoverMotion(XY mousePos, XY gPosOffset) override;
	void handleInput(SDL_Event evt, XY gPosOffset) override;
	bool shouldMoveToFrontOnFocus() override { return true; }

	void eventButtonPressed(int evt_id) override;

	void genButtons(UIButton* (*customButtonGenFunction)(std::string name, std::string item) = NULL);

	void click();
};

