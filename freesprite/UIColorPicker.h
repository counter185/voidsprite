#pragma once
#include "DraggablePanel.h"
#include "EventCallbackListener.h"
#include "colormodels.h"
#include "UIColorSlider.h"
#include "UIButton.h"
#include "UIHueSlider.h"
#include "UISVPicker.h"

enum ColorChangeSource {
    COLORCHANGE_EXTERNAL = 0,
    VISUAL_HSV = 1,
    SLIDERS_RGB_SLIDER = 2,
    SLIDERS_RGB_TBOX = 3,
    SLIDERS_HSV_SLIDER = 4,
    SLIDERS_HSV_TBOX = 5,
    COLORMODELS_SLIDER = 6,
    MIX_SLIDER = 7
};

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
};


class UIColorPicker : public DraggablePanel, public EventCallbackListener
{
protected:
    bool canEditPalettes = true;

    //literally only here so that PalettizedEditorColorPicker doesn't initialize all the widgets
    UIColorPicker(int w, int h);

    virtual void colorUpdatedRGB(SDL_Color col, ColorChangeSource from = COLORCHANGE_EXTERNAL, std::string dontUpdateThisColorModel = "");
    virtual void colorUpdatedHSV(hsv col, ColorChangeSource from = COLORCHANGE_EXTERNAL, std::string dontUpdateThisColorModel = "");
public:
    std::function<void(UIColorPicker*, u32)> onColorChangedCallback = NULL;
    u32 colorNowU32 = 0xFF000000;

    double currentH = 0, currentS = 0, currentV = 0;
    int currentR = 0, currentG = 0, currentB = 0;

    UIHueWheel* hueWheel = NULL;
    UIHueSlider* hueSlider = NULL;
    UISVPicker* satValSlider = NULL;
    UITextField* colorTextField = NULL;

    //todo: change these sliders to work based off the color models system
    UINumberInputField* txtR = NULL, *txtG = NULL, *txtB = NULL;
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

    UIColorInputField* mix1Input = NULL;
    UIColorInputField* mix2Input = NULL;
    UIColorSlider* mixSlider = NULL;

    std::vector<std::pair<std::string, ColorModelData>> colorModels;

    UIColorPicker();

    void eventTextInput(int evt_id, std::string data) override;
    void eventTextInputConfirm(int evt_id, std::string data) override;
    void eventSliderPosChanged(int evt_id, float f) override;
    void eventFileOpen(int evt_id, PlatformNativePathString name, int importerIndex = -1) override;

    virtual void updateLastColorButtons() {}
    void reloadColorLists();
    void addColorToPalette(NamedColorPalette p, u32 color);
    void updateColorModelSliders(std::string dontUpdate = "");
    void updateHSVTextBoxOnInputEvent(std::string data, double* value);

    void colorUpdatedFromVisualHSV();
    void colorUpdatedFromHSVSliders();
    void colorUpdatedFromHSVTextBoxes();
    void colorUpdatedFromRGBSliders();
    void colorUpdatedFromRGBTextBoxes();
    void colorUpdatedFromMixInput();
    void updateUIFrom(ColorChangeSource from, std::string dontUpdateThisColorModel);

    void setColorRGB(u32 color);
    void setColorHSV(double h, double s, double v);

    void updateSliderTabRGBTextboxes();

    void updateSliderTabHSVTextboxes();

    void updateSliderTabRGBSliders();

    void updateVisualTabHSVPicker();

    void updateSliderTabHSVSliders();

    void updateAllSliderColors();

    void editorColorHSliderChanged(double h) {
        currentH = h;
        colorUpdatedFromVisualHSV();
    }
    void editorColorSVPickerChanged(double s, double v) {
        currentS = s;
        currentV = v;
        colorUpdatedFromVisualHSV();
    }

#if _WIN32
    void openOldWindowsColorPicker();
#endif
};

