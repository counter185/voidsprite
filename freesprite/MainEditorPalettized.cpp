#include "MainEditorPalettized.h"
#include "LayerPalettized.h"
#include "Pattern.h"
#include "Notification.h"
#include "TilemapPreviewScreen.h"
#include "PalettizedEditorColorPicker.h"
#include "PalettizedEditorLayerPicker.h"
#include "EditorBrushPicker.h"
#include "RPG2KTilemapPreviewScreen.h"
#include "FileIO.h"

#include "PopupMessageBox.h"
#include "PopupTileGeneric.h"
#include "PopupSetEditorPixelGrid.h"
#include "PopupGlobalConfig.h"

MainEditorPalettized::MainEditorPalettized(XY dimensions)
{
    isPalettized = true;
    texW = dimensions.x;
    texH = dimensions.y;
    palette = g_palettes[PALETTE_DEFAULT];
    LayerPalettized* pltLayer = new LayerPalettized(texW, texH);
    pltLayer->palette = palette;
    layers.push_back(pltLayer);

#if not _DEBUG
    g_addPopup(new PopupMessageBox("Palettized editor", "Palettized editor is currently a work in progress.\nSome features will be unfinished.\nReport all bugs and crashes in itchio comments or github issues."));
#endif

    setUpWidgets();
    recenterCanvas();
    initLayers();
}

MainEditorPalettized::MainEditorPalettized(LayerPalettized* layer)
{
    isPalettized = true;
    texW = layer->w;
    texH = layer->h;

    layers.push_back(layer);
    palette = layer->palette;

#if not _DEBUG
    g_addPopup(new PopupMessageBox("Palettized editor", "Palettized editor is currently a work in progress.\nSome features will be unfinished.\nReport all bugs and crashes in itchio comments in github issues."));
#endif

    setUpWidgets();
    recenterCanvas();
    initLayers();
}

MainEditorPalettized::MainEditorPalettized(std::vector<LayerPalettized*> layers)
{
    //all layers must have the same palette assigned!!!
    isPalettized = true;
    texW = layers[0]->w;
    texH = layers[0]->h;
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
            trySaveWithExporter(name, actualExporterID);
        }
    }
}

void MainEditorPalettized::SetPixel(XY position, uint32_t color, uint8_t symmetry)
{
    if (currentPattern->canDrawAt(position) && (!replaceAlphaMode || (replaceAlphaMode && layer_getPixelAt(position) != -1))) {
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
    ((PalettizedEditorColorPicker*)colorPicker)->setPickedPaletteIndex(col);
    if (animate) {
        colorPickTimer.start();
    }
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

void MainEditorPalettized::setUpWidgets()
{
    mainEditorKeyActions = {
        {
            SDLK_f,
            {
                "File",
                {SDLK_s, SDLK_d, SDLK_c, SDLK_r, SDLK_p},
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
                {SDLK_z, SDLK_r, SDLK_x, SDLK_y, SDLK_c, SDLK_v, SDLK_b, SDLK_n},
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
                    {SDLK_c, { "Resize canvas",
                            [](MainEditor* editor) {
                                g_addPopup(new PopupTileGeneric(editor, "Resize canvas", "New canvas size:", XY{editor->texW, editor->texH}, EVENT_MAINEDITOR_RESIZELAYER));
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
                                    g_addPopup(new PopupTileGeneric(editor, "Resize canvas by tile count", "New tile count:", XY{ (int)ceil(editor->texW / (float)editor->tileDimensions.x), (int)ceil(editor->texH / (float)editor->tileDimensions.y) }, EVENT_MAINEDITOR_RESIZELAYER_BY_TILECOUNT));
                                }
                            }
                        }
                    },
                    {SDLK_n, { "Integer scale canvas",
                            [](MainEditor* editor) {
                                g_addPopup(new PopupTileGeneric(editor, "Integer scale canvas", "Scale:", XY{ 1,1 }, EVENT_MAINEDITOR_INTEGERSCALE));
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
                                if (editor->spritesheetPreview == NULL) {
                                    if (editor->tileDimensions.x == 0 || editor->tileDimensions.y == 0) {
                                        g_addNotification(ErrorNotification("Error", "Set the pixel grid first."));
                                        return;
                                    }
                                    SpritesheetPreviewScreen* newScreen = new SpritesheetPreviewScreen(editor);
                                    g_addScreen(newScreen);
                                    editor->spritesheetPreview = newScreen;
                                }
                                else {
                                    g_addNotification(ErrorNotification("Error", "Spritesheet preview is already open."));
                                }
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
                                if (editor->texW != 480 || editor->texH != 256) {
                                    g_addNotification(ErrorNotification("Error", "Dimensions must be 480x256"));
                                    return;
                                }
                                RPG2KTilemapPreviewScreen* newScreen = new RPG2KTilemapPreviewScreen(editor);
                                g_addScreen(newScreen);
                                //editor->spritesheetPreview = newScreen;
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
    colorPicker->position.y = 80;
    colorPicker->position.x = 10;
    wxsManager.addDrawable(colorPicker);
    ((PalettizedEditorColorPicker*)colorPicker)->setPickedPaletteIndex(pickedPaletteIndex);
    //regenerateLastColors();

    brushPicker = new EditorBrushPicker(this);
    brushPicker->position.y = 480;
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
    LayerPalettized* nl = new LayerPalettized(texW, texH);
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

int32_t* MainEditorPalettized::makeFlatIndicesTable()
{
    int32_t* indices = (int32_t*)malloc(texW * texH * 4);
    memset(indices, 0, texW * texH * 4);
    for (Layer*& l : layers) {
        if (!l->hidden) {
            for (int y = 0; y < texH; y++) {
                for (int x = 0; x < texW; x++) {
                    uint32_t color = l->getPixelAt(XY{ x,y });
                    if (color != -1) {
                        indices[x + y * texW] = color;
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

    Layer* flatAndRGBConvertedLayer = new Layer(texW, texH);
    uint32_t* intpxdata = (uint32_t*)flatAndRGBConvertedLayer->pixelData;
    for (int y = 0; y < texH; y++) {
        for (int x = 0; x < texW; x++) {
            intpxdata[x + y * texW] = palette[indices[x + y * texW]];
        }
    }
    free(indices);
    return flatAndRGBConvertedLayer;
}

Layer* MainEditorPalettized::flattenImageWithoutConvertingToRGB()
{
    int32_t* indices = makeFlatIndicesTable();
    LayerPalettized* flatLayer = new LayerPalettized(texW, texH);
    memcpy(flatLayer->pixelData, indices, texW * texH * 4);
    flatLayer->palette = palette;
    free(indices);
    return flatLayer;
}

void MainEditorPalettized::trySavePalettizedImage()
{
    lastWasSaveAs = false;
    if (!lastConfirmedSave) {
        trySaveAsImage();
    }
    else {
        trySaveWithExporter(lastConfirmedSavePath, lastConfirmedExporter);
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
    Layer* l = flattenImageAndConvertToRGB();
    MainEditor* newEditor = new MainEditor(l);
    return newEditor;
}



void MainEditorPalettized::openInNormalRGBEditor()
{
    g_addScreen(toRGBSession());
}

