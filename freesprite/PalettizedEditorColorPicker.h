#pragma once
#include "EditorColorPicker.h"
#include "MainEditorPalettized.h"
class PalettizedEditorColorPicker :
    public EditorColorPicker
{
protected:
    PalettizedEditorColorPicker() {}
public:
	MainEditorPalettized* upcastCaller;
    TabbedView* colorPaletteTabs;

    UILabel* pickedColorLabel = NULL;

    PalettizedEditorColorPicker(MainEditorPalettized* caller);

    void render(XY position) override;

    void eventButtonPressed(int evt_id) override;
    void eventButtonRightClicked(int evt_id) override;
    void eventDropdownItemSelected(int evt_id, int index, std::string name) override;
    void eventFileSaved(int evt_id, PlatformNativePathString name, int importerIndex = -1) override;
    void eventFileOpen(int evt_id, PlatformNativePathString name, int importerIndex = -1) override;
    void eventColorSet(int evt_id, uint32_t color) override;

    void colorUpdated(SDL_Color col, bool updateHSVSliders = true, bool updateRGBSliders = true, bool updateHSVTextBoxes = true, std::string dontupdatemodel = "") override {}
    void updateLastColorButtons() override {}

    void updateForcedColorPaletteButtons();
    void setPickedPaletteIndex(int32_t index);

};

