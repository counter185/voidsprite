#pragma once
#include "DraggablePanel.h"
#include "EventCallbackListener.h"
#include "colormodels.h"
#include "UIColorSlider.h"
#include "UIButton.h"
#include "UIHueSlider.h"
#include "UISVPicker.h"

struct ColorModelValue {
    UIColorSlider* valueSlider;
    UILabel* valueLabel;
    double valueNow;
    std::pair<double, double> range;
};
struct ColorModelData {
    ColorModel* targetModel;
    std::map<std::string, ColorModelValue> components;
};

class ColorPickerColorButton : public UIButton {
public:
    UIColorPicker* parent = NULL;
    u32 color = 0;

    ColorPickerColorButton(UIColorPicker* parent, u32 color);
    void click() override;
};


class UIColorPicker : public DraggablePanel, public EventCallbackListener
{
protected:
    //literally only here so that PalettizedEditorColorPicker doesn't initialize all the widgets
    UIColorPicker(int w, int h);
public:
    std::function<void(UIColorPicker*, u32)> onColorChangedCallback = NULL;
	u32 colorNowU32 = 0xFF000000;

    double currentH = 0, currentS = 0, currentV = 0;
    uint8_t currentR = 0, currentG = 0, currentB = 0;

    UIHueSlider* hueSlider = NULL;
    UISVPicker* satValSlider = NULL;
    UITextField* colorTextField = NULL;

    //todo: change these sliders to work based off the color models system
    UITextField* txtR = NULL, *txtG = NULL, *txtB = NULL;
    UITextField* txtH = NULL, *txtS = NULL, *txtV = NULL;

    UIColorSlider* sliderH = NULL;
    UIColorSlider* sliderS = NULL;
    UIColorSlider* sliderV = NULL;
    UIColorSlider* sliderR = NULL;
    UIColorSlider* sliderG = NULL;
    UIColorSlider* sliderB = NULL;

    TabbedView* colorModeTabs = NULL;
    TabbedView* colorTabs = NULL;
    ScrollingPanel* palettePanel = NULL;

    std::vector<std::pair<std::string, ColorModelData>> colorModels;

    UIColorPicker();

    void eventTextInput(int evt_id, std::string data) override;
    void eventTextInputConfirm(int evt_id, std::string data) override;
    void eventSliderPosChanged(int evt_id, float f) override;
    void eventFileOpen(int evt_id, PlatformNativePathString name, int importerIndex = -1) override;

    virtual void updateLastColorButtons() {}
    void reloadColorLists();
    void updateColorModelSliders(std::string dontUpdate = "");
    void updateRGBTextBoxOnInputEvent(std::string data, uint8_t* value);
    void updateHSVTextBoxOnInputEvent(std::string data, double* value);

    void colorUpdatedFromHSVSliders();
    void colorUpdatedFromHSVTextBoxes();
    void colorUpdatedFromRGBSliders();
    void colorUpdatedFromRGBTextBoxes();
    void colorUpdatedHSV(double h, double s, double v);
    void colorUpdated(u32 color);
    virtual void colorUpdated(SDL_Color col, bool updateHSVSliders = true, bool updateRGBSliders = true, bool updateHSVTextBoxes = true, std::string dontUpdateThisColorModel = "");

    void editorColorHSliderChanged(double h) {
        currentH = h;
        colorUpdatedFromHSVSliders();
    }
    void editorColorSVPickerChanged(double s, double v) {
        currentS = s;
        currentV = v;
        colorUpdatedFromHSVSliders();
    }

#if _WIN32
    void openOldWindowsColorPicker();
#endif
};

