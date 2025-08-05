#pragma once
#include <mutex>
#include <thread>

#include "globals.h"
#include "BaseScreen.h"
#include "splitsession.h"
#include "DrawableManager.h"
#include "Layer.h"
#include "ScreenWideNavBar.h"
#include "Canvas.h"
#include "EventCallbackListener.h"

struct NET_StreamSocket;

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

enum EditorTouchMode : int {
    TOUCHMODE_PAN = 0,
    TOUCHMODE_LEFTCLICK = 1,
    TOUCHMODE_RIGHTCLICK = 2,
    TOUCHMODE_MAX
};

#define UNDOSTACK_LAYER_DATA_MODIFIED 0
#define UNDOSTACK_CREATE_LAYER 1
#define UNDOSTACK_DELETE_LAYER 2
#define UNDOSTACK_MOVE_LAYER 3
#define UNDOSTACK_ADD_COMMENT 4
#define UNDOSTACK_REMOVE_COMMENT 5
#define UNDOSTACK_SET_OPACITY 6
#define UNDOSTACK_RESIZE_LAYER 7
#define UNDOSTACK_ALL_LAYER_DATA_MODIFIED 8
#define UNDOSTACK_CREATE_LAYER_VARIANT 9
#define UNDOSTACK_DELETE_LAYER_VARIANT 10
struct UndoStackElement {
    Layer* targetlayer = NULL;
    uint32_t type = 0;
    int extdata = 0;
    int extdata2 = 0;
    std::string extdata3 = "";
    void* extdata4 = NULL;
};

struct UndoStackResizeLayerElement {
    XY oldDimensions;
    std::vector<LayerVariant> oldLayerData;
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

#define FRAGMENT_DIRECTION_UP 0b0001
#define FRAGMENT_DIRECTION_DOWN 0b0010
#define FRAGMENT_DIRECTION_LEFT 0b0100
#define FRAGMENT_DIRECTION_RIGHT 0b1000
struct IsolatedFragmentPoint {
    XY onCanvasPixelPosition;
    u8 directions = 0;
    u8 directions2x = 0;
    u8 directions4x = 0;
};

struct NetworkCanvasClientInfo {
    u32 uid = -1;
    std::string clientName = "User";
    XY cursorPosition = {0,0};
    u64 lastReportTime = 0;
    u32 clientColor = 0xC0E1FF;
};

/*struct Frame {
    std::vector<Layer*> layers;
    std::vector<CommentData> comments;
};*/

class MainEditor : public BaseScreen, public EventCallbackListener
{
protected:
    std::vector<IsolatedFragmentPoint> renderedIsolatedFragmentPoints;

    MainEditor() {}
public:
    bool isPalettized = false;

    MainEditorCommentMode commentViewMode = COMMENTMODE_SHOW_HOVERED;
    std::vector<CommentData> comments;

    //std::vector<Frame*> frames;

    std::vector<Layer*> layers;
    int selLayer = 0;

    std::vector<UndoStackElement> undoStack, redoStack;

    XY tileDimensions = XY{ 0,0 };
    u8 tileGridAlpha = 0x40;
    XY tileGridPaddingBottomRight = XY{ 0,0 };
    SDL_Rect getPaddedTilePosAndDimensions(XY tilePos);
    XY getPaddedTileDimensions();

    Canvas canvas;

    XY mousePixelTargetPoint = XY{0,0};
    XY mousePixelTargetPoint2xP = XY{ 0,0 };
    XY mouseHoldPosition = XY{ 0,0 };
    bool closeNextTick = false;
    BaseBrush* currentBrush = NULL;
    bool currentBrushMouseDowned = false;
    bool invertPattern = false;
    Pattern* currentPattern = NULL;
    bool leftMouseHold = false;
    bool middleMouseHold = false;
    Timer64 layerSwitchTimer;
    Timer64 variantSwitchTimer;
    bool lastVariantSwitchWasRight = false;
    Timer64 undoTimer;
    bool lastUndoWasRedo = false;
    bool hideUI = false;
    bool penDown = false;
    bool penAltButtonDown = false;
    double penPressure = 1.0f;

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

    ScreenWideNavBar* navbar;
    EditorColorPicker* colorPicker;
    EditorBrushPicker* brushPicker;
    EditorLayerPicker* layerPicker;

    SDL_Color backgroundColor = SDL_Color{ 0,0,0,255 };

    XY symmetryPositions = { 0,0 };
    XY guidelinePosition = { 0,0 };
    std::vector<Guideline> guidelines;

    bool symmetryEnabled[2] = { false, false };

    bool isolateEnabled = false;
    ScanlineMap isolatedFragment;

    std::map<SDL_Scancode, NavbarSection> mainEditorKeyActions;

    std::vector<uint32_t> lastColors;

    SplitSessionData splitSessionData = { false };

    std::vector<BaseScreen*> hintOpenScreensInInteractiveMode;

    std::map<std::string, double> toolProperties;
    Panel* toolPropertiesPanel = NULL;

    EditorTouchToggle* touchModePanel = NULL;
    EditorTouchMode touchMode = TOUCHMODE_PAN;

    u64 editTime = 0;
    u64 lastTimestamp = -1;

    bool shouldUpdateRenderedIsolatedFragmentPoints = false;

    Timer64 autosaveTimer;

    u32 canvasStateID;
    std::atomic<bool> networkRunning = false;
    std::thread* networkCanvasThread = NULL;
    std::mutex networkClientsListMutex;
    std::vector<NetworkCanvasClientInfo*> networkClients;
    std::vector<std::thread*> networkCanvasResponderThreads;
    NetworkCanvasClientInfo* thisClientInfo = NULL;
    std::atomic<int> nextClientUID = 0;

