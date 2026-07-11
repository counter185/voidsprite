#include "ScreenCubemapPreview.h"
#include "FontRenderer.h"
#include "TooltipsLayer.h"
#include "maineditor.h"

double degToRad(double deg) {
    return deg * M_PI / 180.0;
}

XYZd ScreenCubemapPreview::intersectViewSpace(XYZd a, XYZd b)
{
    double t = (-nearPlane - a.z) / (b.z - a.z);
    return { a.x + t * (b.x - a.x), a.y + t * (b.y - a.y), -nearPlane };
}

Vertex ScreenCubemapPreview::intersectTexturedViewSpace(Vertex a, Vertex b)
{
    double t = (-nearPlane - a.pos.z) / (b.pos.z - a.pos.z);
    XYZd newPos = { a.pos.x + t * (b.pos.x - a.pos.x), a.pos.y + t * (b.pos.y - a.pos.y), -nearPlane };

    XYd newUV = { a.uv.x + t * (b.uv.x - a.uv.x), a.uv.y + t * (b.uv.y - a.uv.y) };
    return Vertex{ newPos, newUV };
}

std::vector<Vertex> ScreenCubemapPreview::clipViewSpaceTriangle(std::vector<Vertex> points)
{
    std::vector<Vertex> ret;
    for (int i = 0; i < points.size(); i++) {
        Vertex now = points[i];
        Vertex next = points[(i + 1) % 3];

        bool nowInside = now.pos.z <= -nearPlane;
        bool nextInside = next.pos.z <= -nearPlane;

        if (nowInside && nextInside)
        {
            ret.push_back(next);
        }
        else if (nowInside && !nextInside)
        {
            Vertex newVtx = intersectTexturedViewSpace(now, next);
            //newVtx.uv = now.uv;
            ret.push_back(newVtx);
        }
        else if (!nowInside && nextInside)
        {
            ret.push_back(intersectTexturedViewSpace(now, next));
            ret.push_back(next);
        }
    }
    return ret;
}

matrix ScreenCubemapPreview::makeViewMatrix()
{
    double radX = degToRad(rotX);
    double radY = degToRad(rotY);
    double radZ = degToRad(rotZ);

    matrix rx = {
        {1, 0, 0},
        {0, cos(radX), -sin(radX)},
        {0, sin(radX), cos(radX)}
    };

    matrix ry = {
        {cos(radY), 0, sin(radY)},
        {0, 1, 0},
        {-sin(radY), 0, cos(radY)}
    };

    matrix rz = {
        {cos(radZ), -sin(radZ), 0},
        {sin(radZ), cos(radZ), 0},
        {0, 0, 1}
    };

    matrix viewM = matrixTranspose(matrixMultiply(rz, matrixMultiply(ry, rx)));

    matrix pos = { {posX}, {posY}, {posZ} };

    matrix translation = matrixMultiply(viewM, pos);

    matrix V = {
        {viewM[0][0], viewM[0][1], viewM[0][2], translation[0][0]},
        {viewM[1][0], viewM[1][1], viewM[1][2], translation[1][0]},
        {viewM[2][0], viewM[2][1], viewM[2][2], translation[2][0]},
        {0,0,0,1}
    };
    return V;
}

XYZWd ScreenCubemapPreview::calcViewSpace(XYZd worldPos)
{
    //matrix viewMatrix = viewMatrix;//makeViewMatrix();
    matrix viewSpace = matrixMultiply(viewMatrix, { {worldPos.x}, {worldPos.y}, {worldPos.z}, {1} });
    return XYZWd{ viewSpace[0][0], viewSpace[1][0], viewSpace[2][0], viewSpace[3][0] };
}

matrix ScreenCubemapPreview::calcClipSpace(double fovRad, double aspectRatio, XYZWd viewSpace)
{
    double xP = viewSpace.x / viewSpace.z;
    double yP = viewSpace.y / viewSpace.z;

    //viewSpace.x = xP;
    //viewSpace.y = yP;

    matrix projection = {
        {1.0 / (aspectRatio * tan(fovRad / 2)), 0, 0, 0},
        {0, 1.0 / (tan(fovRad / 2)), 0, 0},
        {0, 0, nearPlane, farPlane},
        {0, 0, -1, 0}
    };
    return matrixMultiply(projection, { {viewSpace.x}, {viewSpace.y}, {viewSpace.z}, {viewSpace.w} });
}

