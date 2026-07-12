#include "ScreenCubemapPreview.h"
#include "FontRenderer.h"
#include "TooltipsLayer.h"
#include "maineditor.h"
#include "UISlider.h"
#include "UILabel.h"
#include "UIStackPanel.h"
#include "PopupFilePicker.h"
#include "FileIO.h"
#include "Notification.h"
#include "UIButton.h"

#define EVENT_CUBEMAP_OVERRIDE_TOP 1
#define EVENT_CUBEMAP_OVERRIDE_BOTTOM 2

double degToRad(double deg) {
    return deg * M_PI / 180.0;
}

void ScreenCubemapPreview::eventFileOpen(int evt_id, PlatformNativePathString name, int importerIndex)
{
    Layer** target =
        evt_id == EVENT_CUBEMAP_OVERRIDE_TOP ? &overrideTop
        : evt_id == EVENT_CUBEMAP_OVERRIDE_BOTTOM ? &overrideBottom
        : NULL;

    if (target != NULL) {
        g_startNewOperation([target, name](OperationProgressReport* report) {
            Layer* l = loadAnyIntoFlat(convertStringToUTF8OnWin32(name), NULL, report);
            if (l != NULL) {
                g_startNewMainThreadOperation([l, target]() {
                    if (*target != NULL) {
                        delete (*target);
                    }
                    *target = l;
                });
            }
            else {
                g_addNotificationFromThread(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.error.fileloadfail")));
            }
        });
    }
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
    bool noneOutside = true;
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
            noneOutside = false;
            ret.push_back(intersectTexturedViewSpace(now, next));
        }
        else if (!nowInside && nextInside)
        {
            noneOutside = false;
            ret.push_back(intersectTexturedViewSpace(now, next));
            ret.push_back(next);
        }
    }
    return noneOutside && !ret.empty() ? points : ret;
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

matrix ScreenCubemapPreview::makeProjectionMatrix()
{
    double fovRad = degToRad(fov);
    double aspectRatio = (double)g_windowW / g_windowH;

    return {
        {1.0 / (aspectRatio * tan(fovRad / 2)), 0, 0, 0},
        {0, 1.0 / (tan(fovRad / 2)), 0, 0},
        {0, 0, nearPlane, farPlane},
        {0, 0, -1, 0}
    };
}

XYZWd ScreenCubemapPreview::calcViewSpace(XYZd worldPos)
{
    //matrix viewMatrix = viewMatrix;//makeViewMatrix();
    matrix viewSpace = matrixMultiply(viewMatrix, { {worldPos.x}, {worldPos.y}, {worldPos.z}, {1} });
    return XYZWd{ viewSpace[0][0], viewSpace[1][0], viewSpace[2][0], viewSpace[3][0] };
}

matrix ScreenCubemapPreview::calcClipSpace(XYZWd viewSpace)
{
    double xP = viewSpace.x / viewSpace.z;
    double yP = viewSpace.y / viewSpace.z;

    //viewSpace.x = xP;
    //viewSpace.y = yP;

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
    XYZWd pView = calcViewSpace(worldPos);
    matrix pClip = calcClipSpace(pView);
    return calcScreenSpace(pClip);
}

ScreenCubemapPreview::ScreenCubemapPreview(MainEditor* caller)
    : parent(caller)
{
    PanelCubemapPreview* panel = new PanelCubemapPreview(this);
    panel->position = { 10, 30 };
    wxsManager.addDrawable(panel);

    const double fac = 3.0;
    const double facZ = 3.0;

    qFront = new TesellatedQuad(this, Quad{
        Vertex{ {-fac, fac, -facZ}, {0,0} },
        Vertex{ {fac, fac, -facZ}, {1,0} },
        Vertex{ {-fac, -fac, -facZ}, {0,1} },
        Vertex{ {fac, -fac, -facZ}, {1,1} }
    });
    qBack = new TesellatedQuad(this, Quad{
        Vertex{ {fac, fac, facZ}, {0,0} },
        Vertex{ {-fac, fac, facZ}, {1,0} },
        Vertex{ {fac, -fac, facZ}, {0,1} },
        Vertex{ {-fac, -fac, facZ}, {1,1} }
    });
    qRight = new TesellatedQuad(this, Quad{
        Vertex{ {fac, fac, -facZ}, {0,0} },
        Vertex{ {fac, fac, facZ}, {1,0} },
        Vertex{ {fac, -fac, -facZ}, {0,1} },
        Vertex{ {fac, -fac, facZ}, {1,1} }
    });
    qLeft = new TesellatedQuad(this, Quad{
        Vertex{ {-fac, fac, facZ}, {0,0} },
        Vertex{ {-fac, fac, -facZ}, {1,0} },
        Vertex{ {-fac, -fac, facZ}, {0,1} },
        Vertex{ {-fac, -fac, -facZ}, {1,1} }
    });
    qTop = new TesellatedQuad(this, Quad{
        Vertex{ {-fac, fac, facZ} , {0,0} },
        Vertex{ {fac, fac, facZ}, {1,0} },
        Vertex{ {-fac, fac, -facZ}, {0,1} },
        Vertex{ {fac, fac, -facZ} , {1,1} }
    });
    qBottom = new TesellatedQuad(this, Quad{
        Vertex{ { fac, -fac, facZ } , {0,0} },
        Vertex{ {-fac, -fac, facZ}, {1,0} },
        Vertex{ { fac, -fac, -facZ }, {0,1} },
        Vertex{ {-fac, -fac, -facZ}, {1,1} }
    });
}

