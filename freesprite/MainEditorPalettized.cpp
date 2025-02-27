#include "MainEditorPalettized.h"
#include "LayerPalettized.h"
#include "Pattern.h"
#include "Notification.h"
#include "TilemapPreviewScreen.h"
#include "PalettizedEditorColorPicker.h"
#include "PalettizedEditorLayerPicker.h"
#include "EditorBrushPicker.h"
#include "RPG2KTilemapPreviewScreen.h"
#include "MinecraftBlockPreviewScreen.h"
#include "FileIO.h"

#include "PopupIntegerScale.h"
#include "PopupMessageBox.h"
#include "PopupTileGeneric.h"
#include "PopupSetEditorPixelGrid.h"
#include "PopupGlobalConfig.h"

MainEditorPalettized::MainEditorPalettized(XY dimensions)
{
    isPalettized = true;
    canvas.dimensions = dimensions;
    palette = g_palettes[PALETTE_DEFAULT];
    LayerPalettized* pltLayer = new LayerPalettized(dimensions.x, dimensions.y);
    pltLayer->palette = palette;
    layers.push_back(pltLayer);

    setUpWidgets();
    recenterCanvas();
    initLayers();
}

MainEditorPalettized::MainEditorPalettized(LayerPalettized* layer)
{
    isPalettized = true;
    canvas.dimensions = { layer->w, layer->h };

    layers.push_back(layer);
    palette = layer->palette;

    setUpWidgets();
    recenterCanvas();
    initLayers();
}

MainEditorPalettized::MainEditorPalettized(std::vector<LayerPalettized*> layers)
{
    //all layers must have the same palette assigned!!!
    isPalettized = true;
    canvas.dimensions = { layers[0]->w, layers[0]->h };
    for (auto& l : layers) {
        this->layers.push_back(l);
    }
    palette = layers[0]->palette;

    setUpWidgets();
    recenterCanvas();
    initLayers();
}

void MainEditorPalettized::eventFileSaved(int evt_id, PlatformNativePathString name, int exporterId)
{
    if (evt_id == EVENT_PALETTIZEDEDITOR_SAVEFILE) {

        exporterId--;

        bool result = false;

        if (exporterId < g_palettizedFileExporters.size()) {
            FileExporter* actualExporterID = g_palettizedFileExporters[exporterId];
            if (trySaveWithExporter(name, actualExporterID)) {
                g_tryPushLastFilePath(convertStringToUTF8OnWin32(name));
            }
        }
    }
    else if (evt_id == EVENT_PALETTIZEDEDITOR_EXPORTRGBFILE) {
        exporterId--;

        FileExporter* exporter = NULL;
        if (exporterId < g_fileExporters.size()) {
            bool result = false;
            exporter = g_fileExporters[exporterId];

            if (exporter->exportsWholeSession()) {
                MainEditor* rgbConvEditor = toRGBSession();
                result = exporter->exportData(name, rgbConvEditor);
                delete rgbConvEditor;
            }
            else {
                Layer* l = flattenImageAndConvertToRGB();
                result = exporter->exportData(name, l);
                delete l;
            }

            if (result) {
                if (lastWasSaveAs && g_config.openSavedPath) {
                    platformOpenFileLocation(lastConfirmedSavePath);
                }
                g_addNotification(SuccessNotification("Success", "File exported successfully."));
            }
            else {
                g_addNotification(ErrorNotification("Error", "Failed to exported file."));
            }
        }
        else {
            g_addNotification(ErrorNotification("Error", "Invalid exporter"));
        }
    }
    else if (evt_id == EVENT_MAINEDITOR_EXPORTTILES) {
        exporterId--;

        FileExporter* exporter = g_palettizedFileExporters[exporterId];
        XY tileCounts = { canvas.dimensions.x / tileDimensions.x, canvas.dimensions.y / tileDimensions.y };
        PlatformNativePathString pathOfFile = name.substr(0, name.find_last_of(convertStringOnWin32("/\\")));

        Layer* flatImage = flattenImageWithoutConvertingToRGB();
        for (int y = 0; y < tileCounts.y; y++) {
            for (int x = 0; x < tileCounts.x; x++) {
                SDL_Rect clipRect = { x * tileDimensions.x, y * tileDimensions.y, tileDimensions.x, tileDimensions.y };
                Layer* clip = flatImage->trim(clipRect);
                if (clip != NULL) {
                    PlatformNativePathString tileName = name.substr(0, name.find_last_of(convertStringOnWin32("."))) + convertStringOnWin32(std::format("_{}_{}{}", x, y, exporter->extension()));
                    if (!exporter->exportsWholeSession()) {
                        exporter->exportData(tileName, clip);
                        delete clip;
                    }
                    else {
                        MainEditorPalettized* session = new MainEditorPalettized((LayerPalettized*)clip);
                        session->trySaveWithExporter(tileName, exporter);
                        delete session;
                    }
                }
                else {
                    g_addNotification(ErrorNotification("Error", std::format("Failed to export tile {}:{}", x, y)));
                }
            }
        }
        delete flatImage;
        g_addNotification(SuccessNotification("Success", "Tiles exported"));
    }
}