XYZd ScreenCubemapPreview::calcScreenSpace(matrix clipSpace)
{
    double xx = clipSpace[0][0] / clipSpace[3][0];
    double yy = clipSpace[1][0] / clipSpace[3][0];
    double zz = clipSpace[2][0] / clipSpace[3][0];

    return XYZd{ (xx + 1) * g_windowW / 2, (1 - yy) * g_windowH / 2, clipSpace[3][0] };
}

XYZd ScreenCubemapPreview::worldPointToScreenPoint(XYZd worldPos)
{
    double fovRad = degToRad(fov);
    double aspectRatio = (double)g_windowW / g_windowH;

    XYZWd pView = calcViewSpace(worldPos);
    matrix pClip = calcClipSpace(fovRad, aspectRatio, pView);
    return calcScreenSpace(pClip);
}

void ScreenCubemapPreview::render()
{
    if (rotY < 0) {
        rotY += (360 * (int)(rotY / -360 + 1));
    }
    if (rotY >= 360) {
        rotY -= (360 * (int)(rotY / 360));
    }
    rotX = dclamp(-90, rotX, 90);

    viewMatrix = makeViewMatrix();
    Fill::Gradient(0xFF000000, 0xFF000000, 0xFF000000, 0xFF303030).fill({ 0, 0, g_windowW, g_windowH });

    const double fac = 3.0;
    const double facZ = 3.0;
    /*SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 255);
    renderTriangle({
        {-fac, -fac, facZ},
        {fac, -fac, facZ},
        {0, fac, facZ}
    });*/

    /*SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 255);
    renderQuad({
        {-fac, -fac, facZ},
        {fac, -fac, facZ},
        {fac, fac, facZ},
        {-fac, fac, facZ},
    });

    SDL_SetRenderDrawColor(g_rd, 0, 255, 0, 255);
    renderQuad({
        {-fac, -fac, -facZ},
        {fac, -fac, -facZ},
        {fac, fac, -facZ},
        {-fac, fac, -facZ},
    });*/
    /*renderTexturedTriangle({
        {{-fac, -fac, -facZ}, {0,0}},
        {{fac, -fac, -facZ}, {1,0}},
        {{fac, fac, -facZ}, {1,1}}
    }, g_iconNotifTheCreature->get());*/
    /*renderTexturedQuad({
        {{-fac, -fac, -facZ}, {0,1}},
        {{fac, -fac, -facZ}, {1,1}},
        {{-fac, fac, -facZ}, {0,0}},
        {{fac, fac, -facZ}, {1,0}},
        }, parent->getLayerStack()[0]->renderData[g_rd].tex);*/

    const int teselatteCuts = 3;

    static Quad srcFront = Quad{
        Vertex{ {-fac, fac, -facZ}, {0,0} },
        Vertex{ {fac, fac, -facZ}, {1,0} },
        Vertex{ {-fac, -fac, -facZ}, {0,1} },
        Vertex{ {fac, -fac, -facZ}, {1,1} }
    };

    static Quad srcBack = Quad{
        Vertex{ {fac, fac, facZ}, {0,0} },
        Vertex{ {-fac, fac, facZ}, {1,0} },
        Vertex{ {fac, -fac, facZ}, {0,1} },
        Vertex{ {-fac, -fac, facZ}, {1,1} }
    };

    static Quad srcRight = Quad{
        Vertex{ {fac, fac, -facZ}, {0,0} },
        Vertex{ {fac, fac, facZ}, {1,0} },
        Vertex{ {fac, -fac, -facZ}, {0,1} },
        Vertex{ {fac, -fac, facZ}, {1,1} }
    };

    static Quad srcLeft = Quad{
        Vertex{ {-fac, fac, facZ}, {0,0} },
        Vertex{ {-fac, fac, -facZ}, {1,0} },
        Vertex{ {-fac, -fac, facZ}, {0,1} },
        Vertex{ {-fac, -fac, -facZ}, {1,1} }
    };

    static Quad srcTop = Quad{
        Vertex{ {-fac, fac, facZ} , {0,0} },
        Vertex{ {fac, fac, facZ}, {1,0} },
        Vertex{ {-fac, fac, -facZ}, {0,1} },
        Vertex{ {fac, fac, -facZ} , {1,1} }
    };

    static Quad srcBottom = Quad{
        Vertex{ { fac, -fac, facZ } , {0,0} },
        Vertex{ {-fac, -fac, facZ}, {1,0} },
        Vertex{ { fac, -fac, -facZ }, {0,1} },
        Vertex{ {-fac, -fac, -facZ}, {1,1} }
    };

    static std::vector<Quad> frontQs = tesellateQuad(srcFront, teselatteCuts);
    static std::vector<Quad> backQs = tesellateQuad(srcBack, teselatteCuts);
    static std::vector<Quad> rightQs = tesellateQuad(srcRight, teselatteCuts);
    static std::vector<Quad> leftQs = tesellateQuad(srcLeft, teselatteCuts);
    static std::vector<Quad> topQs = tesellateQuad(srcTop, teselatteCuts);
    static std::vector<Quad> bottomQs = tesellateQuad(srcBottom, teselatteCuts);

    if (quadInView(srcFront)) {
        SDL_SetRenderDrawColor(g_rd, 0, 255, 0, 255);
        renderMultipleTexturedQuads(frontQs, parent->getLayerStack()[0]->renderData[g_rd].tex);
    }

    if (quadInView(srcBack)) {
        SDL_SetRenderDrawColor(g_rd, 0, 0, 255, 255);
        renderMultipleTexturedQuads(backQs, parent->getLayerStack()[0]->renderData[g_rd].tex);
    }

    if (quadInView(srcRight)) {
        SDL_SetRenderDrawColor(g_rd, 255, 0, 0, 255);
        renderMultipleTexturedQuads(rightQs, parent->getLayerStack()[0]->renderData[g_rd].tex);
    }

    if (quadInView(srcLeft)) {
        SDL_SetRenderDrawColor(g_rd, 255, 255, 0, 255);
        renderMultipleTexturedQuads(leftQs, parent->getLayerStack()[0]->renderData[g_rd].tex);
    }

    if (quadInView(srcTop)) {
        SDL_SetRenderDrawColor(g_rd, 0, 255, 255, 255);
        renderMultipleTexturedQuads(topQs, parent->getLayerStack()[0]->renderData[g_rd].tex);
    }

    if (quadInView(srcBottom)) {
        SDL_SetRenderDrawColor(g_rd, 255, 0, 255, 255);
        renderMultipleTexturedQuads(bottomQs, parent->getLayerStack()[0]->renderData[g_rd].tex);
    }

    g_fnt->RenderString(frmt("pos: {:02f} {:02f} {:02f}\nrot: {:02f} {:02f} {:02f}\nfov: {}", posX, posY, posZ, rotX, rotY, rotZ, fov),5, 30);
}

