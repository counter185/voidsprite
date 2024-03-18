#pragma once
#include "globals.h"
#include "mathops.h"
#include "drawable.h"
class UIButton : public Drawable
{
public:
	std::string text;
	int wxWidth = 250, wxHeight = 30;
	
	SDL_Color colorBGFocused = SDL_Color{ 0,0,0,0xff };
	SDL_Color colorBGUnfocused = SDL_Color{ 0,0,0,0x30 };
	SDL_Color colorTextFocused = SDL_Color{ 255,255,255,0xff };
	SDL_Color colorTextUnfocused = SDL_Color{ 255,255,255,0xd0 };

	bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override {
		return pointInBox(mousePos, SDL_Rect{ thisPositionOnScreen.x, thisPositionOnScreen.y, wxWidth, wxHeight });
	}
	void render(XY pos) override;
	void focusIn() override;
	void handleInput(SDL_Event evt, XY gPosOffset) override;
};