void MainEditorPalettized::SetPixel(XY position, uint32_t color, bool pushToLastColors, uint8_t symmetry)
{
    if (currentPattern->canDrawAt(position) && (!replaceAlphaMode || (replaceAlphaMode && layer_getPixelAt(position) != -1))) {
        if (!isolateEnabled || (isolateEnabled && isolatedFragment.pointExists(position))) {
            int32_t targetColor = (int32_t)color;
            /*if (blendAlphaMode) {
                if (eraserMode) {
                    targetColor = ((0xff - (targetColor >> 24)) << 24) + (targetColor & 0xffffff);
                }
                targetColor = alphaBlend(getCurrentLayer()->getPixelAt(position), targetColor);
            }*/
            if (targetColor < -1 || targetColor >= palette.size()) {
                targetColor = -1;
            }
            getCurrentLayer()->setPixel(position, eraserMode ? -1 : targetColor);
        }
        //colorPicker->pushLastColor(color);
    }
    if (symmetryEnabled[0] && !(symmetry & 0b10)) {
        int symmetryXPoint = symmetryPositions.x / 2;
        bool symXPointIsCentered = symmetryPositions.x % 2;
        int symmetryFlippedX = symmetryXPoint + (symmetryXPoint - position.x) - (symXPointIsCentered ? 0 : 1);
        SetPixel(XY{ symmetryFlippedX, position.y }, color, symmetry | 0b10);
    }
    if (symmetryEnabled[1] && !(symmetry & 0b1)) {
        int symmetryYPoint = symmetryPositions.y / 2;
        bool symYPointIsCentered = symmetryPositions.y % 2;
        int symmetryFlippedY = symmetryYPoint + (symmetryYPoint - position.y) - (symYPointIsCentered ? 0 : 1);
        SetPixel(XY{ position.x, symmetryFlippedY }, color, symmetry | 0b1);
    }
}

uint32_t MainEditorPalettized::getActiveColor()
{
    return pickedPaletteIndex;
}

void MainEditorPalettized::setActiveColor(uint32_t col, bool animate)
{
    if (col == -1) {
        col = 0;
    }
    ((PalettizedEditorColorPicker*)colorPicker)->setPickedPaletteIndex(col);
    if (animate) {
        colorPickTimer.start();
    }
}

uint32_t MainEditorPalettized::pickColorFromAllLayers(XY pos)
{
    for (int x = layers.size() - 1; x >= 0; x--) {
        if (layers[x]->hidden) {
            continue;
        }
        uint32_t nextC = layers[x]->getPixelAt(pos, false);
        if (nextC != -1) {
            return nextC;
        }
    }
    return 0;
}

