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
#include "CollapsableDraggablePanel.h"
#include "EditorTouchToggle.h"
#include "SpritesheetPreviewScreen.h"
#include "ViewSessionScreen.h"
#include "MinecraftSkinPreviewScreen.h"
#include "UndoStack.h"
#include "EditorFramePicker.h"

#include "PopupIntegerScale.h"
#include "PopupMessageBox.h"
#include "PopupTileGeneric.h"
#include "PopupSetEditorPixelGrid.h"
#include "PopupGlobalConfig.h"
#include "PopupExportScaled.h"
#include "PopupFilePicker.h"
#include "PopupChooseFormat.h"

MainEditorPalettized::MainEditorPalettized(XY dimensions)
{
    isPalettized = true;
    canvas.dimensions = dimensions;
    palette = g_palettes()[PALETTE_DEFAULT];
    LayerPalettized* pltLayer = new LayerPalettized(dimensions.x, dimensions.y);
    pltLayer->palette = palette;
    getLayerStack().push_back(pltLayer);

    setUpWidgets();
    recenterCanvas();
    initLayers();
}

MainEditorPalettized::MainEditorPalettized(LayerPalettized* layer)
{
    isPalettized = true;
    canvas.dimensions = { layer->w, layer->h };

    getLayerStack().push_back(layer);
    palette = layer->palette;

    setUpWidgets();
    recenterCanvas();
    initLayers();
}

MainEditorPalettized::MainEditorPalettized(std::vector<LayerPalettized*> layerss)
{
    //all layers must have the same palette assigned!!!
    auto& layers = getLayerStack();
    isPalettized = true;
    canvas.dimensions = { layerss[0]->w, layerss[0]->h };
    for (auto& l : layerss) {
        layers.push_back(l);
    }
    palette = layerss[0]->palette;

    setUpWidgets();
    recenterCanvas();
    initLayers();
}

MainEditorPalettized::MainEditorPalettized(std::vector<Frame*> framess)
{
    isPalettized = true;
    for (auto*& f : frames) {
        delete f;
    }
    frames = framess;
    //todo: check if these are all indexed layers

    Layer* firstLayer = frames.front()->layers.front();
    canvas.dimensions = { firstLayer->w, firstLayer->h };
    palette = ((LayerPalettized*)firstLayer)->palette;

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
#if VSP_PLATFORM == VSP_PLATFORM_EMSCRIPTEN
                emDownloadFile(lastConfirmedSavePath);
#endif
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
                Layer* l = flattenImageAndConvertToRGB(getCurrentFrame());
                result = exporter->exportData(name, l);
                delete l;
            }

            if (result) {
#if VSP_PLATFORM == VSP_PLATFORM_EMSCRIPTEN
                emDownloadFile(lastConfirmedSavePath);
#endif
                if (lastWasSaveAs && g_config.openSavedPath) {
                    platformOpenFileLocation(lastConfirmedSavePath);
                }
                g_addNotification(SuccessNotification("Success", "File exported successfully."));
            }
            else {
                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Failed to exported file."));
            }
        }
        else {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Invalid exporter"));
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
                    PlatformNativePathString tileName = name.substr(0, name.find_last_of(convertStringOnWin32("."))) + convertStringOnWin32(frmt("_{}_{}{}", x, y, exporter->extension()));
                    if (!exporter->exportsWholeSession()) {
                        exporter->exportData(tileName, clip);
#if VSP_PLATFORM == VSP_PLATFORM_EMSCRIPTEN
                        emDownloadFile(tileName);
#endif
                        delete clip;
                    }
                    else {
                        MainEditorPalettized* session = new MainEditorPalettized((LayerPalettized*)clip);
                        session->trySaveWithExporter(tileName, exporter);
                        delete session;
                    }
                }
                else {
                    g_addNotification(ErrorNotification("Error", frmt("Failed to export tile {}:{}", x, y)));
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
        SetPixel(XY{ symmetryFlippedX, position.y }, color, false, symmetry | 0b10);
    }
    if (symmetryEnabled[1] && !(symmetry & 0b1)) {
        int symmetryYPoint = symmetryPositions.y / 2;
        bool symYPointIsCentered = symmetryPositions.y % 2;
        int symmetryFlippedY = symmetryYPoint + (symmetryYPoint - position.y) - (symYPointIsCentered ? 0 : 1);
        SetPixel(XY{ position.x, symmetryFlippedY }, color, false, symmetry | 0b1);
    }
}

