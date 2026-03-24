#pragma once
#include "PanelUserInteractable.h"
#include "EventCallbackListener.h"
#include "IEditorColorPicker.h"
#include "UIButton.h"

class PaletteButton : public UIButton {
protected:
    std::vector<u32> colors;
public:
    PaletteButton(std::vector<u32> c, int maxNumColors = 32) {
        colors = c;
        if (colors.size() > maxNumColors) {
            colors.resize(maxNumColors);
        }
    }

    void render(XY pos) override;
};

class PalettizedEditorColorPicker :
    public PanelUserInteractable, public IEditorColorPicker, public EventCallbackListener
{
protected:
    void _toggleEraser();
    void updateEraserButton();
    void updatePanelColors();
public:
    MainEditorPalettized* caller;
    TabbedView* colorPaletteTabs;

    UILabel* pickedColorLabel = NULL;

    UIButton* eraserButton = NULL;
    std::vector<UIButton*> colorButtons;

    PalettizedEditorColorPicker(MainEditorPalettized* caller);

    void render(XY at) override {
        updatePanelColors();
        PanelUserInteractable::render(at);
    };
    void renderAfterBG(XY position) override;

    void eventDropdownItemSelected(int evt_id, int index, std::string name) override;
    void eventFileOpen(int evt_id, PlatformNativePathString name, int importerIndex = -1) override;

    void updateForcedColorPaletteButtons();
    void highlightActiveColorButton();
    void setPickedPaletteIndex(int32_t index);

    Panel* getPanel() override { return this; }

    void toggleEraser() override;
    void toggleBlendMode() override {};
    void forceFocusOnColorInputField() override {};

    void setColorRGB(u32 color) override;

    void updateAlphaSlider() override {};

    void pushLastColor(u32 color) override {};
    void clearLastColors() override {};
    void reloadColorLists() override {};

    void actionCtrlScroll(float scrollAmount) override {};
    void actionShiftScroll(float scrollAmount) override {};

};