void MainEditorPalettized::setPalette(std::vector<uint32_t> newPalette)
{
    if (palette.size() > newPalette.size()) {
        for (int x = 0; x < newPalette.size(); x++) {
            palette[x] = newPalette[x];
        }
    }
    else {
        palette = newPalette;
    }

    for (Layer*& l : layers) {
        ((LayerPalettized*)l)->palette = palette;
        ((LayerPalettized*)l)->layerDirty = true;
    }

    ((PalettizedEditorColorPicker*)colorPicker)->updateForcedColorPaletteButtons();
}

void MainEditorPalettized::renderColorPickerAnim()
{
    if (colorPickTimer.started && colorPickTimer.percentElapsedTime(500) <= 1.0f) {
        float progress = colorPickTimer.percentElapsedTime(500);
        int pxDistance = 40 * (lastColorPickWasFromWholeImage ? XM1PW3P1(progress) : (1.0 - XM1PW3P1(progress)));

        SDL_Rect colRect2 = { g_mouseX, g_mouseY, 1,1 };
        int pxDistance2 = pxDistance + 1;
        colRect2.x -= pxDistance2;
        colRect2.w = pxDistance2 * 2;
        colRect2.y -= pxDistance2;
        colRect2.h = pxDistance2 * 2;

        //SDL_SetRenderDrawColor(g_rd, 255,255,255, (uint8_t)(127 * XM1PW3P1(1.0f - progress)));
        //SDL_RenderDrawRect(g_rd, &colRect2);

        for (int x = 3; x >= 1; x--) {
            SDL_Rect colRect = { g_mouseX, g_mouseY, 1,1 };
            int nowPxDistance = pxDistance / x;
            colRect.x -= nowPxDistance;
            colRect.w = nowPxDistance * 2;
            colRect.y -= nowPxDistance;
            colRect.h = nowPxDistance * 2;
            uint32_t effectColor = (pickedPaletteIndex < palette.size() && pickedPaletteIndex >= 0) ? palette[pickedPaletteIndex] : 0;
            hsv thsv = rgb2hsv(rgb{ ((effectColor >> 16) & 0xff) / 255.0f, ((effectColor >> 8) & 0xff) / 255.0f, (effectColor & 0xff) / 255.0f });
            thsv.s /= 3;
            thsv.v += 0.4;
            thsv.v = dxmin(1.0, thsv.v);
            rgb trgb = hsv2rgb(thsv);
            SDL_Color trgbColor = SDL_Color{ (uint8_t)(trgb.r * 255.0), (uint8_t)(trgb.g * 255.0), (uint8_t)(trgb.b * 255.0), 255 };
            SDL_SetRenderDrawColor(g_rd, trgbColor.r, trgbColor.g, trgbColor.b, (uint8_t)(255 * XM1PW3P1(1.0f - progress)));
            SDL_RenderDrawRect(g_rd, &colRect);
        }
    }
}

