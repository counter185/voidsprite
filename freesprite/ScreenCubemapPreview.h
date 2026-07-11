#pragma once
#include "BaseScreen.h"
#include "mathops.h"

struct Vertex {
    XYZd pos;
    XYd uv;
};

struct Quad {
    Vertex topLeft, topRight, bottomLeft, bottomRight;
};

class ScreenCubemapPreview :
    public BaseScreen
{
private:
    MainEditor* parent = NULL;
    matrix viewMatrix;
    matrix projection;
public:
    double posX = 0, posY = 0, posZ = 0;
    double rotX = 0, rotY = 0, rotZ = 0;

    double fov = 100;
    double nearPlane = 0.5;
    double farPlane = 200;

    bool mouseDrag = false;

    XYZd intersectViewSpace(XYZd a, XYZd b);
    Vertex intersectTexturedViewSpace(Vertex a, Vertex b);
    std::vector<Vertex> clipViewSpaceTriangle(std::vector<Vertex> points);
    matrix makeViewMatrix();
    matrix makeProjectionMatrix();
    XYZWd calcViewSpace(XYZd worldSpace);
    matrix calcClipSpace(XYZWd viewSpace);
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
    void renderTexturedTriangle(std::vector<Vertex> worldSpacePoints, SDL_Texture* tex);
    void renderViewSpaceTriangle(std::vector<Vertex> viewMt, SDL_Texture* tex, XYZd* screenPointCache = NULL);
    void renderTexturedQuad(std::vector<Vertex> worldSpacePoints, SDL_Texture* tex);
    void renderMultipleTexturedQuads(std::vector<Quad> q, SDL_Texture* tex);
    void renderTesellatedTexturedQuad(std::vector<std::vector<Vertex>>& q, SDL_Texture* tex);

    std::vector<Quad> tesellateQuad(Quad q);
    std::vector<Quad> tesellateQuad(Quad q, int iterations);
    std::vector<std::vector<Vertex>> tesellateQuadV2(Quad q);

    bool quadInView(Quad q);
};

