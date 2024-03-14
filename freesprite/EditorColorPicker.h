#pragma once
#include "drawable.h"
#include "DrawableManager.h"
#include "UIColorSlider.h"
#include "UISVPicker.h"

class EditorColorPicker : public Drawable
{
public:
	int wxWidth = 400;
	int wxHeight = 380;
	MainEditor* caller;

	DrawableManager subWidgets;

	double currentH = 0, currentS = 0, currentV = 0;

	EditorColorPicker(MainEditor* c) {
		caller = c;
		UIColorSlider* colorSlider = new UIColorSlider(this);
		colorSlider->position = XY{20,30};
		UISVPicker* svPicker = new UISVPicker(this);
		svPicker->position = XY{ 20,100 };
		subWidgets.addDrawable(colorSlider);
		subWidgets.addDrawable(svPicker);
	}

	bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override;
	void render(XY position) override;
	void handleInput(SDL_Event evt, XY gPosOffset) override;
	void focusOut() override {
		Drawable::focusOut();
		subWidgets.forceUnfocus();
	}

	void updateMainEditorColor();

	void editorColorHSliderChanged(double h) {
		currentH = h;
		updateMainEditorColor();
	}
	void editorColorSVPickerChanged(double s, double v) {
		currentS = s;
		currentV = v;
		updateMainEditorColor();
	}
};