void MainEditorPalettized::setUpWidgets()
{
    mainEditorKeyActions = {
        {
            SDLK_f,
            {
                "File",
                {SDLK_s, SDLK_d, SDLK_e, SDLK_a, SDLK_r, SDLK_p, SDLK_c},
                {
                    {SDLK_d, { "Save as",
                            [](MainEditor* editor) {
                                editor->trySaveAsImage();
                            }
                        }
                    },
                    {SDLK_s, { "Save",
                            [](MainEditor* editor) {
                                editor->trySaveImage();
                            }
                        }
                    },
                    {SDLK_e, { "Export as RGB",
                            [](MainEditor* editor) {
                                ((MainEditorPalettized*)editor)->tryExportRGB();
                            }
                        }
                    },
                    {SDLK_a, { "Export tiles individually",
                            [](MainEditor* editor) {
                                editor->exportTilesIndividually();
                            }
                        }
                    },
                    {SDLK_c, { "Close",
                            [](MainEditor* editor) {
                                editor->requestSafeClose();
                            }
                        }
                    },
                    {SDLK_r, { "Open in RGB editor",
                            [](MainEditor* editor) {
                                ((MainEditorPalettized*)editor)->openInNormalRGBEditor();
                            }
                        }
                    },
                    {SDLK_p, { "Preferences",
                            [](MainEditor* screen) {
                                g_addPopup(new PopupGlobalConfig());
                            }
                        }
                    }
                },
                g_iconNavbarTabFile
            }
        },
        {
            SDLK_e,
            {
                "Edit",
                {SDLK_z, SDLK_r, SDLK_x, SDLK_y, SDLK_s, SDLK_c, SDLK_v, SDLK_b, SDLK_n},
                {
                    {SDLK_z, { "Undo",
                            [](MainEditor* editor) {
                                editor->undo();
                            }
                        }
                    },
                    {SDLK_r, { "Redo",
                            [](MainEditor* editor) {
                                editor->redo();
                            }
                        }
                    },
                    {SDLK_x, { "Toggle symmetry: X",
                            [](MainEditor* editor) {
                                editor->symmetryEnabled[0] = !editor->symmetryEnabled[0];
                            }
                        }
                    },
                    {SDLK_y, { "Toggle symmetry: Y",
                            [](MainEditor* editor) {
                                editor->symmetryEnabled[1] = !editor->symmetryEnabled[1];
                            }
                        }
                    },
                    {SDLK_s, { "Deselect",
                            [](MainEditor* editor) {
                                editor->isolateEnabled = false;
                            }
                        }
                    },
                    {SDLK_c, { "Resize canvas",
                            [](MainEditor* editor) {
                                g_addPopup(new PopupTileGeneric(editor, "Resize canvas", "New canvas size:", editor->canvas.dimensions, EVENT_MAINEDITOR_RESIZELAYER));
                            }
                        }
                    },
                    {SDLK_v, { "Resize canvas (per tile)",
                            [](MainEditor* editor) {
                                if (editor->tileDimensions.x == 0 || editor->tileDimensions.y == 0) {
                                    g_addNotification(ErrorNotification("Error", "Set the pixel grid first."));
                                }
                                else {
                                    g_addPopup(new PopupTileGeneric(editor, "Resize canvas by tile size", "New tile size:", XY{ editor->tileDimensions.x, editor->tileDimensions.y }, EVENT_MAINEDITOR_RESIZELAYER_BY_TILE));
                                }
                            }
                        }
                    },
                    {SDLK_b, { "Resize canvas (per n.tiles)",
                            [](MainEditor* editor) {
                                if (editor->tileDimensions.x == 0 || editor->tileDimensions.y == 0) {
                                    g_addNotification(ErrorNotification("Error", "Set the pixel grid first."));
                                }
                                else {
                                    g_addPopup(new PopupTileGeneric(editor, "Resize canvas by tile count", "New tile count:", XY{ (int)ceil(editor->canvas.dimensions.x / (float)editor->tileDimensions.x), (int)ceil(editor->canvas.dimensions.y / (float)editor->tileDimensions.y) }, EVENT_MAINEDITOR_RESIZELAYER_BY_TILECOUNT));
                                }
                            }
                        }
                    },
                    {SDLK_n, { "Integer scale canvas",
                            [](MainEditor* editor) {
                                g_addPopup(new PopupIntegerScale(editor, "Integer scale canvas", "Scale:", XY{ 1,1 }, EVENT_MAINEDITOR_INTEGERSCALE));
                            }
                        }
                    },
                },
                g_iconNavbarTabEdit
            }
        },
        {
            SDLK_l,
            {
                "Layer",
                {},
                {
                    {SDLK_f, { "Flip current layer: X axis",
                            [](MainEditor* editor) {
                                editor->layer_flipHorizontally();
                            }
                        }
                    },
                    {SDLK_g, { "Flip current layer: Y axis",
                            [](MainEditor* editor) {
                                editor->layer_flipVertically();
                            }
                        }
                    },
                    {SDLK_r, { "Rename current layer",
                            [](MainEditor* editor) {
                                editor->layer_promptRename();
                            }
                        }
                    },
                    {SDLK_s, { "Isolate layer alpha",
                            [](MainEditor* editor) {
                                editor->layer_selectCurrentAlpha();
                            }
                        }
                    },
                    {SDLK_o, { "Outline current layer",
                            [](MainEditor* editor) {
                                editor->layer_outline(false);
                            }
                        }
                    }
                },
                g_iconNavbarTabLayer
            }
        },
        {
            SDLK_v,
            {
                "View",
                {},
                {
                    {SDLK_r, { "Recenter canvas",
                            [](MainEditor* editor) {
                                editor->recenterCanvas();
                            }
                        }
                    },
                    {SDLK_b, { "Toggle background color",
                            [](MainEditor* editor) {
                                editor->backgroundColor.r = ~editor->backgroundColor.r;
                                editor->backgroundColor.g = ~editor->backgroundColor.g;
                                editor->backgroundColor.b = ~editor->backgroundColor.b;
                            }
                        }
                    },
                    {SDLK_c, { "Toggle comments",
                            [](MainEditor* editor) {
                                (*(int*)&editor->commentViewMode)++;
                                (*(int*)&editor->commentViewMode) %= 3;
                                g_addNotification(Notification(std::format("{}",
                                    editor->commentViewMode == COMMENTMODE_HIDE_ALL ? "All comments hidden" :
                                    editor->commentViewMode == COMMENTMODE_SHOW_HOVERED ? "Comments shown on hover" :
                                    "All comments shown"), "", 1500
                                ));
                            }
                        }
                    },
                    {SDLK_g, { "Set pixel grid...",
                            [](MainEditor* editor) {
                                g_addPopup(new PopupSetEditorPixelGrid(editor, "Set pixel grid", "Enter grid size <w>x<h>:"));
                            }
                        }
                    },
                    {SDLK_s, { "Open spritesheet preview...",
                            [](MainEditor* editor) {
                                //if (editor->spritesheetPreview == NULL) {
                                    if (editor->tileDimensions.x == 0 || editor->tileDimensions.y == 0) {
                                        g_addNotification(ErrorNotification("Error", "Set the pixel grid first."));
                                        return;
                                    }
                                    SpritesheetPreviewScreen* newScreen = new SpritesheetPreviewScreen(editor);
                                    g_addScreen(newScreen);
                                //}
                                //else {
                                //    g_addNotification(ErrorNotification("Error", "Spritesheet preview is already open."));
                                //}
                            }
                        }
                    },
                    {SDLK_t, { "Open tileset preview...",
                            [](MainEditor* editor) {
                                if (editor->tileDimensions.x == 0 || editor->tileDimensions.y == 0) {
                                    g_addNotification(ErrorNotification("Error", "Set the pixel grid first."));
                                    return;
                                }
                                TilemapPreviewScreen* newScreen = new TilemapPreviewScreen(editor);
                                g_addScreen(newScreen);
                                //editor->spritesheetPreview = newScreen;
                            }
                        }
                    },
                    {SDLK_y, { "Open RPG Maker 2K/2K3 ChipSet preview...",
                            [](MainEditor* editor) {
                                if (!xyEqual(editor->canvas.dimensions, {480,256})) {
                                    g_addNotification(ErrorNotification("Error", "Dimensions must be 480x256"));
                                    return;
                                }
                                RPG2KTilemapPreviewScreen* newScreen = new RPG2KTilemapPreviewScreen(editor);
                                g_addScreen(newScreen);
                                //editor->spritesheetPreview = newScreen;
                            }
                        }
                    },
                    {SDLK_n, { "Open cube preview...",
                            [](MainEditor* editor) {
                                if (editor->tileDimensions.x == 0 || editor->tileDimensions.y == 0) {
                                    g_addNotification(ErrorNotification("Error", "Tile grid must be set"));
                                    return;
                                }
                                MinecraftBlockPreviewScreen* newScreen = new MinecraftBlockPreviewScreen(editor);
                                g_addScreen(newScreen);
                            }
                        }
                    },
                },
                g_iconNavbarTabView
            }
        }
    };

    currentBrush = g_brushes[0];
    currentPattern = g_patterns[0];

    colorPicker = new PalettizedEditorColorPicker(this);
    colorPicker->position.y = 50;
    colorPicker->position.x = 10;
    wxsManager.addDrawable(colorPicker);
    ((PalettizedEditorColorPicker*)colorPicker)->setPickedPaletteIndex(pickedPaletteIndex);
    //regenerateLastColors();

    brushPicker = new EditorBrushPicker(this);
    brushPicker->position.y = 450;
    brushPicker->position.x = 10;
    wxsManager.addDrawable(brushPicker);

    layerPicker = new PalettizedEditorLayerPicker(this);
    layerPicker->position = XY{ 440, 80 };
    layerPicker->anchor = XY{ 1,0 };
    wxsManager.addDrawable(layerPicker);

    navbar = new ScreenWideNavBar<MainEditor*>(this, mainEditorKeyActions, { SDLK_f, SDLK_e, SDLK_l, SDLK_v });
    wxsManager.addDrawable(navbar);
}

