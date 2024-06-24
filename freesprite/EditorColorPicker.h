#pragma once
#include "drawable.h"
#include "DrawableManager.h"
#include "EventCallbackListener.h"
#include "UIHueSlider.h"
#include "UIColorSlider.h"
#include "UISVPicker.h"
#include "UITextField.h"
#include "UIButton.h"
#include "TabbedView.h"
#include "UILabel.h"

class EditorColorPicker : public Drawable, public EventCallbackListener
{
public:
	int wxWidth = 400;
	int wxHeight = 390;
	MainEditor* caller;

	DrawableManager subWidgets;

	double currentH = 0, currentS = 0, currentV = 0;

	uint8_t currentR = 0, currentG = 0, currentB = 0;

	UIHueSlider* hueSlider;
	UISVPicker* satValSlider;
	UITextField* colorTextField;
	UISlider* sliderH;

	UILabel* labelH,*labelS, *labelV, *labelR, *labelG, *labelB;
	UIColorSlider* sliderS;
	UIColorSlider* sliderV;
	UIColorSlider* sliderR;
	UIColorSlider* sliderG;
	UIColorSlider* sliderB;

	TabbedView* colorModeTabs;
	TabbedView* colorTabs;

	UIButton* eraserButton;

	EditorColorPicker(MainEditor* c) {
		caller = c;

		colorModeTabs = new TabbedView({ {"Colors"}, {"Palette"}}, 75);
		colorModeTabs->position = XY{ 20,30 };
		subWidgets.addDrawable(colorModeTabs);

		colorTabs = new TabbedView({{"Visual", g_iconColorVisual}, {"HSV", g_iconColorHSV}, {"RGB", g_iconColorRGB}}, 90);
		colorTabs->position = XY{ 0,5 };
		colorModeTabs->tabs[0].wxs.addDrawable(colorTabs);

		hueSlider = new UIHueSlider(this);
		hueSlider->position = XY{0,10};
		colorTabs->tabs[0].wxs.addDrawable(hueSlider);

		satValSlider = new UISVPicker(this);
		satValSlider->position = XY{ 0,40 };
		colorTabs->tabs[0].wxs.addDrawable(satValSlider);

		sliderH = new UISlider();
		sliderH->position = XY{ 70, 10 };
		sliderH->wxWidth = 200;
		sliderH->setCallbackListener(EVENT_COLORPICKER_SLIDERH, this);
		colorTabs->tabs[1].wxs.addDrawable(sliderH);

		labelH = new UILabel();
		labelR = new UILabel();
		labelH->position = labelR->position = XY{ 0, 20 };
		colorTabs->tabs[1].wxs.addDrawable(labelH);
		colorTabs->tabs[2].wxs.addDrawable(labelR);

		labelS = new UILabel();
		labelG = new UILabel();
		labelS->position = labelG->position = XY{ 0, 70 };
		colorTabs->tabs[1].wxs.addDrawable(labelS);
		colorTabs->tabs[2].wxs.addDrawable(labelG);

		labelV = new UILabel();
		labelB = new UILabel();
		labelV->position = labelB->position = XY{ 0, 120 };
		colorTabs->tabs[1].wxs.addDrawable(labelV);
		colorTabs->tabs[2].wxs.addDrawable(labelB);

		sliderS = new UIColorSlider();
		sliderS->position = XY{ 70, 60 };
		sliderS->wxWidth = 200;
		sliderS->setCallbackListener(EVENT_COLORPICKER_SLIDERS, this);
		colorTabs->tabs[1].wxs.addDrawable(sliderS);		
		
		sliderV = new UIColorSlider();
		sliderV->position = XY{ 70, 110 };
		sliderV->wxWidth = 200;
		sliderV->setCallbackListener(EVENT_COLORPICKER_SLIDERV, this);
		colorTabs->tabs[1].wxs.addDrawable(sliderV);

		sliderR = new UIColorSlider();
		sliderR->position = XY{ 70, 10 };
		sliderR->wxWidth = 200;
		sliderR->setCallbackListener(EVENT_COLORPICKER_SLIDERR, this);
		colorTabs->tabs[2].wxs.addDrawable(sliderR);

		sliderG = new UIColorSlider();
		sliderG->position = XY{ 70, 60 };
		sliderG->wxWidth = 200;
		sliderG->setCallbackListener(EVENT_COLORPICKER_SLIDERG, this);
		colorTabs->tabs[2].wxs.addDrawable(sliderG);

		sliderB = new UIColorSlider();
		sliderB->position = XY{ 70, 110 };
		sliderB->wxWidth = 200;
		sliderB->setCallbackListener(EVENT_COLORPICKER_SLIDERB, this);
		colorTabs->tabs[2].wxs.addDrawable(sliderB);

		colorTextField = new UITextField();
		colorTextField->color = true;
		colorTextField->position = XY{ 60, 350 };
		colorTextField->wxWidth = 140;
		colorTextField->setCallbackListener(EVENT_COLORPICKER_TEXTFIELD, this);
		subWidgets.addDrawable(colorTextField);

		eraserButton = new UIButton();
		eraserButton->position = { 20, 350 };
		//eraserButton->text = "E";
		eraserButton->icon = g_iconEraser;
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
	void eventTextInputConfirm(int evt_id, std::string data) override;
	void eventButtonPressed(int evt_id) override;
	void eventSliderPosChanged(int evt_id, float f) override;

	void toggleEraser();
	void updateMainEditorColor();
	void updateMainEditorColorFromRGBSliders();
	void setMainEditorColorHSV(double h, double s, double v);
	void setMainEditorColorRGB(unsigned int col);
	void setMainEditorColorRGB(SDL_Color col, bool updateHSVSliders = true, bool updateRGBSliders = true);

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

