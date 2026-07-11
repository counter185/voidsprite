#include "ScreenCubemapPreview.h"
#include "FontRenderer.h"

double degToRad(double deg) {
    return deg * M_PI / 180.0;
}

XYZd ScreenCubemapPreview::intersectViewSpace(XYZd a, XYZd b)
{
    double t = (-nearPlane - a.z) / (b.z - a.z);
    return { a.x + t * (b.x - a.x), a.y + t * (b.y - a.y), -nearPlane };
}

std::vector<XYZd> ScreenCubemapPreview::clipViewSpaceTriangle(std::vector<XYZd> points)
{
    std::vector<XYZd> ret;
    for (int i = 0; i < points.size(); i++) {
        XYZd now = points[i];
        XYZd next = points[(i + 1) % 3];

        bool nowInside = now.z <= -nearPlane;
        bool nextInside = next.z <= -nearPlane;

        if (nowInside && nextInside)
        {
            ret.push_back(next);
        }
        else if (nowInside && !nextInside)
        {
            ret.push_back(intersectViewSpace(now, next));
        }
        else if (!nowInside && nextInside)
        {
            ret.push_back(intersectViewSpace(now, next));
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
    matrix viewMatrix = makeViewMatrix();
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
    Fill::Gradient(0xFF000000, 0xFF000000, 0xFF000000, 0xFF303030).fill({ 0, 0, g_windowW, g_windowH });

    const double fac = 3.0;
    const double facZ = 4.0;
    /*SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 255);
    renderTriangle({
        {-fac, -fac, facZ},
        {fac, -fac, facZ},
        {0, fac, facZ}
    });*/

    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 255);
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
    });

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

    std::vector<XYZd> viewMt;
    bool anyOnScreen = false;
    for (auto& point : worldSpacePoints) {
        XYZWd viewSpace = calcViewSpace(point);
        viewMt.push_back(XYZd{ viewSpace.x, viewSpace.y, viewSpace.z });
        anyOnScreen |= viewSpace.z <= -nearPlane;
    }

    if (!anyOnScreen) {
        return;
    }

    viewMt = clipViewSpaceTriangle(viewMt);

    std::vector<XY> screenPoints;

    for (auto& v : viewMt) {
        XYZd screenSpace = calcScreenSpace(calcClipSpace(fovRad, aspectRatio, {v.x, v.y, v.z, 1}));
        screenPoints.push_back(XY{ (int)screenSpace.x, (int)screenSpace.y });
    }

    for (int i = 0; i < screenPoints.size(); i++) {
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