void MainEditorPalettized::trySaveImage()
{
    trySavePalettizedImage();
}

bool MainEditorPalettized::trySaveWithExporter(PlatformNativePathString name, FileExporter* exporter)
{
    bool result = false;
    if (exporter->exportsWholeSession()) {
        result = exporter->exportData(name, this);
    }
    else {
        Layer* flat = flattenImageWithoutConvertingToRGB();
        result = exporter->exportData(name, flat);
        delete flat;
    }
    if (result) {
        lastConfirmedSave = true;
        lastConfirmedSavePath = name;
        lastConfirmedExporter = exporter;
        changesSinceLastSave = false;
        if (lastWasSaveAs && g_config.openSavedPath) {
            platformOpenFileLocation(lastConfirmedSavePath);
        }
        g_addNotification(SuccessNotification("Success", "File saved successfully."));
    }
    else {
        g_addNotification(ErrorNotification("Error", "Failed to save file."));
    }

    return result;
}

void MainEditorPalettized::trySaveAsImage()
{
    trySaveAsPalettizedImage();
}

Layer* MainEditorPalettized::flattenImage()
{
    return flattenImageAndConvertToRGB();
}

Layer* MainEditorPalettized::newLayer()
{
    LayerPalettized* nl = new LayerPalettized(canvas.dimensions.x, canvas.dimensions.y);
    nl->palette = palette;
    nl->name = std::format("New Layer {}", layers.size() + 1);
    layers.push_back(nl);
    switchActiveLayer(layers.size() - 1);

    addToUndoStack(UndoStackElement{ nl, UNDOSTACK_CREATE_LAYER });
    return nl;
}

