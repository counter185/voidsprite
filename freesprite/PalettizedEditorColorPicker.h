#pragma once
#include "EditorColorPicker.h"
#include "MainEditorPalettized.h"
class PalettizedEditorColorPicker :
    public EditorColorPicker
{
protected:
    PalettizedEditorColorPicker() {}

    void colorUpdatedRGB(SDL_Color col, ColorChangeSource from = COLORCHANGE_EXTERNAL, std::string dontupdatemodel = "") override {}
    void colorUpdatedHSV(hsv col, ColorChangeSource from = COLORCHANGE_EXTERNAL, std::string dontupdatemodel = "") override {}
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

    void updateLastColorButtons() override {}

    void updateForcedColorPaletteButtons();
    void setPickedPaletteIndex(int32_t index);

};