ScreenCubemapPreview::~ScreenCubemapPreview()
{
    delete qFront;
    delete qBack;
    delete qLeft;
    delete qRight;
    delete qTop;
    delete qBottom;

    if (overrideTop != NULL) {
        delete overrideTop;
    }
    if (overrideBottom != NULL) {
        delete overrideBottom;
    }
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
    projection = makeProjectionMatrix();
    renderWithBlurPanelsIfEnabled([this]() {this->renderScene(); });
    BaseScreen::render();
}

void ScreenCubemapPreview::renderScene()
{
    Fill::Gradient(0xFF000000, 0xFF000000, 0xFF000000, 0xFF303030).fill({ 0, 0, g_windowW, g_windowH });

    SDL_SetRenderDrawColor(g_rd, 0, 255, 0, 255);
    qFront->render();
    //renderTesellatedTexturedQuad(frontQqs, parent->getLayerStack()[0]->renderData[g_rd].tex);

    SDL_SetRenderDrawColor(g_rd, 0, 0, 255, 255);
    qBack->render();
    //renderTesellatedTexturedQuad(backQqs, parent->getLayerStack()[0]->renderData[g_rd].tex);

    SDL_SetRenderDrawColor(g_rd, 255, 0, 0, 255);
    qRight->render();

    SDL_SetRenderDrawColor(g_rd, 255, 255, 0, 255);
    qLeft->render();

    SDL_SetRenderDrawColor(g_rd, 0, 255, 255, 255);
    qTop->render(overrideTop);

    SDL_SetRenderDrawColor(g_rd, 255, 0, 255, 255);
    qBottom->render(overrideBottom);

    g_fnt->RenderString(frmt("pos: {:02f} {:02f} {:02f}\nrot: {:02f} {:02f} {:02f}\nfov: {}", posX, posY, posZ, rotX, rotY, rotZ, fov), 5, 30);
}

void ScreenCubemapPreview::defaultInputAction(SDL_Event evt)
{
    evt = convertTouchToMouseEvent(evt);

    if (evt.type == SDL_KEYDOWN) {
        switch (evt.key.scancode)  {
            case SDL_SCANCODE_W:
                fov += 1;
                resetAllQuads();
                break;
            case SDL_SCANCODE_S:
                fov -= 1;
                resetAllQuads();
                break;
            case SDL_SCANCODE_D:
                rotZ += 0.2;
                resetAllQuads();
                break;
            case SDL_SCANCODE_A:
                rotZ -= 0.2;
                resetAllQuads();
                break;
            case SDL_SCANCODE_LEFT:
                posX -= 0.2;
                resetAllQuads();
                break;
            case SDL_SCANCODE_RIGHT:
                posX += 0.2;
                resetAllQuads();
                break;
            case SDL_SCANCODE_UP:
                posY += 0.2;
                resetAllQuads();
                break;
            case SDL_SCANCODE_DOWN:
                posY -= 0.2;
                resetAllQuads();
                break;
            case SDL_SCANCODE_PAGEUP:
                posZ += 0.2;
                resetAllQuads();
                break;
            case SDL_SCANCODE_PAGEDOWN:
                posZ -= 0.2;
                resetAllQuads();
                break;
        }
    }
    else if (evt.type == SDL_MOUSEBUTTONDOWN || evt.type == SDL_MOUSEBUTTONUP) {
        mouseDrag = evt.button.down;
    
    }
    else if (evt.type == SDL_MOUSEMOTION) {
        if (mouseDrag) {
            const double rotSensitivity = 0.2;
            rotY += -evt.motion.xrel * rotSensitivity;
            rotX += -evt.motion.yrel * rotSensitivity;
            resetAllQuads();
        }
    }
}

