#pragma once
#include "drawable.h"
#include "mathops.h"
class UISVPicker : public Drawable
{
public:
	float sPos = 0.0f;
	float vPos = 1.0f;

	int wxWidth = 360, wxHeight = 200;
	bool mouseHeld = false;

	EditorColorPicker* parent;

	UISVPicker(EditorColorPicker* parent) {
		this->parent = parent;
	}

	void drawPosIndicator(XY origin);

	bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override {
		return pointInBox(mousePos, SDL_Rect{ thisPositionOnScreen.x, thisPositionOnScreen.y, wxWidth, wxHeight });
	}
	void render(XY pos) override;
	void handleInput(SDL_Event evt, XY gPosOffset) override;
	void focusOut() override {
		Drawable::focusOut();
		mouseHeld = false;
	}

	void onSVValueChanged();
};

