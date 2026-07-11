#pragma once
#include "BaseScreen.h"
#include "mathops.h"

class ScreenCubemapPreview :
    public BaseScreen
{
private:
    MainEditor* parent = NULL;
public:
    double posX = 0, posY = 0, posZ = 0;
    double rotX = 0, rotY = 0, rotZ = 0;

    double fov = 80;
    double nearPlane = 0.5;
    double farPlane = 200;

    bool mouseDrag = false;

    XYZd intersectViewSpace(XYZd a, XYZd b);
    std::vector<XYZd> clipViewSpaceTriangle(std::vector<XYZd> points);
    matrix makeViewMatrix();
    XYZWd calcViewSpace(XYZd worldSpace);
    matrix calcClipSpace(double fovRad, double aspectRatio, XYZWd viewSpace);
    XYZd calcScreenSpace(matrix clipSpace);
    XYZd worldPointToScreenPoint(XYZd worldPos);

    ScreenCubemapPreview(MainEditor* caller)
        : parent(caller) {}

    void render() override;
    void takeInput(SDL_Event evt) override;
    std::string getName() { return "Cubemap preview"; }
    BaseScreen* isSubscreenOf() override { return (BaseScreen*)parent; }

    void renderTriangle(std::vector<XYZd> worldSpacePoints);
    void renderQuad(std::vector<XYZd> worldSpacePoints);
};

