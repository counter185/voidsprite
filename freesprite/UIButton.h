#pragma once
#include "globals.h"
#include "mathops.h"
#include "drawable.h"
class UIButton : public Drawable
{
public:
	std::string text;
	int wxWidth = 250, wxHeight = 30;

	bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override {
		return pointInBox(mousePos, SDL_Rect{ thisPositionOnScreen.x, thisPositionOnScreen.y, wxWidth, wxHeight });
	}
	void render(XY pos) override;
	void focusIn() override;
};