    MainEditor(XY dimensions);
    MainEditor(SDL_Surface* srf);
    MainEditor(Layer* srf);
    MainEditor(std::vector<Layer*> layers);
    ~MainEditor();

    void render() override;
    void tick() override;
    void takeInput(SDL_Event evt) override;

    std::string getName() override { return TL("vsp.maineditor") + (lastConfirmedSave ? ": " + fileNameFromPath(convertStringToUTF8OnWin32(lastConfirmedSavePath)) : std::string("")); }
    bool takesTouchEvents() override { return true; }

    void eventFileSaved(int evt_id, PlatformNativePathString name, int exporterId) override;
    void eventPopupClosed(int evt_id, BasePopup* p) override;
    void eventColorSet(int evt_id, uint32_t color) override;
    void eventFileOpen(int evt_id, PlatformNativePathString name, int importerId) override;

    void DrawBackground();
    void DrawForeground();
    void renderComments();
    void renderUndoStack();
    void drawSymmetryLines();
    void evalIsolatedFragmentRender();
    void drawIsolatedFragment();
    void drawTileGrid();
    void renderGuidelines();
    void drawSplitSessionFragments();
    void drawZoomLines();
    void drawRowColNumbers();
    virtual void drawNetworkCanvasClients();

    void inputMouseRight(XY at, bool down);

    void focusOnColorInputTextBox();
    void initLayers();
    virtual void setUpWidgets();
    void makeActionBar();
    void initToolParameters();
    void addWidget(Drawable* wx);
    void removeWidget(Drawable* wx);
    void RecalcMousePixelTargetPoint(int x, int y);
    void FillTexture();
    virtual void SetPixel(XY position, uint32_t color, bool pushToLastColors = true, uint8_t symmetry = 0);
    void DrawLine(XY from, XY to, uint32_t color);
    void copyImageToClipboard();
    void copyLayerToClipboard(Layer* l);
    virtual void trySaveImage();
    virtual bool trySaveWithExporter(PlatformNativePathString name, FileExporter* exporter);
    virtual void trySaveAsImage();
    std::map<std::string, std::string> makeSingleLayerExtdata();
    void loadSingleLayerExtdata(Layer* l);
    std::string makeCommentDataString();
    static std::vector<CommentData> parseCommentDataString(std::string data);
    void recenterCanvas();
    bool requestSafeClose();
    void zoom(int how_much);
    bool isInBounds(XY pos);
    virtual uint32_t pickColorFromAllLayers(XY);
    void regenerateLastColors();
    virtual void setActiveColor(uint32_t);
    virtual uint32_t getActiveColor();
    virtual void playColorPickerVFX(bool inward);
    void setActiveBrush(BaseBrush* b);
    void tickAutosave();
    bool usingAltBG();
    void setAltBG(bool useAltBG);
    void tryAddReference(PlatformNativePathString path);
    void tryToggleTilePreviewLockAtMousePos();

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
    Layer* getCurrentLayer() { return layers[selLayer]; }
    int indexOfLayer(Layer* l);
    void layer_setOpacity(uint8_t alpha);
    void layer_promptRename();
    void layer_flipHorizontally();
    void layer_flipVertically();
    uint32_t layer_getPixelAt(XY pos);
    void layer_setAllAlpha255();
    void layer_replaceColor(uint32_t from, uint32_t to);
    void layer_hsvShift(hsv shift);
    void layer_outline(bool wholeImage = false);
    void layer_clearSelectedArea();
    void layer_selectCurrentAlpha();
    void layer_fillActiveColor();
    virtual Layer* flattenImage();
    virtual Layer* mergeLayers(Layer* bottom, Layer* top);
    void flipAllLayersOnX();
    void flipAllLayersOnY();
    void rescaleAllLayersFromCommand(XY size);
    void resizeAllLayersFromCommand(XY size, bool byTile = false);
    void resizzeAllLayersByTilecountFromCommand(XY size);
    void integerScaleAllLayersFromCommand(XY scale, bool downscale = false);
    MainEditorPalettized* toPalettizedSession();
    void tryExportPalettizedImage();
    virtual void exportTilesIndividually();

    void startNetworkSession();
    virtual void networkCanvasStateUpdated(int whichLayer);
    void networkCanvasServerThread();
    void networkCanvasServerResponderThread(NET_StreamSocket* clientSocket);
    void networkCanvasProcessCommandFromClient(std::string command, NET_StreamSocket* clientSocket, NetworkCanvasClientInfo* clientInfo);
    std::string networkReadCommand(NET_StreamSocket* socket);
    bool networkReadCommandIfAvailable(NET_StreamSocket* socket, std::string& outCommand);
    void networkSendCommand(NET_StreamSocket* socket, std::string commandName);
    bool networkReadBytes(NET_StreamSocket* socket, u8* buffer, u32 count);
    void networkSendBytes(NET_StreamSocket* socket, u8* buffer, u32 count);
    void networkSendString(NET_StreamSocket* socket, std::string s);
    void networkCanvasSendLRDT(NET_StreamSocket* socket, int index, Layer* l);
    std::string networkReadString(NET_StreamSocket* socket);
    void endNetworkSession();

    void layer_newVariant();
    void layer_duplicateVariant();
    void layer_removeVariant(Layer* layer, int variantIndex);
    void layer_switchVariant(Layer* layer, int variantIndex);
    void layer_promptRenameCurrentVariant();

    bool canAddCommentAt(XY a);
    void addComment(CommentData c);
    void addCommentAt(XY a, std::string c);
    void removeCommentAt(XY a);
private:
    CommentData _removeCommentAt(XY a);
};

