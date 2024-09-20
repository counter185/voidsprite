#pragma once
#include "TilemapPreviewScreen.h"
#include "EventCallbackListener.h"
#include "ScreenWideNavBar.h"

class RPG2KTilemapPreviewScreen :
    public BaseScreen, EventCallbackListener
{
public:
    MainEditor* caller = NULL;
    XY dimensions = XY{ 20,15 };
    uint16_t* lowerLayerData = NULL;
    uint16_t* upperLayerData = NULL;
    ScreenWideNavBar<RPG2KTilemapPreviewScreen*>* navbar = NULL;
    DrawableManager wxsManager;

    uint8_t gridOpacity = 0x18;
    int scale = 1;
    XY canvasDrawPoint = { 40,40 };
    bool scrollingTilemap = false;

    RPG2KTilemapPreviewScreen(MainEditor* parent);
    ~RPG2KTilemapPreviewScreen();

    void render() override;
    void tick() override;
    void takeInput(SDL_Event evt) override;
    std::string getName() override { return "Preview RPG2K map"; }

    void eventFileOpen(int evt_id, PlatformNativePathString path, int importer_index) override;

    uint16_t lowerLayerTileAt(XY position);
    bool isDeepWaterTileAt(XY position);

    void RenderWaterTile(uint8_t connection, uint16_t watertileIndex, XY position, SDL_Rect dst, SDL_Texture* tex);
    void RenderAutoTile(uint8_t connection, uint16_t autotileIndex, SDL_Rect dst, SDL_Texture* tex);
    void RenderRPG2KTile(uint16_t tile, XY position, SDL_Rect dst);
    void LoadLMU(PlatformNativePathString path);
};

