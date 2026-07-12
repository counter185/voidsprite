#pragma once
#include "BaseScreen.h"
#include "mathops.h"
#include "PanelUserInteractable.h"

class ScreenCubemapPreview;

struct Vertex {
    XYZd pos;
    XYd uv;
};

struct Quad {
    Vertex topLeft, topRight, bottomLeft, bottomRight;
};

class TesellatedQuad {
private:
    ScreenCubemapPreview* parent;
public:
    Quad baseQuad;

    std::vector<std::pair<std::vector<SDL_Vertex>, std::vector<int>>> screenSpaceDrawCalls;
    //std::vector<Vertex> screenSpaceVtx;
    //std::vector<int> indexBuffer;

    int count = 1;
    int maxCount = 40;
    int framesWait = 0;

    TesellatedQuad(ScreenCubemapPreview* caller, Quad base);

    void render();
    void reset();

    void updateScreenSpaceVtx();
};

class PanelCubemapPreview : public PanelUserInteractable {
private:
    ScreenCubemapPreview* parent;
public:
    PanelCubemapPreview(ScreenCubemapPreview* caller);
};

class ScreenCubemapPreview :
    public BaseScreen
{
private:
public:
    TesellatedQuad* qFront, *qBack, *qLeft, *qRight, *qTop, *qBottom;
    MainEditor* parent = NULL;
    matrix viewMatrix;
    matrix projection;

    double posX = 0, posY = 0, posZ = 0;
    double rotX = 0, rotY = 0, rotZ = 0;

    double fov = 100;
    double nearPlane = 0.5;
    double farPlane = 200;

    bool mouseDrag = false;

    ScreenCubemapPreview(MainEditor* caller);
    ~ScreenCubemapPreview();

    void render() override;
    void defaultInputAction(SDL_Event evt) override;
    bool takesTouchEvents() override { return true; }
    std::string getName() { return "Cubemap preview"; }
    BaseScreen* isSubscreenOf() override { return (BaseScreen*)parent; }
    void screenResized(XY from, XY to) override { resetAllQuads(); }

    XYZd intersectViewSpace(XYZd a, XYZd b);
    Vertex intersectTexturedViewSpace(Vertex a, Vertex b);
    std::vector<Vertex> clipViewSpaceTriangle(std::vector<Vertex> points);
    matrix makeViewMatrix();
    matrix makeProjectionMatrix();
    XYZWd calcViewSpace(XYZd worldSpace);
    matrix calcClipSpace(XYZWd viewSpace);
    XYZd calcScreenSpace(matrix clipSpace);
    XYZd worldPointToScreenPoint(XYZd worldPos);

    void renderScene();
    void renderTriangle(std::vector<XYZd> worldSpacePoints);
    void renderQuad(std::vector<XYZd> worldSpacePoints);
    void renderTexturedTriangle(std::vector<Vertex> worldSpacePoints, SDL_Texture* tex);
    void renderViewSpaceTriangle(std::vector<Vertex> viewMt, SDL_Texture* tex, XYZd* screenPointCache = NULL);
    void renderTexturedQuad(std::vector<Vertex> worldSpacePoints, SDL_Texture* tex);
    void renderMultipleTexturedQuads(std::vector<Quad> q, SDL_Texture* tex);
    void renderTesellatedTexturedQuad(std::vector<std::vector<Vertex>>& q, SDL_Texture* tex);

    std::vector<Quad> tesellateQuad(Quad q);
    std::vector<Quad> tesellateQuad(Quad q, int iterations);
    std::vector<std::vector<Vertex>> tesellateQuadV2(Quad q, int splitsW = 10, int splitsH = 10);

    void resetAllQuads();

    bool quadInView(Quad q);
    std::pair<std::vector<SDL_Vertex>, std::vector<int>> evalTriangleScreenPoints(std::vector<Vertex> viewMt);
};

