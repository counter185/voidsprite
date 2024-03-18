#pragma once
#include "drawable.h"
#include "DrawableManager.h"
#include "EventCallbackListener.h"
#include "UIColorSlider.h"
#include "UISVPicker.h"
#include "UITextField.h"
#include "UIButton.h"

class EditorColorPicker : public Drawable, public EventCallbackListener
{
public:
	int wxWidth = 400;
	int wxHeight = 380;
	MainEditor* caller;

	DrawableManager subWidgets;

	double currentH = 0, currentS = 0, currentV = 0;

	UIColorSlider* hueSlider;
	UISVPicker* satValSlider;
	UITextField* colorTextField;

	UIButton* eraserButton;

	EditorColorPicker(MainEditor* c) {
		caller = c;
		hueSlider = new UIColorSlider(this);
		hueSlider->position = XY{20,30};
		subWidgets.addDrawable(hueSlider);

		satValSlider = new UISVPicker(this);
		satValSlider->position = XY{ 20,100 };
		subWidgets.addDrawable(satValSlider);

		colorTextField = new UITextField();
		colorTextField->position = XY{ 20, 305 };
		colorTextField->wxWidth = 140;
		colorTextField->setCallbackListener(EVENT_COLORPICKER_TEXTFIELD, this);
		subWidgets.addDrawable(colorTextField);

		eraserButton = new UIButton();
		eraserButton->position = { 20, 340 };
		eraserButton->text = "E";
		eraserButton->wxWidth = 30;
		eraserButton->setCallbackListener(EVENT_COLORPICKER_TOGGLEERASER, this);
		subWidgets.addDrawable(eraserButton);
	}

	bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override;
	void render(XY position) override;
	void handleInput(SDL_Event evt, XY gPosOffset) override;
	void focusOut() override {
		Drawable::focusOut();
		subWidgets.forceUnfocus();
	}
	void eventTextInput(int evt_id, std::string data) override;
	void eventButtonPressed(int evt_id) override;

	void updateMainEditorColor();
	void setMainEditorColorRGB(unsigned int col);
	void setMainEditorColorRGB(SDL_Color col, bool updateHSVSliders = true);

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

