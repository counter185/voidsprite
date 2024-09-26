#pragma once
#include "TilemapPreviewScreen.h"
#include "EventCallbackListener.h"
#include "ScreenWideNavBar.h"

struct LMUEvent {
    XY pos;
    std::string name = "";
    std::string texFileName = "";
    int charsetIndex = 0, charsetDirection = 0, charsetPattern = 1;
    SDL_Texture* tex = NULL;
};

enum LMUEventViewMode : int {
    LMUEVENTS_HIDE_ALL = 0,
    LMUEVENTS_SHOW_INGAME = 1,
    LMUEVENTS_SHOW_RECTS = 2,
    LMUEVENTS_SHOW_INGAME_AND_RECTS = 3
};

class RPG2KTilemapPreviewScreen :
    public BaseScreen, EventCallbackListener
{
public:
    MainEditor* caller = NULL;
    XY dimensions = XY{ 20,15 };
    uint16_t* lowerLayerData = NULL;
    uint16_t* upperLayerData = NULL;
    ScreenWideNavBar<RPG2KTilemapPreviewScreen*>* navbar = NULL;
    std::vector<LMUEvent> events;

    std::map<std::string, SDL_Texture*> texturesLoaded;
    SDL_Texture* callerCanvas = NULL;

    bool rdLowerLayer = true, rdUpperLayer = true, rdEventLayer = true;
    uint8_t gridOpacity = 0x18;
    int scale = 1;
    XY canvasDrawPoint = { 40,40 };
    bool scrollingTilemap = false;
    LMUEventViewMode eventViewMode = LMUEVENTS_SHOW_INGAME_AND_RECTS;

    RPG2KTilemapPreviewScreen(MainEditor* parent);
    ~RPG2KTilemapPreviewScreen();

    void render() override;
    void tick() override;
    void takeInput(SDL_Event evt) override;
    std::string getName() override { return "Preview RPG2K map"; }
    BaseScreen* isSubscreenOf() override;

    void eventFileOpen(int evt_id, PlatformNativePathString path, int importer_index) override;
    void eventFileSaved(int evt_id, PlatformNativePathString path, int exporter_index) override;

    uint16_t lowerLayerTileAt(XY position);
    bool isDeepWaterTileAt(XY position);

    void RecenterCanvas();
    void PrerenderCanvas();
    void RenderWaterTile(uint8_t connection, uint16_t watertileIndex, XY position, SDL_Rect dst, SDL_Texture* tex, int animState = -1);
    void RenderAutoTile(uint8_t connection, uint16_t autotileIndex, SDL_Rect dst, SDL_Texture* tex);
    void RenderRPG2KTile(uint16_t tile, XY position, SDL_Rect dst, int animState = -1);
    void RenderEvents(XY originPoint, int canvasScale);
    void RenderWholeMap(XY at, int scale, bool rdLowerLayer = true, bool rdUpperLayer = true, bool rdEvents = true);
    Layer* RenderWholeMapToTexture();
    void LoadLMU(PlatformNativePathString path);
};