void ScreenCubemapPreview::renderTriangle(std::vector<XYZd> worldSpacePoints)
{
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
        XYZd screenSpace = calcScreenSpace(calcClipSpace({v.pos.x, v.pos.y, v.pos.z, 1}));
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
    std::vector<Vertex> viewMt;
    for (auto& point : worldSpacePoints) {
        XYZWd viewSpace = calcViewSpace(point.pos);
        viewMt.push_back({ XYZd{ viewSpace.x, viewSpace.y, viewSpace.z }, point.uv });
    }

    renderViewSpaceTriangle(viewMt, tex);

    //SDL_RenderGeometry(g_rd, tex, )
}

void ScreenCubemapPreview::renderViewSpaceTriangle(std::vector<Vertex> viewMt, SDL_Texture* tex, XYZd* screenPointCache)
{
    bool anyOnScreen = false;
    for (auto& viewSpacePoint : viewMt) {
        anyOnScreen |= viewSpacePoint.pos.z <= -nearPlane;
    }
    if (!anyOnScreen) {
        return;
    }

    auto newViewMt = clipViewSpaceTriangle(viewMt);
    viewMt = newViewMt;

    std::vector<SDL_Vertex> screenPoints;

    int i = 0;
    for (auto& v : viewMt) {
        XYZd screenSpace;
        if (viewMt.size() == 3 && screenPointCache != NULL && !isnan(screenPointCache[i].x)) {
            screenSpace = screenPointCache[i];
        }
        else
        {
            screenSpace = calcScreenSpace(calcClipSpace({ v.pos.x, v.pos.y, v.pos.z, 1 }));
            if (viewMt.size() == 3 && screenPointCache != NULL) {
                screenPointCache[i] = screenSpace;
            }
        }
        i++;
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

    /*for (int i = 0; i < screenPoints.size(); i++) {
        XY a = { (int)screenPoints[i].position.x, (int)screenPoints[i].position.y };
        XY b = { (int)screenPoints[(i + 1) % screenPoints.size()].position.x, (int)screenPoints[(i + 1) % screenPoints.size()].position.y };
        //g_ttp->addTooltip(Tooltip{a, frmt("{:.02f} {:.02f}", screenPoints[i].tex_coord.x, screenPoints[i].tex_coord.y)});
        drawLine(a, b);
    }*/
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

std::vector<std::vector<Vertex>> ScreenCubemapPreview::tesellateQuadV2(Quad q, int splitsX, int splitsY)
{
    std::vector<std::vector<Vertex>> ret;

    Vertex topLeft = q.topLeft;
    Vertex topRight = q.topRight;
    Vertex bottomLeft = q.bottomLeft;
    Vertex bottomRight = q.bottomRight;

    for (int y = 0; y < (splitsY + 1); y++) {
        double tVertical = (double)y / splitsY;
        Vertex leftOrigin = {
            interpolatePosXYZd(topLeft.pos, bottomLeft.pos, tVertical),
            interpolatePosXYd(topLeft.uv, bottomLeft.uv, tVertical)
        };
        Vertex rightOrigin = {
            interpolatePosXYZd(topRight.pos, bottomRight.pos, tVertical),
            interpolatePosXYd(topRight.uv, bottomRight.uv, tVertical)
        };

        std::vector<Vertex> thisLine;
        for (int x = 0; x < (splitsX + 1); x++) {
            double tHorizontal = (double)x / splitsX;
            thisLine.push_back({
                interpolatePosXYZd(leftOrigin.pos, rightOrigin.pos, tHorizontal),
                interpolatePosXYd(leftOrigin.uv, rightOrigin.uv, tHorizontal)
            });
        }
        ret.push_back(thisLine);
    }

    return ret;
}

void ScreenCubemapPreview::resetAllQuads()
{
    qFront->reset();
    qBack->reset();
    qLeft->reset();
    qRight->reset();
    qTop->reset();
    qBottom->reset();
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

std::pair<std::vector<SDL_Vertex>, std::vector<int>> ScreenCubemapPreview::evalTriangleScreenPoints(std::vector<Vertex> viewMt)
{
    bool anyOnScreen = false;
    for (auto& viewSpacePoint : viewMt) {
        anyOnScreen |= viewSpacePoint.pos.z <= -nearPlane;
    }
    if (!anyOnScreen) {
        return {};
    }

    auto newViewMt = clipViewSpaceTriangle(viewMt);
    viewMt = newViewMt;

    std::vector<SDL_Vertex> screenPoints;

    for (auto& v : viewMt) {
        XYZd screenSpace = calcScreenSpace(calcClipSpace({ v.pos.x, v.pos.y, v.pos.z, 1 }));
        SDL_Vertex vv{};
        vv.position = { (float)screenSpace.x, (float)screenSpace.y };
        vv.color = { 1,1,1,1 };
        vv.tex_coord = { (float)v.uv.x, (float)v.uv.y };
        screenPoints.push_back(vv);
    }

    if (viewMt.size() == 3) {
        return { screenPoints, {0,1,2} };
    }
    else if (viewMt.size() == 4) {
        return { screenPoints, { 0,1,2, 1,2,3 } };
    }
    return {};
}

void ScreenCubemapPreview::promptOverrideTexture(int id)
{
    PopupFilePicker::PlatformAnyImageImportDialog(this, TL("vsp.popup.overridecubetex"), id, true);
}

void ScreenCubemapPreview::renderMultipleTexturedQuads(std::vector<Quad> q, SDL_Texture* tex)
{
    for (auto& qq : q) {
        renderTexturedQuad({ qq.topLeft, qq.topRight, qq.bottomLeft, qq.bottomRight }, tex);
    }
}

void ScreenCubemapPreview::renderTesellatedTexturedQuad(std::vector<std::vector<Vertex>>& q, SDL_Texture* tex)
{

    std::vector<std::vector<Vertex>> viewSpaceQs;
    std::vector<std::vector<XYZd>> screenPointCache;

    for (auto& qq : q) {
        std::vector<Vertex> n;
        std::vector<XYZd> s;
        for (auto& qqq : qq) {
            XYZWd viewSpace = calcViewSpace(qqq.pos);
            n.push_back({ XYZd{ viewSpace.x, viewSpace.y, viewSpace.z }, qqq.uv });
            s.push_back({ NAN,0,0 });
        }
        viewSpaceQs.push_back(n);
        screenPointCache.push_back(s);
    }


    int pointsX = q[0].size();
    int pointsY = q.size();

    for (int y = 0; y < pointsY - 1; y++) {
        for (int x = 0; x < pointsX - 1; x++) {
            XYZd screenPointBuffer[3];
            screenPointBuffer[0] = screenPointCache[y][x];
            screenPointBuffer[1] = screenPointCache[y][x+1];
            screenPointBuffer[2] = screenPointCache[y+1][x];
            renderViewSpaceTriangle({
                viewSpaceQs[y][x],
                viewSpaceQs[y][x+1],
                viewSpaceQs[y+1][x],
            }, tex, screenPointBuffer);
            screenPointCache[y][x] = screenPointBuffer[0];
            screenPointCache[y][x+1] = screenPointBuffer[1];
            screenPointCache[y+1][x] = screenPointBuffer[2];

            screenPointBuffer[0] = screenPointCache[y+1][x];
            screenPointBuffer[1] = screenPointCache[y][x+1];
            screenPointBuffer[2] = screenPointCache[y+1][x+1];
            renderViewSpaceTriangle({
                viewSpaceQs[y+1][x],
                viewSpaceQs[y][x+1],
                viewSpaceQs[y+1][x+1],
            }, tex, screenPointBuffer);
            screenPointCache[y+1][x] = screenPointBuffer[0];
            screenPointCache[y][x+1] = screenPointBuffer[1];
            screenPointCache[y+1][x+1] = screenPointBuffer[2];
        }
    }
}

TesellatedQuad::TesellatedQuad(ScreenCubemapPreview* caller, Quad base)
{
    parent = caller;
    baseQuad = base;
}

void TesellatedQuad::reset() {
    screenSpaceDrawCalls.clear();
    count = 4;
    framesWait = 0;
}

void TesellatedQuad::render(Layer* overrideTex) {
    if (overrideTex != NULL) {
        overrideTex->prerender();
    }
    else {
        for (auto& l : parent->parent->getLayerStack()) {
            l->prerender();
        }
    }

    if (count < maxCount) {
        if (framesWait == 0) {
            count =
                count < 15 ? count + 1
                : count < 25 ? count + 3
                : ixmin(maxCount, count + 10);
            updateScreenSpaceVtx();
            framesWait = 
                count < 15 ? 0
                : count < 25 ? 1
                : 30;
        }
        else {
            framesWait--;
        }
    }

    for (auto& [v, i] : screenSpaceDrawCalls) {
        if (overrideTex != NULL) {
            SDL_RenderGeometry(g_rd, overrideTex->renderData[g_rd].tex,
                v.data(), v.size(), i.data(), i.size());
        }
        else {
            for (auto& l : parent->parent->getLayerStack()) {
                SDL_RenderGeometry(g_rd, l->renderData[g_rd].tex,
                    v.data(), v.size(), i.data(), i.size());
            }
        }
    }
}

void TesellatedQuad::updateScreenSpaceVtx()
{
    screenSpaceDrawCalls.clear();

    if (parent->quadInView(baseQuad)) {
        auto tesellated = parent->tesellateQuadV2(baseQuad, count, count);

        std::vector<std::vector<Vertex>> viewSpaceQs;

        for (auto& qq : tesellated) {
            std::vector<Vertex> n;
            std::vector<XYZd> s;
            for (auto& qqq : qq) {
                XYZWd viewSpace = parent->calcViewSpace(qqq.pos);
                n.push_back({ XYZd{ viewSpace.x, viewSpace.y, viewSpace.z }, qqq.uv });
                s.push_back({ NAN,0,0 });
            }
            viewSpaceQs.push_back(n);
        }

        int pointsX = tesellated[0].size();
        int pointsY = tesellated.size();

        for (int y = 0; y < pointsY - 1; y++) {
            for (int x = 0; x < pointsX - 1; x++) {
                screenSpaceDrawCalls.push_back(parent->evalTriangleScreenPoints({
                    viewSpaceQs[y][x],
                    viewSpaceQs[y][x + 1],
                    viewSpaceQs[y + 1][x],
                    }));

                screenSpaceDrawCalls.push_back(parent->evalTriangleScreenPoints({
                    viewSpaceQs[y + 1][x],
                    viewSpaceQs[y][x + 1],
                    viewSpaceQs[y + 1][x + 1],
                    }));
            }
        }
    }
}

PanelCubemapPreview::PanelCubemapPreview(ScreenCubemapPreview* caller)
{
    parent = caller;

    wxWidth = 300;
    wxHeight = 200;

    setupDraggable();
    //setupResizable({300,100});
    setupCollapsible();
    addTitleText("CUBEMAP PREVIEW");


    UILabel* fovV = new UILabel("000");

    UISlider* fovSlider = new UISlider();
    fovSlider->setValue(20, 140, parent->fov);
    fovSlider->wxWidth = 180;
    fovSlider->wxHeight = 30;
    fovSlider->onChangeValueCallback = [this, fovV](UISlider* s, float v) {
        parent->fov = s->getValue(20, 140);
        fovV->setText(std::to_string((int)parent->fov));
        parent->resetAllQuads();
    };

    UIButton* btnOverrideTop = new UIButton("Override top texture");
    btnOverrideTop->onClickCallback = [this](...) { parent->promptOverrideTexture(EVENT_CUBEMAP_OVERRIDE_TOP); };

    UIButton* btnOverrideBottom = new UIButton("Override bottom texture");
    btnOverrideBottom->onClickCallback = [this](...) { parent->promptOverrideTexture(EVENT_CUBEMAP_OVERRIDE_BOTTOM); };

    wxsTarget().addDrawable(
        UIStackPanel::Vertical(5, {
            UIStackPanel::Horizontal(4, {
                new UILabel("FOV"),
                Panel::Space(20, 1),
                fovV,
                Panel::Space(5,1),
                fovSlider
            }),
            btnOverrideTop,
            btnOverrideBottom,
            new UIDynamicLabel([this]() {
                int count = parent->qTop->count;
                int maxCount = parent->qTop->maxCount;
                return count != maxCount ? frmt("Unwarping textures... {}/{}", count, maxCount) : std::string();
            })
        }, {5, 40})
    );

    fovV->setText(std::to_string((int)parent->fov));
}
