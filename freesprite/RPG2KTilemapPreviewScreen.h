#pragma once
#include "TilemapPreviewScreen.h"
#include "EventCallbackListener.h"
#include "ScreenWideNavBar.h"

class RPG2KTilemapPreviewScreen :
    public BaseScreen, EventCallbackListener
{
public:
    MainEditor* caller = NULL;
    int16_t* lowerLayerData = NULL;
    int16_t* upperLayerData = NULL;
    ScreenWideNavBar<RPG2KTilemapPreviewScreen*>* navbar = NULL;
    DrawableManager wxsManager;

    RPG2KTilemapPreviewScreen(MainEditor* parent);
    ~RPG2KTilemapPreviewScreen();

    void render() override;
    void tick() override;
};