Layer* MainEditorPalettized::mergeLayers(Layer* bottom, Layer* top)
{
    LayerPalettized* ret = new LayerPalettized(bottom->w, bottom->h);
    ret->palette = palette;

    if (!bottom->isPalettized || !top->isPalettized) {
        printf("UH OH\n");
    }

    memcpy(ret->pixelData, bottom->pixelData, bottom->w * bottom->h * 4);

    uint32_t* ppx = (uint32_t*)top->pixelData;
    uint32_t* retppx = (uint32_t*)ret->pixelData;
    for (uint64_t p = 0; p < ret->w * ret->h; p++) {
        uint32_t pixel = ppx[p];
        uint32_t srcPixel = retppx[p];
        retppx[p] = pixel == -1 ? srcPixel : pixel;
    }

    return ret;
}

void MainEditorPalettized::exportTilesIndividually()
{
    if (tileDimensions.x != 0 && tileDimensions.y != 0) {
        std::vector<std::pair<std::string, std::string>> formats;
        for (auto f : g_palettizedFileExporters) {
            formats.push_back({ f->extension(), f->name() });
        }
        platformTrySaveOtherFile(this, formats, "export tiles", EVENT_MAINEDITOR_EXPORTTILES);
    }
    else {
        g_addNotification(ErrorNotification("Error", "Set the pixel grid first."));
    }
}

