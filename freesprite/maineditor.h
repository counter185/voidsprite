#pragma once
#include "globals.h"
#include "BaseScreen.h"
#include "splitsession.h"
#include "DrawableManager.h"
#include "EditorColorPicker.h"
#include "BaseBrush.h"
#include "Brush1x1.h"
#include "Brush3pxCircle.h"
#include "Brush1pxLine.h"
#include "BrushRect.h"
#include "BrushRectFill.h"
#include "ToolRectClone.h"
#include "ToolColorPicker.h"
#include "ToolSetXSymmetry.h"
#include "ToolSetYSymmetry.h"
#include "Layer.h"
#include "SpritesheetPreviewScreen.h"
#include "ToolComment.h"
#include "ToolMeasure.h"
#include "ScreenWideNavBar.h"
#include "Canvas.h"

enum EditorUnsavedChanges : int {
    NO_UNSAVED_CHANGES = 0,
    CHANGES_RECOVERY_AUTOSAVED = 1,
    HAS_UNSAVED_CHANGES = 2
};

enum MainEditorCommentMode : int {
    COMMENTMODE_HIDE_ALL = 0,
    COMMENTMODE_SHOW_HOVERED = 1,
    COMMENTMODE_SHOW_ALL = 2
};

struct CommentData {
    XY position;
    std::string data;
    Timer64 animTimer;
    bool hovered = false;
};

struct Guideline {
    bool vertical;
    int position;
};

class MainEditor : public BaseScreen, public EventCallbackListener
{
protected:
    MainEditor() {}
public:
    bool isPalettized = false;

    MainEditorCommentMode commentViewMode = COMMENTMODE_SHOW_HOVERED;
    std::vector<CommentData> comments;

    std::vector<Layer*> layers;
    int selLayer = 0;

    std::vector<UndoStackElement> undoStack, redoStack;

    XY tileDimensions = XY{ 0,0 };
    u8 tileGridAlpha = 0x40;
    XY tileGridPaddingBottomRight = XY{ 0,0 };
    SDL_Rect getPaddedTilePosAndDimensions(XY tilePos);
    XY getPaddedTileDimensions();

    Canvas canvas;

    XY mousePixelTargetPoint;
    XY mousePixelTargetPoint2xP;
    XY mouseHoldPosition;
    bool closeNextTick = false;
    BaseBrush* currentBrush;
    bool currentBrushMouseDowned = false;
    bool invertPattern = false;
    Pattern* currentPattern;
    bool leftMouseHold = false;
    bool middleMouseHold = false;
    Timer64 layerSwitchTimer;
    Timer64 colorPickTimer;
    bool lastColorPickWasFromWholeImage = false;
    Timer64 undoTimer;
    bool lastUndoWasRedo = false;
    bool hideUI = false;
    bool penDown = false;

    EditorUnsavedChanges changesSinceLastSave = NO_UNSAVED_CHANGES;
    PlatformNativePathString lastConfirmedSavePath;
    FileExporter* lastConfirmedExporter = NULL;
    bool lastConfirmedSave = false;
    bool lastWasSaveAs = false;

    bool replaceAlphaMode = false;
    bool eraserMode = false;
    bool blendAlphaMode = false;
    uint32_t pickedColor = 0xFFFFFF;

    bool qModifier = false;
    XY lockedTilePreview = { -1,-1 };
    Timer64 tileLockTimer;

    bool zoomKeyHeld = false;
    XY zoomOrigin{};
    int zoomInitial = 0;
    const int zoomPixelStep = 50;
    Timer64 zoomKeyTimer;

    ScreenWideNavBar<MainEditor*>* navbar;
    EditorColorPicker* colorPicker;
    EditorBrushPicker* brushPicker;
    EditorLayerPicker* layerPicker;

    SDL_Color backgroundColor = SDL_Color{0,0,0,255};

    XY symmetryPositions = {0,0};
    XY guidelinePosition = {0,0};
    std::vector<Guideline> guidelines;

    bool symmetryEnabled[2] = { false, false };

    bool isolateEnabled = false;
    ScanlineMap isolatedFragment;

    std::map<SDL_Scancode, NavbarSection<MainEditor*>> mainEditorKeyActions;

    std::vector<uint32_t> lastColors;

    SplitSessionData splitSessionData = { false };