void ScreenCubemapPreview::takeInput(SDL_Event evt)
{
    if (evt.type == SDL_EVENT_QUIT) {
        g_closeScreen(this);
        return;
    }

    if (evt.type == SDL_KEYDOWN) {
        switch (evt.key.scancode) {
            case SDL_SCANCODE_W:
                fov += 1;
                break;
            case SDL_SCANCODE_S:
                fov -= 1;
                break;
            case SDL_SCANCODE_D:
                rotZ += 0.2;
                break;
            case SDL_SCANCODE_A:
                rotZ -= 0.2;
                break;
            case SDL_SCANCODE_LEFT:
                posX -= 0.2;
                break;
            case SDL_SCANCODE_RIGHT:
                posX += 0.2;
                break;
            case SDL_SCANCODE_UP:
                posY += 0.2;
                break;
            case SDL_SCANCODE_DOWN:
                posY -= 0.2;
                break;
            case SDL_SCANCODE_PAGEUP:
                posZ += 0.2;
                break;
            case SDL_SCANCODE_PAGEDOWN:
                posZ -= 0.2;
                break;
        }
    }
    else if (evt.type == SDL_MOUSEBUTTONDOWN || evt.type == SDL_MOUSEBUTTONUP) {
        mouseDrag = evt.button.down;
    
    }
    else if (evt.type == SDL_MOUSEMOTION) {
        if (mouseDrag) {
            rotY += -evt.motion.xrel;
            rotX += -evt.motion.yrel;
        }
    }
}

