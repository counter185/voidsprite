#pragma once
#include "BaseScreen.h"

class MinecraftSkinPreviewScreen :
    public BaseScreen
{
private:
    MainEditor* caller;

    double rotAlpha = 35.264;
    double rotBeta = 45;
    double size = 20;
    XY screen00 = { 0,0 };
protected:
    int dragging = 0;
    bool slimModel = false;

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

    void render() override;
    void takeInput(SDL_Event evt) override;
    BaseScreen* isSubscreenOf() override;

    std::string getName() override { return "Preview MC skin"; }

    void renderQuad(XYZd ul, XYZd ur, XYZd dl, XYZd dr, SDL_Rect texture, double shading = 0.0);
    void renderBox(XYZd at, double sizeX, double sizeZ, double sizeY, XY textureBoxOrigin);
    void renderBoxOffset(XYZd at, double sizeX, double sizeZ, double sizeY, XY textureBoxOrigin, double offset);
    XYd worldSpaceToScreenSpace(XYZd point, double alpha, double beta);

    void debugRenderAxes();
    void renderFloorGrid();
};

