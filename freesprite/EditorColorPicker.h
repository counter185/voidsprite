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
#include "DraggablePanel.h"

class EditorColorPicker : public DraggablePanel, public EventCallbackListener
{
protected:
	EditorColorPicker() {}
public:
	MainEditor* caller;

	double currentH = 0, currentS = 0, currentV = 0;

	uint8_t currentR = 0, currentG = 0, currentB = 0;

	UIHueSlider* hueSlider = NULL;
	UISVPicker* satValSlider = NULL;
	UITextField* colorTextField = NULL;
	UISlider* sliderH = NULL;

	UITextField* txtR, *txtG, *txtB;
	UITextField* txtH, *txtS, *txtV;

	UIColorSlider* sliderS = NULL;
	UIColorSlider* sliderV = NULL;
	UIColorSlider* sliderR = NULL;
	UIColorSlider* sliderG = NULL;
	UIColorSlider* sliderB = NULL;

	TabbedView* colorModeTabs = NULL;
	TabbedView* colorTabs = NULL;

	UIButton* eraserButton = NULL;
	UIButton* blendModeButton = NULL;

	std::vector<uint32_t> lastColors;
	bool lastColorsChanged = true;

	EditorColorPicker(MainEditor* c);

	void render(XY position) override;

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
	virtual void setMainEditorColorRGB(SDL_Color col, bool updateHSVSliders = true, bool updateRGBSliders = true, bool updateHSVTextBoxes = true);

	void updateRGBTextBoxOnInputEvent(std::string data, uint8_t* value);
	void updateHSVTextBoxOnInputEvent(std::string data, double* value);

	void pushLastColor(uint32_t col);
	virtual void updateLastColorButtons();

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

