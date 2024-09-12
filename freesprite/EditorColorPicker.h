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
protected:
	UIColorSlider* sliderS;
	UIColorSlider* sliderV;
	UIColorSlider* sliderR;
	UIColorSlider* sliderG;
	UIColorSlider* sliderB;

	UITextField* txtR, * txtG, * txtB;
	UITextField* txtH, * txtS, * txtV;

	TabbedView* colorModeTabs;
	TabbedView* colorTabs;

	UIHueSlider* hueSlider;
	UISVPicker* satValSlider;
	UISlider* sliderH;

	TabbedView* forcedColorPaletteTabs;
public:
	int wxWidth = 400;
	int wxHeight = 390;
	MainEditor* caller;

	DrawableManager subWidgets;

	double currentH = 0, currentS = 0, currentV = 0;

	uint8_t currentR = 0, currentG = 0, currentB = 0;

	UITextField* colorTextField;
	UIButton* eraserButton;
	UIButton* blendModeButton;

	std::vector<uint32_t> lastColors;
	bool lastColorsChanged = true;

	EditorColorPicker(MainEditor* c);

	void initWidgets();

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
	void toggleAlphaBlendMode();
	void updateMainEditorColor();
	void updateMainEditorColorFromRGBSliders();
	void updateMainEditorColorFromRGBTextBoxes();
	void updateMainEditorColorFromHSVTextBoxes();
	void setMainEditorColorHSV(double h, double s, double v);
	void setMainEditorColorRGB(unsigned int col);
	void setMainEditorColorRGB(SDL_Color col, bool updateHSVSliders = true, bool updateRGBSliders = true, bool updateHSVTextBoxes = true);

	void updateRGBTextBoxOnInputEvent(std::string data, uint8_t* value);
	void updateHSVTextBoxOnInputEvent(std::string data, double* value);

	void pushLastColor(uint32_t col);
	void updateLastColorButtons();
	void updateForcedColorPaletteButtons();

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

