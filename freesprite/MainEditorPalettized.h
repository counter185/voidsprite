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

    void eventFileSaved(int evt_id, PlatformNativePathString name, int exporterId) override;

    void SetPixel(XY position, uint32_t color, bool pushToLastColors = true, uint8_t symmetry = 0) override;
    uint32_t getActiveColor() override;
    void setActiveColor(uint32_t col, bool animate) override;
    uint32_t pickColorFromAllLayers(XY) override;
    void setPalette(std::vector<uint32_t> palette);

    void renderColorPickerAnim() override;

    void setUpWidgets() override;
    void trySaveImage() override;
    bool trySaveWithExporter(PlatformNativePathString name, FileExporter* exporter) override;
    void trySaveAsImage() override;
    Layer* flattenImage() override;
    Layer* newLayer() override;
    Layer* mergeLayers(Layer* bottom, Layer* top) override;
    void exportTilesIndividually() override;

    int32_t* makeFlatIndicesTable();
    Layer* flattenImageAndConvertToRGB();
    Layer* flattenImageWithoutConvertingToRGB();

    void tryExportRGB();
    void trySavePalettizedImage();
    void trySaveAsPalettizedImage();
    MainEditor* toRGBSession();
    void openInNormalRGBEditor();
};