uint32_t MainEditorPalettized::getActiveColor()
{
    return pickedPaletteIndex;
}

void MainEditorPalettized::setActiveColor(uint32_t col)
{
    if (col == -1) {
        col = 0;
    }
    ((PalettizedEditorColorPicker*)colorPicker)->setPickedPaletteIndex(col);
}

uint32_t MainEditorPalettized::pickColorFromAllLayers(XY pos)
{
    auto& layers = getLayerStack();
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

void MainEditorPalettized::setPaletteIndex(u32 index, u32 color) {
    if (index < palette.size()) {
        palette[index] = color;
    }
    updatePalette();
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

    updatePalette();
}

void MainEditorPalettized::updatePalette() {
    for (auto& frame : frames) {
        auto& layers = frame->layers;
        for (Layer*& l : layers) {
            ((LayerPalettized*)l)->palette = palette;
            ((LayerPalettized*)l)->markLayerDirty();
        }
    }

    ((PalettizedEditorColorPicker*)colorPicker)->updateForcedColorPaletteButtons();
}

void MainEditorPalettized::playColorPickerVFX(bool inward)
{
    u32 targetColor = 0;
    if (pickedPaletteIndex < palette.size() && pickedPaletteIndex > 0) {
        targetColor = palette[pickedPaletteIndex];
    }
    g_newVFX(VFX_COLORPICKER, 500, targetColor, { g_mouseX, g_mouseY,-1,-1 }, { inward ? 1u : 0u });
}

void MainEditorPalettized::setUpWidgets()
{
    compactEditor = g_config.compactEditor;

    mainEditorKeyActions = {
        {
            SDL_SCANCODE_F,
            {
                makeNavbarSection(TL("vsp.nav.file"), g_iconNavbarTabFile,
                {
                    {SDL_SCANCODE_S, { TL("vsp.nav.save"), [this]() { this->trySaveImage(); } } },
                    {SDL_SCANCODE_D, { TL("vsp.maineditor.saveas"), [this]() { this->trySaveAsImage(); } } },
                    {SDL_SCANCODE_W, { TL("vsp.maineditor.nav.forceautosave"), [this]() { this->createRecoveryAutosave(); } } },
                    {SDL_SCANCODE_F, { TL("vsp.maineditor.nav.exportscaled"),
                            [this]() {
                                PopupExportScaled* popup = new PopupExportScaled(this);
                                popup->setCallbackListener(EVENT_MAINEDITOR_EXPORTSCALED, this);
                                g_addPopup(popup);
                            }
                        }
                    },
                    {SDL_SCANCODE_E, { "Export as RGB", [this]() { this->tryExportRGB(); } } },
                    {SDL_SCANCODE_A, { "Export tiles individually", [this]() { this->exportTilesIndividually(); } } },
                    {SDL_SCANCODE_R, { "Open in RGB editor", [this]() { this->openInNormalRGBEditor(); } } },
                    {SDL_SCANCODE_C, { TL("vsp.maineditor.copyflattoclipboard"), [this]() { this->copyImageToClipboard(); } } },
                    {SDL_SCANCODE_P, { TL("vsp.maineditor.preference"), [this]() { g_addPopup(new PopupGlobalConfig()); } } },
                    {SDL_SCANCODE_X, { TL("vsp.cmn.close"), [this]() { this->requestSafeClose(); } } },
                })
            }
        },
        {
            SDL_SCANCODE_E,
            {
                makeNavbarSection(TL("vsp.maineditor.edit"), g_iconNavbarTabEdit, {
                    {SDL_SCANCODE_Z, { TL("vsp.maineditor.undo"), [this]() { this->undo(); } } },
                    {SDL_SCANCODE_R, { TL("vsp.maineditor.redo"), [this]() { this->redo(); } } },
                    {SDL_SCANCODE_X, { TL("vsp.maineditor.symx"),
                            [this]() {
                                this->symmetryEnabled[0] = !this->symmetryEnabled[0];
                            }
                        }
                    },
                    {SDL_SCANCODE_Y, { TL("vsp.maineditor.symy"),
                            [this]() {
                                this->symmetryEnabled[1] = !this->symmetryEnabled[1];
                            }
                        }
                    },
                    {SDL_SCANCODE_F, { TL("vsp.maineditor.flipallx"), [this]() { this->flipAllLayersOnX(); } } },
                    {SDL_SCANCODE_G, { TL("vsp.maineditor.flipally"),
                            [this]() {
                                this->flipAllLayersOnY();
                            }
                        }
                    },
                    {SDL_SCANCODE_C, { TL("vsp.maineditor.rescanv"),
                            [this]() {
                                g_addPopup(new PopupTileGeneric(this, TL("vsp.maineditor.rescanv"), "New canvas size:", this->canvas.dimensions, EVENT_MAINEDITOR_RESIZELAYER));
                            }
                        }
                    },
                    {SDL_SCANCODE_S, { TL("vsp.maineditor.dsel"),
                            [this]() { this->isolateEnabled = false; }
                        }
                    },
                    {SDL_SCANCODE_V, { TL("vsp.maineditor.rescanv_bytile"),
                            [this]() {
                                if (this->tileDimensions.x == 0 || this->tileDimensions.y == 0) {
                                    g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Set the pixel grid first."));
                                }
                                else {
                                    g_addPopup(new PopupTileGeneric(this, "Resize canvas by tile size", "New tile size:", XY{ this->tileDimensions.x, this->tileDimensions.y }, EVENT_MAINEDITOR_RESIZELAYER_BY_TILE));
                                }
                            }
                        }
                    },
                    {SDL_SCANCODE_B, { TL("vsp.maineditor.rescanv_ntile"),
                            [this]() {
                                if (this->tileDimensions.x == 0 || this->tileDimensions.y == 0) {
                                    g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Set the pixel grid first."));
                                }
                                else {
                                    g_addPopup(new PopupTileGeneric(this, "Resize canvas by tile count", "New tile count:", XY{ (int)ceil(this->canvas.dimensions.x / (float)this->tileDimensions.x), (int)ceil(this->canvas.dimensions.y / (float)this->tileDimensions.y) }, EVENT_MAINEDITOR_RESIZELAYER_BY_TILECOUNT));
                                }
                            }
                        }
                    },
                    {SDL_SCANCODE_N, { TL("vsp.maineditor.intscale"),
                            [this]() {
                                g_addPopup(new PopupIntegerScale(this, TL("vsp.maineditor.intscale"), "Scale:", canvas.dimensions, XY{ 1,1 }, EVENT_MAINEDITOR_INTEGERSCALE));
                            }
                        }
                    },
                    {SDL_SCANCODE_M, { TL("vsp.maineditor.canvscale"),
                            [this]() {
                                g_addPopup(new PopupTileGeneric(this, TL("vsp.maineditor.canvscale"), "New size:", this->canvas.dimensions, EVENT_MAINEDITOR_RESCALELAYER));
                            }
                        }
                    },
                    {SDL_SCANCODE_COMMA, { TL("vsp.maineditor.rescanv_reorder"),
                            [this]() {
                                g_addPopup(new PopupTileGeneric(this, TL("vsp.maineditor.rescanv_reorder"), "New size:", this->canvas.dimensions, EVENT_MAINEDITOR_RESIZELAYER_REORDER_TILES));
                            }
                        }
                    },
                })
            }
        },
        {
            SDL_SCANCODE_L,
            {
                makeNavbarSection(TL("vsp.maineditor.layer"), g_iconNavbarTabLayer,
                {
                    {SDL_SCANCODE_F, { "Flip current layer: X axis", [this]() { this->layer_flipHorizontally();}}},
                    {SDL_SCANCODE_G, { "Flip current layer: Y axis",[this]() {this->layer_flipVertically();}}},
                    {SDL_SCANCODE_R, { "Rename current layer", [this]() { this->layer_promptRenameCurrent();}}},
                    {SDL_SCANCODE_S, { "Select layer alpha",[this]() {this->layer_selectCurrentAlpha();}}},
                    {SDL_SCANCODE_O, { "Outline current layer",[this]() {this->layer_outline(false);}}},
                    {SDL_SCANCODE_V, { TL("vsp.maineditor.nav.layer.transformlayer"),[this]() {this->layer_promptTransform(); }}},
                    {SDL_SCANCODE_E, { TL("vsp.maineditor.nav.layer.clearselection"),[this]() {this->layer_clearSelectedArea();}}},
                    {SDL_SCANCODE_M, { TL("vsp.maineditor.nav.layer.newvariant"),[this]() { this->layer_newVariant(); }}},
                    {SDL_SCANCODE_N, { TL("vsp.maineditor.nav.layer.copyvariant"),[this]() { this->layer_duplicateActiveVariant(); }}},
                    {SDL_SCANCODE_T, { TL("vsp.maineditor.nav.layer.renvariant"),[this]() { this->layer_promptRenameCurrentVariant(); }}},
                    //todo: fix it (make it so that the -1 index never gets passed)
                    /*{SDL_SCANCODE_C, {TL("vsp.maineditor.nav.layer.copylayertoclipboard"),
                            [](MainEditor* editor) {
                                editor->copyLayerToClipboard(editor->getCurrentLayer());
                            }
                        }
                    }*/
                })
            }
        },
        {
            SDL_SCANCODE_V,
            {
                makeNavbarSection(TL("vsp.nav.view"), g_iconNavbarTabView,
                {
                    {SDL_SCANCODE_R, { "Recenter canvas", [this]() { this->recenterCanvas(); }}},
                    {SDL_SCANCODE_F, { "Add reference...",
                            [this]() {
                                PopupFilePicker::PlatformAnyImageImportDialog(this, TL("vsp.popup.addreference"), EVENT_MAINEDITOR_ADD_REFERENCE, true);
                            }
                        }
                    },
                    {SDL_SCANCODE_V, { "Add reference from clipboard...", [this]() { tryAddReferenceFromClipboard(); }}},
                    {SDL_SCANCODE_B, { "Toggle background color",
                            [this]() {
                                this->backgroundColor.r = ~this->backgroundColor.r;
                                this->backgroundColor.g = ~this->backgroundColor.g;
                                this->backgroundColor.b = ~this->backgroundColor.b;
                            }
                        }
                    },
                    {SDL_SCANCODE_C, { "Toggle comments",
                            [this]() {
                                (*(int*)&this->commentViewMode)++;
                                (*(int*)&this->commentViewMode) %= 3;
                                g_addNotification(Notification(frmt("{}",
                                    this->commentViewMode == COMMENTMODE_HIDE_ALL ? "All comments hidden" :
                                    this->commentViewMode == COMMENTMODE_SHOW_HOVERED ? "Comments shown on hover" :
                                    "All comments shown"), "", 1500
                                ));
                            }
                        }
                    },
                    {SDL_SCANCODE_L, { "Toggle guidelines",
                            [this]() {
                                (*(int*)&this->guidelineDisplayMode)++;
                                (*(int*)&this->guidelineDisplayMode) %= 3;
                                g_addNotification(Notification(frmt("{}",
                                    this->guidelineDisplayMode == GUIDELINE_HIDE_ALL ? "All guidelines hidden" :
                                    this->guidelineDisplayMode == GUIDELINE_SHOW_COLORED ? "Guidelines shown in active color" :
                                    "Guidelines shown in UI color"), "", 1500
                                ));
                            }
                        }
                    },
                    {SDL_SCANCODE_G, { "Set pixel grid...",
                            [this]() { g_addPopup(new PopupSetEditorPixelGrid(this, "Set pixel grid", "Enter grid size <w>x<h>:")); }
                        }
                    },
                    {SDL_SCANCODE_P, { "Open preview panel...", [this]() { openPreviewPanel(); }}},
                    {SDL_SCANCODE_T, { "Open touch mode panel...", [this]() { openTouchModePanel(); }}},
                    {SDL_SCANCODE_A, { "Open frames panel...",
                            [this]() {
                                framePicker->enabled = true;
                                framePicker->playPanelOpenVFX();
                            }
                        }
                    },
                })
            }
        },
        {
            SDL_SCANCODE_P,
            makeNavbarSection(TL("vsp.nav.preview"), {
                {SDL_SCANCODE_V, { "Preview in separate workspace...",
                        [this]() {
                            g_addScreen(new ViewSessionScreen(this));
                        }
                    }
                },
                {SDL_SCANCODE_N, { "Preview 3D cube...",
                        [this]() {
                            if (this->tileDimensions.x == 0 || this->tileDimensions.y == 0) {
                                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Tile grid must be set"));
                                return;
                            }
                            MinecraftBlockPreviewScreen* newScreen = new MinecraftBlockPreviewScreen(this);
                            g_addScreen(newScreen);
                        }
                    }
                },
                {SDL_SCANCODE_S, { "Preview spritesheet...",
                        [this]() {
                            if (this->tileDimensions.x == 0 || this->tileDimensions.y == 0) {
                                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Set the pixel grid first."));
                                return;
                            }
                            SpritesheetPreviewScreen* newScreen = new SpritesheetPreviewScreen(this);
                            g_addScreen(newScreen);
                        }
                    }
                },
                {SDL_SCANCODE_T, { "Preview tileset...",
                        [this]() {
                            if (this->tileDimensions.x == 0 || this->tileDimensions.y == 0) {
                                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Set the pixel grid first."));
                                return;
                            }
                            TilemapPreviewScreen* newScreen = new TilemapPreviewScreen(this);
                            g_addScreen(newScreen);
                        }
                    }
                },
                {SDL_SCANCODE_M, { "Preview Minecraft skin...",
                        [this]() {
                            if (!MinecraftSkinPreviewScreen::dimensionsValidForPreview(this->canvas.dimensions)) {
                                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Invalid size. Aspect must be 1:1 or 2:1."));
                                return;
                            }
                            g_addScreen(new MinecraftSkinPreviewScreen(this));
                        }
                    }
                },
#if VSP_USE_LIBLCF
                {SDL_SCANCODE_R, { "Preview RPG Maker 2K / 2K3 ChipSet...",
                        [this]() {
                            if (!xyEqual(this->canvas.dimensions, {480, 256})) {
                                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Dimensions must be 480x256"));
                                return;
                            }
                            RPG2KTilemapPreviewScreen* newScreen = new RPG2KTilemapPreviewScreen(this);
                            g_addScreen(newScreen);
                        }
                    }
                },
#endif
            })
        }
    };

    //add print
    if (platformSupportsFeature(VSP_FEATURE_OS_PRINTER)) {
        mainEditorKeyActions[SDL_SCANCODE_F].order.insert(mainEditorKeyActions[SDL_SCANCODE_F].order.begin() + 9, SDL_SCANCODE_L);
        mainEditorKeyActions[SDL_SCANCODE_F].actions[SDL_SCANCODE_L] = {
            TL("vsp.maineditor.nav.print"),
            [this]() {
                PopupIntegerScale* printScalePopup = new PopupIntegerScale(this, TL("vsp.maineditor.nav.print.scale"), TL("vsp.maineditor.nav.print.scale.desc"), canvas.dimensions, {1,1}, EVENT_MAINEDITOR_PRINT, false);
                g_addPopup(printScalePopup);
            }
        };
    }

    colorPicker = new PalettizedEditorColorPicker(this);
    auto colorPickerPanel = new CollapsableDraggablePanel("COLOR PICKER", colorPicker);
    colorPickerPanel->position.y = 63;
    colorPickerPanel->position.x = 10;
    wxsManager.addDrawable(colorPickerPanel);
    ((PalettizedEditorColorPicker*)colorPicker)->setPickedPaletteIndex(pickedPaletteIndex);
    //regenerateLastColors();

    brushPicker = new EditorBrushPicker(this);
    brushPicker->position.y = 454;
    brushPicker->position.x = 10;
    wxsManager.addDrawable(brushPicker);

    layerPicker = new PalettizedEditorLayerPicker(this);
    layerPicker->position = XY{ 440, 80 };
    layerPicker->anchor = XY{ 1,0 };
    wxsManager.addDrawable(layerPicker);

    framePicker = new EditorFramePicker(this);
    framePicker->position = XY{ 10 + colorPickerPanel->getDimensions().x, 67 };
    wxsManager.addDrawable(framePicker);

    navbar = new ScreenWideNavBar(this, mainEditorKeyActions, { SDL_SCANCODE_F, SDL_SCANCODE_E, SDL_SCANCODE_L, SDL_SCANCODE_V, SDL_SCANCODE_P });
    wxsManager.addDrawable(navbar);

    makeActionBar();

    setActiveBrush(g_brushes[0]);
    currentPattern = g_patterns[0];

    if (g_lastConfirmInputWasTouch) {
        openTouchModePanel();
    }

    if (g_config.compactEditor) {
        std::vector<CompactEditorSection> createSections = {
            {colorPickerPanel, g_iconCompactColorPicker},
            {brushPicker, g_iconCompactToolPicker},
            {layerPicker, g_iconCompactLayerPicker},
            {framePicker, g_iconCompactFramePicker}
        };

        SetupCompactEditor(createSections);
    }
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
        changesSinceLastSave = NO_UNSAVED_CHANGES;
#if VSP_PLATFORM == VSP_PLATFORM_EMSCRIPTEN
        emDownloadFile(lastConfirmedSavePath);
#endif
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

Layer* MainEditorPalettized::flattenFrame(Frame* f)
{
    return flattenImageAndConvertToRGB(f);
}

Layer* MainEditorPalettized::newLayer()
{
    auto& layers = getLayerStack();
    LayerPalettized* nl = LayerPalettized::tryAllocIndexedLayer(canvas.dimensions.x, canvas.dimensions.y);
    if (nl != NULL) {
        nl->palette = palette;
        nl->name = frmt("New Layer {}", layers.size() + 1);
        int insertAtIdx = std::find(layers.begin(), layers.end(), getCurrentLayer()) - layers.begin() + 1;
        layers.insert(layers.begin() + insertAtIdx, nl);
        switchActiveLayer(insertAtIdx);

        addToUndoStack(new UndoLayerCreated(getCurrentFrame(), nl, insertAtIdx));
    }
    else {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.error.mallocfail")));
    }
    return nl;
}

Layer* MainEditorPalettized::mergeLayers(Layer* bottom, Layer* top)
{
    LayerPalettized* ret = new LayerPalettized(bottom->w, bottom->h);
    ret->palette = palette;

    if (!bottom->isPalettized || !top->isPalettized) {
        logerr("UH OH\n");
    }

    memcpy(ret->pixels32(), bottom->pixels32(), bottom->w * bottom->h * 4);

    uint32_t* ppx = top->pixels32();
    uint32_t* retppx = ret->pixels32();
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

int32_t* MainEditorPalettized::makeFlatIndicesTable(Frame* f)
{
    int32_t* indices = (int32_t*)tracked_malloc(canvas.dimensions.x * canvas.dimensions.y * 4);
    memset(indices, 0, canvas.dimensions.x * canvas.dimensions.y * 4);
    for (Layer*& l : f->layers) {
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

Layer* MainEditorPalettized::flattenImageAndConvertToRGB(Frame* f)
{
    int32_t* indices = makeFlatIndicesTable(f);

    Layer* flatAndRGBConvertedLayer = new Layer(canvas.dimensions.x, canvas.dimensions.y);
    uint32_t* intpxdata = flatAndRGBConvertedLayer->pixels32();
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
    return flattenFrameWithoutConvertingToRGB(getCurrentFrame());
}

Layer* MainEditorPalettized::flattenFrameWithoutConvertingToRGB(Frame* fr) {
    int32_t* indices = makeFlatIndicesTable(fr);
    LayerPalettized* flatLayer = new LayerPalettized(canvas.dimensions.x, canvas.dimensions.y);
    memcpy(flatLayer->pixels32(), indices, canvas.dimensions.x * canvas.dimensions.y * 4);
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
    std::vector<FormatDef> formats;
    int i = 0;
    for (auto& f : g_fileExporters) {
        if ((f->formatFlags() & FORMAT_PALETTIZED) != 0) {
            formats.push_back({ 
                .name = f->name(), 
                .extension = f->extension(),
                .description = "--test desc from editor",
                .udata = (void*)f
            });
        }
    }
    PopupChooseFormat* popup = new PopupChooseFormat("Choose format", "", formats);
    popup->chooseFormatAndDoFileSavePrompt("save indexed image", [this](FormatDef* f, PlatformNativePathString path) {
        if (trySaveWithExporter(path, (FileExporter*)f->udata)) {
            g_tryPushLastFilePath(convertStringToUTF8OnWin32(path));
        }
    });
}

MainEditor* MainEditorPalettized::toRGBSession()
{
    std::vector<Frame*> rgbFrames;
    for (Frame* ff : frames) {
        Frame* f = new Frame();
        rgbFrames.push_back(f);
        *f = *ff;
        f->layers.clear();
        for (Layer*& ll : ff->layers) {
            f->layers.push_back(((LayerPalettized*)ll)->toRGB());
        }
    }
    MainEditor* newEditor = new MainEditor(rgbFrames);
    newEditor->tileDimensions = tileDimensions;
    return newEditor;
}



void MainEditorPalettized::openInNormalRGBEditor()
{
    g_addScreen(toRGBSession());
}