    std::vector<BaseScreen*> hintOpenScreensInInteractiveMode;

    u64 editTime = 0;
    u64 lastTimestamp = -1;

    Timer64 autosaveTimer;

    MainEditor(XY dimensions);
    MainEditor(SDL_Surface* srf);
    MainEditor(Layer* srf);
    MainEditor(std::vector<Layer*> layers);
    ~MainEditor();

    void render() override;
    void tick() override;
    void takeInput(SDL_Event evt) override;

    std::string getName() override { return lastConfirmedSave ? std::format("Editor: {}", fileNameFromPath(convertStringToUTF8OnWin32(lastConfirmedSavePath))) : "Editor"; }
    bool takesTouchEvents() override { return true; }

    void eventFileSaved(int evt_id, PlatformNativePathString name, int exporterId) override;
    void eventPopupClosed(int evt_id, BasePopup* p) override;
    void eventTextInputConfirm(int evt_id, std::string text) override;
    void eventColorSet(int evt_id, uint32_t color) override;
    
    void DrawBackground();
    void DrawForeground();
    void renderComments();
    void renderUndoStack();
    virtual void renderColorPickerAnim();
    void drawSymmetryLines();
    void drawIsolatedFragment();
    void drawTileGrid();
    void renderGuidelines();
    void drawSplitSessionFragments();
    void drawZoomLines();

    void initLayers();
    virtual void setUpWidgets();
    void addWidget(Drawable* wx);
    void removeWidget(Drawable* wx);
    void RecalcMousePixelTargetPoint(int x, int y);
    void FillTexture();
    virtual void SetPixel(XY position, uint32_t color, bool pushToLastColors = true, uint8_t symmetry = 0);
    void DrawLine(XY from, XY to, uint32_t color);
    virtual void trySaveImage();
    virtual bool trySaveWithExporter(PlatformNativePathString name, FileExporter* exporter);
    virtual void trySaveAsImage();
    std::map<std::string,std::string> makeSingleLayerExtdata();
    void loadSingleLayerExtdata(Layer* l);
    std::string makeCommentDataString();
    static std::vector<CommentData> parseCommentDataString(std::string data);
    void recenterCanvas();
    bool requestSafeClose();
    void zoom(int how_much);
    bool isInBounds(XY pos);
    virtual uint32_t pickColorFromAllLayers(XY);
    void regenerateLastColors();
    virtual void setActiveColor(uint32_t, bool animate = true);
    virtual uint32_t getActiveColor();
    void setActiveBrush(BaseBrush* b);
    void tickAutosave();

    void discardEndOfUndoStack();
    void checkAndDiscardEndOfUndoStack();
    void commitStateToLayer(Layer* l);
    void commitStateToCurrentLayer();
    void addToUndoStack(UndoStackElement undo);
    void discardUndoStack();
    void discardRedoStack();
    void undo();
    void redo();

    virtual Layer* newLayer();
    void deleteLayer(int index);
    void moveLayerUp(int index);
    void moveLayerDown(int index);
    void mergeLayerDown(int index);
    void duplicateLayer(int index);
    void switchActiveLayer(int index);
    Layer* getCurrentLayer() {
        return layers[selLayer];
    }
    void layer_setOpacity(uint8_t alpha);
    void layer_promptRename();
    void layer_flipHorizontally();
    void layer_flipVertically();
    uint32_t layer_getPixelAt(XY pos);
    void layer_setAllAlpha255();
    void layer_replaceColor(uint32_t from, uint32_t to);
    void layer_hsvShift(hsv shift);
    void layer_outline(bool wholeImage = false);
    void layer_selectCurrentAlpha();
    virtual Layer* flattenImage();
    virtual Layer* mergeLayers(Layer* bottom, Layer* top);
    void rescaleAllLayersFromCommand(XY size);
    void resizeAllLayersFromCommand(XY size, bool byTile = false);
    void resizzeAllLayersByTilecountFromCommand(XY size);
    void integerScaleAllLayersFromCommand(XY scale, bool downscale = false);
    MainEditorPalettized* toPalettizedSession();
    void tryExportPalettizedImage();
    virtual void exportTilesIndividually();

    bool canAddCommentAt(XY a);
    void addComment(CommentData c);
    void addCommentAt(XY a, std::string c);
    void removeCommentAt(XY a);
private:
    CommentData _removeCommentAt(XY a);
};

