#pragma once
#include "maineditor.h"

class MainEditorPalettized :
    public MainEditor
{
public:

    int32_t pickedPaletteIndex = 0;

    std::vector<uint32_t> palette;
    //std::vector<LayerPalettized*> pltLayers;

    MainEditorPalettized(XY dimensions);
    MainEditorPalettized(LayerPalettized* layer);
    MainEditorPalettized(std::vector<LayerPalettized*> layers);

    void SetPixel(XY position, uint32_t color, uint8_t symmetry = 0) override;
    uint32_t getActiveColor() override;
    void setActiveColor(uint32_t col, bool animate) override;
    void setPalette(std::vector<uint32_t> palette);

    void setUpWidgets() override;
    Layer* flattenImage() override;
};

