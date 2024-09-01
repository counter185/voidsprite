#pragma once
#include "globals.h"
#include "drawable.h"
#include "mathops.h"

class UITextField : public Drawable
{
public:
	std::string text = "";
	bool isNumericField = false;
	bool isColorField = false;
	int wxWidth = 250, wxHeight = 30;
	SDL_Color bgColor = { 0,0,0, 0xff };
	SDL_Color textColor = { 0xff,0xff,0xff, 0xff };

	bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override {
		return pointInBox(mousePos, SDL_Rect{ thisPositionOnScreen.x, thisPositionOnScreen.y, wxWidth, wxHeight });
	}
	void render(XY pos) override;
	void handleInput(SDL_Event evt, XY gPosOffset) override;
	bool focusableWithTab() override { return true; }

	bool isValidOrPartialColor();
};

