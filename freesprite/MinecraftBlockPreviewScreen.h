#pragma once
#include "globals.h"
#include "BaseScreen.h"
#include "EventCallbackListener.h"
#include "Canvas.h"
#include "ScreenWideNavBar.h"

class MinecraftBlockPreviewScreen : public BaseScreen, EventCallbackListener
{
protected:
    bool closeNextTick = false;

    PanelMCBlockPreview* panelBig;
    PanelMCBlockPreview* panelSmalm;

    ScreenWideNavBar<MinecraftBlockPreviewScreen*>* navbar = NULL;
public: 
    MainEditor* caller;
    Canvas canvas;

    XY tileTop = {-1,-1};
    XY tileSideLeft = {-1,-1};
    XY tileSideRight = {-1,-1};

    int choosingSide = 0;
    float isometricBlockScale = 5;
    bool shadeSides = true;

    bool scrollingCanvas = false;

    MinecraftBlockPreviewScreen(MainEditor* parent);
    ~MinecraftBlockPreviewScreen();

    void render() override;
    void takeInput(SDL_Event evt) override;
    void tick() override;
    BaseScreen* isSubscreenOf() override;

    std::string getName() override { return "Preview cube"; }

    void eventPopupClosed(int evt_id, BasePopup* target) override;

    void renderToWorkspace(XY wh);
    void drawBackground();
    void drawIsometricBlock(SDL_Rect at);
};

