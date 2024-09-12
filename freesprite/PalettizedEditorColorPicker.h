#pragma once
#include "EditorColorPicker.h"
#include "MainEditorPalettized.h"
class PalettizedEditorColorPicker :
    public EditorColorPicker
{
public:
	MainEditorPalettized* upcastCaller;
    TabbedView* colorPaletteTabs;

    PalettizedEditorColorPicker(MainEditorPalettized* caller);

    void render(XY position) override;
    void eventButtonPressed(int evt_id) override;
    void eventDropdownItemSelected(int evt_id, int index, std::string name) override;

    void setMainEditorColorRGB(SDL_Color col, bool updateHSVSliders = true, bool updateRGBSliders = true, bool updateHSVTextBoxes = true) override {}
    void updateLastColorButtons() override {}

    void updateForcedColorPaletteButtons();
    void setPickedPaletteIndex(int32_t index);

};