void ScreenCubemapPreview::renderTriangle(std::vector<XYZd> worldSpacePoints)
{
    double fovRad = degToRad(fov);
    double aspectRatio = (double)g_windowW / g_windowH;

    std::vector<Vertex> viewMt;
    bool anyOnScreen = false;
    for (auto& point : worldSpacePoints) {
        XYZWd viewSpace = calcViewSpace(point);
        viewMt.push_back({ XYZd{ viewSpace.x, viewSpace.y, viewSpace.z } });
        anyOnScreen |= viewSpace.z <= -nearPlane;
    }

    if (!anyOnScreen) {
        return;
    }

    viewMt = clipViewSpaceTriangle(viewMt);

    std::vector<XY> screenPoints;

    for (auto& v : viewMt) {
        XYZd screenSpace = calcScreenSpace(calcClipSpace(fovRad, aspectRatio, {v.pos.x, v.pos.y, v.pos.z, 1}));
        screenPoints.push_back(XY{ (int)screenSpace.x, (int)screenSpace.y });
    }

    for (int i = 0; i < screenPoints.size(); i++) {
        g_ttp->addTooltip(Tooltip{ screenPoints[i], frmt("{}",i) });
        drawLine(screenPoints[i], screenPoints[(i + 1) % screenPoints.size()]);
    }
}

void ScreenCubemapPreview::renderQuad(std::vector<XYZd> worldSpacePoints)
{
    if (worldSpacePoints.size() != 4) {
        return;
    }

    renderTriangle({
        worldSpacePoints[0],
        worldSpacePoints[1],
        worldSpacePoints[2],
    });
    renderTriangle({
        worldSpacePoints[3],
        worldSpacePoints[0],
        worldSpacePoints[2],
    });
}

void ScreenCubemapPreview::renderTexturedTriangle(std::vector<Vertex> worldSpacePoints, SDL_Texture* tex)
{
    double fovRad = degToRad(fov);
    double aspectRatio = (double)g_windowW / g_windowH;

    std::vector<Vertex> viewMt;
    bool anyOnScreen = false;
    for (auto& point : worldSpacePoints) {
        XYZWd viewSpace = calcViewSpace(point.pos);
        viewMt.push_back({ XYZd{ viewSpace.x, viewSpace.y, viewSpace.z }, point.uv });
        anyOnScreen |= viewSpace.z <= -nearPlane;
    }

    if (!anyOnScreen) {
        return;
    }

    viewMt = clipViewSpaceTriangle(viewMt);

    std::vector<SDL_Vertex> screenPoints;

    for (auto& v : viewMt) {
        XYZd screenSpace = calcScreenSpace(calcClipSpace(fovRad, aspectRatio, { v.pos.x, v.pos.y, v.pos.z, 1 }));
        SDL_Vertex vv{};
        vv.position = { (float)screenSpace.x, (float)screenSpace.y };
        vv.color = { 1,1,1,1 };
        vv.tex_coord = { (float)v.uv.x, (float)v.uv.y };
        screenPoints.push_back(vv);
    }

    if (viewMt.size() == 3) {
        int ind[] = { 0,1,2 };
        SDL_RenderGeometry(g_rd, tex, screenPoints.data(), 3, ind, 3);
    }
    else if (viewMt.size() == 4) {
        int ind[] = { 0,1,2, 1,2,3 };
        SDL_RenderGeometry(g_rd, tex, screenPoints.data(), 4, ind, 6);
    }
    else {
        logerr(frmt("invalid vertex count: {}", viewMt.size()));
    }

    for (int i = 0; i < screenPoints.size(); i++) {
        XY a = { (int)screenPoints[i].position.x, (int)screenPoints[i].position.y };
        XY b = { (int)screenPoints[(i + 1) % screenPoints.size()].position.x, (int)screenPoints[(i + 1) % screenPoints.size()].position.y };
        //g_ttp->addTooltip(Tooltip{a, frmt("{:.02f} {:.02f}", screenPoints[i].tex_coord.x, screenPoints[i].tex_coord.y)});
        drawLine(a,b);
    }

    //SDL_RenderGeometry(g_rd, tex, )
}

