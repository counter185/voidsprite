#pragma once
#include "globals.h"
#include "BaseScreen.h"
#include "EventCallbackListener.h"
#include "Canvas.h"
#include "ScreenWideNavBar.h"

class MinecraftBlockPreviewScreen : public BaseScreen, EventCallbackListener
{
private:
    void mapRectToVerts(SDL_Vertex* verts, std::vector<int> indices, SDL_Rect r);
protected:
    bool closeNextTick = false;

    PanelMCBlockPreview* panelBig;
    PanelMCBlockPreview* panelSmalm;

    ScreenWideNavBar* navbar = NULL;
public: 
    MainEditor* caller;
    Canvas canvas;

    XY tileTop = {-1,-1};
    XY tileSideLeft = {-1,-1};
    XY tileSideRight = {-1,-1};

    int choosingSide = 0;
    float isometricBlockScale = 5;
    double isomAlpha = 35.264;
    double isomBeta = 45;
    bool shadeSides = true;

    bool scrollingCanvas = false;

    MinecraftBlockPreviewScreen(MainEditor* parent);
    ~MinecraftBlockPreviewScreen();

    void render() override;
    void RenderCanvas();
    void takeInput(SDL_Event evt) override;
    void tick() override;
    BaseScreen* isSubscreenOf() override;

    std::string getName() override { return TL("vsp.cubepreview"); }

    void eventPopupClosed(int evt_id, BasePopup* target) override;

    void renderToWorkspace(XY wh);
    void drawBackground();
    void drawIsometricBlockV2(SDL_Rect at);

    XYd projectPointIsom(double x, double y, double z, double alpha, double beta);
};

