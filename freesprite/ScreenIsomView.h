#pragma once

#include "BaseScreen.h"

class ScreenIsomView : public BaseScreen {
protected:
    double rotAlpha = 35.264;
    double rotBeta = 45;
    double size = 20;
    bool drawWireframe = false;
    double pointScale = 1;
    XY screen00 = { 0,0 };

    bool shade = true;
    double shadeRight = 0.36;
    double shadeFront = 0.12;
    double shadeLeft = 0.50;
    double shadeBack = 0.60;
    double shadeBottom = 0.70;
    double shadeTop = 0;
public:

    void renderQuadFromEditorContent(MainEditor* editor, XY origin00, double scale, XYZd ul, XYZd ur, XYZd dl, XYZd dr, SDL_Rect texture, double shading = 0.0);
    void renderBoxFromEditorContent(MainEditor* editor, XY origin00, double scale, XYZd at, double sizeX, double sizeZ, double sizeY, XY textureBoxOrigin, double offset, bool flipUVX);

    void renderQuad(SDL_Color color, XY origin00, double scale, XYZd ul, XYZd ur, XYZd dl, XYZd dr, double shading = 0.0);
    void renderBox(SDL_Color color, XY origin00, double scale, XYZd at, double sizeX, double sizeZ, double sizeY, double offset);

    void rotateFromMouseInput(double xrel, double yrel);

    void renderFloorGrid();
    XYd worldSpaceToScreenSpace(XYZd point, double alpha, double beta);
    XYZd screenSpaceToWorldSpace(XYd point, double alpha, double beta);
    XY scaledPoint(XY point);

    SDL_Rect uvFlipHorizontal(SDL_Rect x);
};