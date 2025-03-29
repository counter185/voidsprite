#pragma once
#include "globals.h"
#include "drawable.h"
#include "mathops.h"

class UITextField : public Drawable
{
protected:
	std::string text = "";
public:
	std::string tooltip = "";
	bool isNumericField = false;
	bool isColorField = false;
	int insertPosition = 0;
	int wxWidth = 250, wxHeight = 30;
	SDL_Color bgColor = { 0,0,0, 0xff };
	SDL_Color textColor = { 0xff,0xff,0xff, 0xff };

	std::vector<std::string> imeCandidates;
    int imeCandidateIndex = 0;
	Timer64 imeCandidatesTimer;

	void focusIn() override {
		Drawable::focusIn();
		SDL_StartTextInput(g_wd);
	}
	void focusOut() override {
		Drawable::focusOut();
		SDL_StopTextInput(g_wd);
		imeCandidates.clear();
	}

	bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override {
		return pointInBox(mousePos, SDL_Rect{ thisPositionOnScreen.x, thisPositionOnScreen.y, wxWidth, wxHeight });
	}
	void render(XY pos) override;
	void handleInput(SDL_Event evt, XY gPosOffset) override;
	bool focusableWithTab() override { return true; }

	bool textEmpty() { return text.empty(); }
	std::string getText() { return text; }
	void setText(std::string t) 
	{
		text = t;
		insertPosition = text.size();
	}
	void concatToText(std::string t) {
		text += t;
		insertPosition = text.size();
	}
	bool isValidOrPartialColor();
};