int32_t* MainEditorPalettized::makeFlatIndicesTable()
{
    int32_t* indices = (int32_t*)tracked_malloc(canvas.dimensions.x * canvas.dimensions.y * 4);
    memset(indices, 0, canvas.dimensions.x * canvas.dimensions.y * 4);
    for (Layer*& l : layers) {
        if (!l->hidden) {
            for (int y = 0; y < canvas.dimensions.y; y++) {
                for (int x = 0; x < canvas.dimensions.x; x++) {
                    uint32_t color = l->getPixelAt(XY{ x,y });
                    if (color != -1) {
                        indices[x + y * canvas.dimensions.x] = color;
                    }
                }
            }
        }
    }
    return indices;
}

Layer* MainEditorPalettized::flattenImageAndConvertToRGB()
{
    int32_t* indices = makeFlatIndicesTable();

    Layer* flatAndRGBConvertedLayer = new Layer(canvas.dimensions.x, canvas.dimensions.y);
    uint32_t* intpxdata = (uint32_t*)flatAndRGBConvertedLayer->pixelData;
    for (int y = 0; y < canvas.dimensions.y; y++) {
        for (int x = 0; x < canvas.dimensions.x; x++) {
            intpxdata[x + y * canvas.dimensions.x] = palette[indices[x + y * canvas.dimensions.x]];
        }
    }
    tracked_free(indices);
    return flatAndRGBConvertedLayer;
}

Layer* MainEditorPalettized::flattenImageWithoutConvertingToRGB()
{
    int32_t* indices = makeFlatIndicesTable();
    LayerPalettized* flatLayer = new LayerPalettized(canvas.dimensions.x, canvas.dimensions.y);
    memcpy(flatLayer->pixelData, indices, canvas.dimensions.x * canvas.dimensions.y * 4);
    flatLayer->palette = palette;
    tracked_free(indices);
    return flatLayer;
}

void MainEditorPalettized::tryExportRGB()
{
    std::vector<std::pair<std::string, std::string>> formats;
    for (auto f : g_fileExporters) {
        formats.push_back({ f->extension(), f->name() });
    }
    platformTrySaveOtherFile(this, formats, "export image as RGB", EVENT_PALETTIZEDEDITOR_EXPORTRGBFILE);
}

void MainEditorPalettized::trySavePalettizedImage()
{
    lastWasSaveAs = false;
    if (!lastConfirmedSave) {
        trySaveAsImage();
    }
    else {
        if (trySaveWithExporter(lastConfirmedSavePath, lastConfirmedExporter)) {
            g_tryPushLastFilePath(convertStringToUTF8OnWin32(lastConfirmedSavePath));
        }
    }
}

void MainEditorPalettized::trySaveAsPalettizedImage()
{
    lastWasSaveAs = true;
    std::vector<std::pair<std::string, std::string>> namesAndExtensions;
    for (auto& e : g_palettizedFileExporters) {
        if ((e->formatFlags() & FORMAT_PALETTIZED) != 0) {
            namesAndExtensions.push_back({ e->extension(), e->name()});
        }
    }
    platformTrySaveOtherFile(this, namesAndExtensions, "save palettized image", EVENT_PALETTIZEDEDITOR_SAVEFILE);
}

MainEditor* MainEditorPalettized::toRGBSession()
{
    std::vector<Layer*> rgbLayers;
    for (Layer*& ll : layers) {
        rgbLayers.push_back(((LayerPalettized*)ll)->toRGB());
    }
    MainEditor* newEditor = new MainEditor(rgbLayers);
    newEditor->tileDimensions = tileDimensions;
    return newEditor;
}



void MainEditorPalettized::openInNormalRGBEditor()
{
    g_addScreen(toRGBSession());
}

