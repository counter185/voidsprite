#pragma once
#include "BaseScreen.h"

class MinecraftSkinPreviewScreen :
    public BaseScreen
{
private:
    MainEditor* caller;

    double rotAlpha = 35.264;
    double rotBeta = 45;
    XY screen00 = { 0,0 };
protected:
    int dragging = 0;
    bool slimModel = true;
public:

    MinecraftSkinPreviewScreen(MainEditor* parent) {
        caller = parent;
        screen00 = { g_windowW / 2, g_windowH / 2 };
    }

    void render() override;
    void takeInput(SDL_Event evt) override;
    BaseScreen* isSubscreenOf() override;

    std::string getName() override { return "Preview MC skin"; }

    void renderQuad(XYZd ul, XYZd ur, XYZd dl, XYZd dr, SDL_Rect texture);
    void renderBox(XYZd at, double sizeX, double sizeZ, double sizeY, XY textureBoxOrigin);
    void renderBoxOffset(XYZd at, double sizeX, double sizeZ, double sizeY, XY textureBoxOrigin, double offset);
    XYd worldSpaceToScreenSpace(XYZd point, double alpha, double beta);

    void debugRenderAxes();
    void renderFloorGrid();
};

