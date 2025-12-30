#pragma once
#include <mutex>
#include <thread>

#include "globals.h"
#include "BaseScreen.h"
#include "splitsession.h"
#include "Layer.h"
#include "Canvas.h"
#include "EventCallbackListener.h"
#include "operation_queue.h"
#include "Panel.h"
#include "PanelUserInteractable.h"

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

struct CommentData {
    XY position;
    std::string data;
    Timer64 animTimer;
    bool hovered = false;
};

class Guideline {
public:
    bool vertical;
    int position;

    std::string Serialize() {
        return frmt("{}-{}", vertical ? "v" : "h", position);
    }
    static Guideline Deserialize(std::string str) {
        Guideline g;
        auto splt = splitString(str, '-');
        if (splt.size() != 2) {
            throw std::runtime_error("Invalid guideline string");
        }
        else {
            g.vertical = (splt[0] == "v");
            g.position = std::stoi(splt[1]);
            return g;
        }
    }
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
    std::string clientIP = "??";
    XY cursorPosition = {0,0};
    u64 lastReportTime = 0;
    u32 clientColor = 0xC0E1FF;
    bool chatTyping = false;

    //host hints
    bool hostKick = false;
};

struct NetworkCanvasChatMessage {
    std::string fromName;
    u32 fromColor = 0xFFFFFF;
    std::string message;
    u32 messageColor = 0xFFFFFF;
    u64 timestamp = 0;
};
class NetworkCanvasChatState {
public:
    std::mutex messagesMutex;
    std::vector<NetworkCanvasChatMessage> messages;
    std::atomic<u32> messagesState = 0;

    virtual ~NetworkCanvasChatState() = default;

    void fromJson(std::string jsonStr);
};
class NetworkCanvasChatHostState : public NetworkCanvasChatState {
protected:
    void nextState() {
        messagesState++;
    }
public:
    void newMessage(NetworkCanvasChatMessage msg);
    std::string toJson();
};

class Frame {
public:
    std::vector<Layer*> layers;
    int activeLayer = 0;
    //std::vector<CommentData> comments;

    ~Frame() {
        for (Layer* l : layers) {
            delete l;
        }
    }

    Frame* copy() {
        Frame* f = new Frame();
        *f = *this;
        f->layers.clear();
        for (Layer*& l : layers) {
            f->layers.push_back(l->copyCurrentVariant());
        }
        return f;
    }
};

class EditorNetworkCanvasChatPanel : public PanelUserInteractable {
protected:
    MainEditor* parent = NULL;
    ScrollingPanel* chatMsgPanel = NULL;
    bool clientSide = false;
public:
    EditorNetworkCanvasChatPanel(MainEditor* caller, bool clientSide = false);

    void updateChat();
};

class EditorNetworkCanvasHostPanel : public PanelUserInteractable {
protected:
    MainEditor* parent = NULL;
    ScrollingPanel* clientList = NULL;
    bool clientSide = false;
public:
    EditorNetworkCanvasHostPanel(MainEditor* caller, bool clientSide = false);

    void updateClientList();
};

struct CompactEditorSection {
    Panel* targetPanel;
    ReldTex* icon = NULL;
};

class MainEditor : public BaseScreen, public EventCallbackListener
{
protected:
    std::vector<IsolatedFragmentPoint> renderedIsolatedFragmentPoints;
    bool compactEditor = false;
    std::vector<PanelReference*> openReferencePanels;
    //std::vector<Layer*> layers;

    MainEditor() {}
public:
    bool isPalettized = false;

    OperationQueue mainThreadOps;

    MainEditorCommentMode commentViewMode = COMMENTMODE_SHOW_HOVERED;
    std::vector<CommentData> comments;

    int activeFrame = 0;
    std::vector<Frame*> frames = { new Frame() };
    Timer64 frameAnimationStartTimer;
    bool frameAnimationPlaying = false;
    int frameAnimMSPerFrame = 200;

    int selLayer = 0;

    std::vector<UndoStackElementV2*> undoStack, redoStack;

    XY tileDimensions = XY{ 0,0 };
    u8 tileGridAlpha = 0x40;
    XY tileGridPaddingBottomRight = XY{ 0,0 };
    SDL_Rect getPaddedTilePosAndDimensions(XY tilePos);
    XY getPaddedTileDimensions();

    Canvas canvas;

    XY mousePixelTargetPoint = XY{0,0};
    XY mousePixelTargetPoint2xP = XY{ 0,0 };
    XY mouseHoldPosition = XY{ 0,0 };
    XY mouseHoldPosition2xP = XY{ 0,0 };
    bool closeNextTick = false;
    BaseBrush* currentBrush = NULL;
    bool currentBrushMouseDowned = false;
    bool invertPattern = false;
    Pattern* currentPattern = NULL;
    Timer64 leftMouseReleaseTimer;
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
    ScreenWideActionBar* actionbar;
    EditorColorPicker* colorPicker;
    EditorBrushPicker* brushPicker;
    EditorLayerPicker* layerPicker;
    EditorFramePicker* framePicker;

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
    Timer64 timerSinceLastSave;

