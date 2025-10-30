#pragma once
#include "BaseScreen.h"
#include "PanelUserInteractable.h"

class MinecraftSkinPreviewScreen;

class PanelMCSkinPreview : public PanelUserInteractable {
private:
    bool dragging = false;
public:
    MinecraftSkinPreviewScreen* parent;

    PanelMCSkinPreview(MinecraftSkinPreviewScreen* caller);
    void renderAfterBG(XY at) override;
    SDL_Rect getPreviewAreaRect();
    bool defaultInputAction(SDL_Event evt, XY at) override;
};

class MinecraftSkinPreviewScreen :
    public BaseScreen
{
private:
    MainEditor* caller;
    PanelMCSkinPreview* inEditorPanel = NULL;

    double rotAlpha = 35.264;
    double rotBeta = 45;
    double size = 20;
    bool drawWireframe = false;
    XY screen00 = { 0,0 };

    double pointScale = 1;
protected:
    int dragging = 0;
    bool slimModel = false;
    bool twoByOneSkin = false;

    bool shade = true;
    double shadeRight = 0.36;
    double shadeFront = 0.12;
    double shadeLeft = 0.50;
    double shadeBack = 0.60;
    double shadeBottom = 0.70;
    double shadeTop = 0;

    ScreenWideNavBar* navbar;
public:

    MinecraftSkinPreviewScreen(MainEditor* parent);
    ~MinecraftSkinPreviewScreen();

    void render() override;
    void takeInput(SDL_Event evt) override;
    void tick() override;
    void recalcPointScale();
    void rotateFromMouseInput(double xrel, double yrel);
    BaseScreen* isSubscreenOf() override;

    std::string getName() override { return "Preview MC skin"; }

    void renderQuad(XY origin00, double scale, XYZd ul, XYZd ur, XYZd dl, XYZd dr, SDL_Rect texture, double shading = 0.0);
    void renderBox(XY origin00, double scale, XYZd at, double sizeX, double sizeZ, double sizeY, XY textureBoxOrigin, double offset, bool flipUVX);
    XYd worldSpaceToScreenSpace(XYZd point, double alpha, double beta);
    XY scaledPoint(XY point);

    void debugRenderAxes();
    void renderModel(XY origin00, double scale);
    void renderFloorGrid();
    SDL_Rect getModelRenderArea(XY);
    SDL_Rect uvFlipHorizontal(SDL_Rect x);

    static bool dimensionsValidForPreview(XY size) {
        return size.x > 0 && size.y > 0
            && size.x % 8 == 0  //allowing downscale up to 1/4
            && (size.x == size.y || size.x == (2 * size.y));
    }
};