void ScreenCubemapPreview::renderTexturedQuad(std::vector<Vertex> worldSpacePoints, SDL_Texture* tex)
{
    renderTexturedTriangle({
        worldSpacePoints[0],
        worldSpacePoints[1],
        worldSpacePoints[2]
    }, tex);
    renderTexturedTriangle({
        worldSpacePoints[1],
        worldSpacePoints[2],
        worldSpacePoints[3]
    }, tex);
}

std::vector<Quad> ScreenCubemapPreview::tesellateQuad(Quad q) {
    Vertex center = {
        interpolatePosXYZd(q.topLeft.pos, q.bottomRight.pos, 0.5),
        interpolatePosXYd(q.topLeft.uv, q.bottomRight.uv, 0.5)
    };
    Vertex top = {
        interpolatePosXYZd(q.topLeft.pos, q.topRight.pos, 0.5),
        interpolatePosXYd(q.topLeft.uv, q.topRight.uv, 0.5)
    };
    Vertex left = {
        interpolatePosXYZd(q.topLeft.pos, q.bottomLeft.pos, 0.5),
        interpolatePosXYd(q.topLeft.uv, q.bottomLeft.uv, 0.5)
    };
    Vertex right = {
        interpolatePosXYZd(q.topRight.pos, q.bottomRight.pos, 0.5),
        interpolatePosXYd(q.topRight.uv, q.bottomRight.uv, 0.5)
    };
    Vertex bottom = {
        interpolatePosXYZd(q.bottomLeft.pos, q.bottomRight.pos, 0.5),
        interpolatePosXYd(q.bottomLeft.uv, q.bottomRight.uv, 0.5)
    };

    return {
        Quad{ q.topLeft, top, left, center },
        Quad{ top, q.topRight, center, right },
        Quad{ left, center, q.bottomLeft, bottom },
        Quad{ center, right, bottom, q.bottomRight }
    };
}

std::vector<Quad> ScreenCubemapPreview::tesellateQuad(Quad q, int cuts)
{
    std::queue<Quad> cutQueue;
    cutQueue.push(q);
    for (int i = 0; i < cuts; i++) {
        std::queue<Quad> nextQueue;
        while (!cutQueue.empty()) {
            for (auto& q : tesellateQuad(cutQueue.front())) {
                nextQueue.push(q);
            }
            cutQueue.pop();
        }
        cutQueue = nextQueue;
    }

    std::vector<Quad> ret;
    while (!cutQueue.empty()) {
        ret.push_back(cutQueue.front());
        cutQueue.pop();
    }
    return ret;
}

bool ScreenCubemapPreview::quadInView(Quad q)
{
    //double fovRad = degToRad(fov);
    //double aspectRatio = (double)g_windowW / g_windowH;

    bool anyOnScreen = false;
    for (auto& point : {q.bottomLeft, q.bottomRight, q.topLeft, q.topRight}) {
        XYZWd viewSpace = calcViewSpace(point.pos);
        if (viewSpace.z <= -nearPlane) {
            return true;
        }
    }

    return false;
}

void ScreenCubemapPreview::renderMultipleTexturedQuads(std::vector<Quad> q, SDL_Texture* tex)
{
    for (auto& qq : q) {
        renderTexturedQuad({ qq.topLeft, qq.topRight, qq.bottomLeft, qq.bottomRight }, tex);
    }
}