    u32 canvasStateID;
    std::atomic<bool> networkRunning = false;
    std::thread* networkCanvasThread = NULL;
    std::mutex networkClientsListMutex;
    std::vector<NetworkCanvasClientInfo*> networkClients;
    std::vector<std::thread*> networkCanvasResponderThreads;
    NetworkCanvasClientInfo* thisClientInfo = NULL;
    std::atomic<int> nextClientUID = 0;
    EditorNetworkCanvasHostPanel* networkCanvasHostPanel = NULL;
    EditorNetworkCanvasChatPanel* networkCanvasChatPanel = NULL;
    NetworkCanvasChatState* networkCanvasCurrentChatState = NULL;
    int networkCanvasPort = -1;
    std::string networkCanvasPassword = "";
    bool networkCanvasBroadcastRPC = false;
    std::string networkCanvasLobbyID = "";
    bool networkCanvasRPCPrivate = false;
    std::string networkCanvasRPCAddress = "";
    Timer64 networkCanvasLastLANBroadcast;

    MainEditor(XY dimensions);
    MainEditor(SDL_Surface* srf);
    MainEditor(Layer* srf);
    MainEditor(std::vector<Layer*> layers);
    MainEditor(std::vector<Frame*> frames);
    ~MainEditor();

    void render() override;
    void tick() override;
    void takeInput(SDL_Event evt) override;
    void dropEverythingYoureDoingAndSave() override;

    std::string getName() override { 
        return TL("vsp.maineditor") 
            + ((lastConfirmedSave || splitSessionData.set) ? ": " + fileNameFromPath(convertStringToUTF8OnWin32(lastConfirmedSavePath))
                : std::string("")); 
    }
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
    void SetupCompactEditor(std::vector<CompactEditorSection> createSections);
    void openTouchModePanel();
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
    virtual void tickAutosave();
    void createRecoveryAutosave(std::string insertIntoFilename = "");
    bool usingAltBG();
    void setAltBG(bool useAltBG);
    bool tryAddReference(PlatformNativePathString path);
    void openPreviewPanel();
    void tryToggleTilePreviewLockAtMousePos();
    void promptPasteImageFromClipboard();
    void promptPasteImage(Layer* l);

    void discardEndOfUndoStack();
    void checkAndDiscardEndOfUndoStack();
    void commitStateToLayer(Layer* l);
    void commitStateToCurrentLayer();
    void addToUndoStack(UndoStackElementV2* undo);
    void discardUndoStack();
    void discardRedoStack();
    void undo();
    void redo();

    Frame* getCurrentFrame() { return frames[activeFrame]; }
    void newFrame();
    void duplicateFrame(int index);
    void deleteFrame(int index);
    void switchFrame(int index);
    void moveFrameLeft(int index);
    void moveFrameRight(int index);
    void toggleFrameAnimation();
    void setMSPerFrame(int ms);

    std::vector<Layer*>& getLayerStack() { return getCurrentFrame()->layers; }
    Layer* layerAt(int index);
    virtual Layer* newLayer();
    virtual void deleteLayer(int index);
    virtual void moveLayerUp(int index);
    virtual void moveLayerDown(int index);
    virtual void mergeLayerDown(int index);
    virtual void duplicateLayer(int index);
    void switchActiveLayer(int index);
    Layer* getCurrentLayer() { return getLayerStack()[selLayer]; }
    int indexOfLayer(Layer* l);
    void layer_setOpacity(uint8_t alpha);
    void layer_promptRename(int index);
    void layer_promptRenameCurrent();
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

    virtual void rescaleAllLayersFromCommand(XY size);
    //todo: split byTile into two functions
    virtual void resizeAllLayersFromCommand(XY size, bool byTile = false);
    virtual void resizzeAllLayersByTilecountFromCommand(XY size);
    virtual void resizeAllLayersReorderingTilesFromCommand(XY size);
    virtual void integerScaleAllLayersFromCommand(XY scale, bool downscale = false);
    MainEditorPalettized* toPalettizedSession();
    void tryExportPalettizedImage();
    virtual void exportTilesIndividually();

    std::string networkGetSocketAddress(NET_StreamSocket* sock);
    virtual void promptStartNetworkSession();
    virtual void networkCanvasStateUpdated(int whichLayer);
    void networkCanvasServerThread(PopupSetNetworkCanvasData startData);
    void networkCanvasServerResponderThread(NET_StreamSocket* clientSocket);
    void networkCanvasProcessCommandFromClient(std::string command, NET_StreamSocket* clientSocket, NetworkCanvasClientInfo* clientInfo);
    bool networkCanvasProcessAUTHCommand(std::string request);
    std::string networkReadCommand(NET_StreamSocket* socket);
    bool networkReadCommandIfAvailable(NET_StreamSocket* socket, std::string& outCommand);
    void networkSendCommand(NET_StreamSocket* socket, std::string commandName);
    bool networkReadBytes(NET_StreamSocket* socket, u8* buffer, u32 count);
    void networkSendBytes(NET_StreamSocket* socket, u8* buffer, u32 count);
    void networkSendString(NET_StreamSocket* socket, std::string s);
    void networkCanvasSendLRDT(NET_StreamSocket* socket, int index, Layer* l);
    std::string networkReadString(NET_StreamSocket* socket);
    void networkCanvasKickUID(u32 uid);
    void networkCanvasSystemMessage(std::string msg);
    virtual void networkCanvasChatSendCallback(std::string content);
    void networkCanvasBroadcastToLAN();
    void endNetworkSession();

    void layer_newVariant();
    void layer_duplicateActiveVariant();
    void layer_duplicateActiveVariant(Layer* layer);
    void layer_duplicateVariant(Layer* layer, int variantIndex);
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

