#include "UIHueWheel.h"
#include "FontRenderer.h"
#include "UIColorPicker.h"

#include <queue>

void UIHueWheel::valueChanged(double v)
{
    value = v;
    parent->editorColorHSliderChanged(value);
}

void UIHueWheel::render(XY at)
{
    SDL_Rect bounds = { at.x,at.y, wxWidth, wxHeight };
    XY origin = { bounds.x + bounds.w / 2, bounds.y + bounds.h / 2 };

    double r = outerDistance();
    double r2 = innerDistance();

    double accuracy = 12;

    std::vector<SDL_Vertex> vertexBuffer;

    for (double a = 0; a < 360; a += accuracy) {

        double rad = ((a-90) / 180) * M_PI;

        double vecX = cos(rad);
        double vecY = sin(rad);

        XY longerPos = xyAdd(origin, { (int)round(vecX * r), (int)round(vecY * r) });
        XY shorterPos = xyAdd(origin, { (int)round(vecX * r2), (int)round(vecY * r2) });

        SDL_Color col = rgb2sdlcolor(hsv2rgb(hsv{ a, 1.0, 1.0 }));

        SDL_Vertex newVtx{};
        newVtx.color = toFColor(col);
        newVtx.position = { (float)shorterPos.x, (float)shorterPos.y };
        vertexBuffer.push_back(newVtx);
        newVtx.position = { (float)longerPos.x, (float)longerPos.y };
        vertexBuffer.push_back(newVtx);
    }

    vertexBuffer.push_back(vertexBuffer[0]);
    vertexBuffer.push_back(vertexBuffer[1]);

    int tridraws = 0;

    SDL_Vertex* vtxPtr = vertexBuffer.data();
    int idx[] = { 0,1,2 };
    for (int i = 0; i < vertexBuffer.size() - 2; i++) {
        SDL_RenderGeometry(g_rd, NULL, vtxPtr + i, 3, idx, 3);
        tridraws++;

#if _DEBUG
        if (g_debugConfig.debugColorWheel) {
            std::vector<SDL_FPoint> fps = {
                vertexBuffer[i].position,
                vertexBuffer[i + 1].position,
                vertexBuffer[i + 2].position,
                vertexBuffer[i].position
            };

            SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 127);
            SDL_RenderLines(g_rd, fps.data(), 4);
        }
#endif
    }

    //draw outline
    for (int i = 0; i < vertexBuffer.size() - 2; i++) {
        XY cPos = { (int)vertexBuffer[i].position.x, (int)vertexBuffer[i].position.y };
        XY cNextPos = { (int)vertexBuffer[i + 2].position.x, (int)vertexBuffer[i + 2].position.y };
        SDL_SetRenderDrawColor(g_rd, 255,255,255, 80);
        drawLine(cPos, cNextPos, 1.0);
    }

    if (g_debugConfig.debugColorWheel) {
        g_fnt->RenderString(frmt("dcs: {}", tridraws), at.x, at.y);
    }

    renderWheelPosition(at);
}

void UIHueWheel::handleInput(SDL_Event evt, XY gPosOffset)
{
    switch (evt.type) {
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (evt.button.button == SDL_BUTTON_LEFT) {
                XY mousePos = { (int)evt.button.x, (int)evt.button.y };
                if (onScreenPosInWheelSlider(gPosOffset, mousePos)) {
                    mouseDrag = true;
                    valueChanged(angleFromOriginAt(gPosOffset, mousePos));
                }
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (evt.button.button == SDL_BUTTON_LEFT) {
                mouseDrag = false;
            }
            break;
        case SDL_EVENT_MOUSE_MOTION:
            {
                XY mousePos = { (int)evt.motion.x, (int)evt.motion.y };
                if (mouseDrag && onScreenPosInWheelSlider(gPosOffset, mousePos)) {
                    valueChanged(angleFromOriginAt(gPosOffset, mousePos));
                }
            }
            break;
    }
}

bool UIHueWheel::isMouseIn(XY thisPositionOnScreen, XY mousePos)
{
    XY dim = getDimensions();
    return pointInBox(mousePos, SDL_Rect{ thisPositionOnScreen.x, thisPositionOnScreen.y, dim.x, dim.y });
}

void UIHueWheel::renderWheelPosition(XY at)
{
    SDL_Rect bounds = { at.x,at.y, wxWidth, wxHeight };
    XY origin = { bounds.x + bounds.w / 2, bounds.y + bounds.h / 2 };

    double innerDist = innerDistance();
    double outerDist = outerDistance();

    double innerDistOffs = innerDist - 4;
    double outerDistOffs = outerDist + 4;

    double angleRad = ((value - 90) / 180) * M_PI;
    XY pointInner = xyAdd(origin, { (int)(cos(angleRad) * innerDist), (int)(sin(angleRad) * innerDist) });
    XY pointOuter = xyAdd(origin, { (int)(cos(angleRad) * outerDist), (int)(sin(angleRad) * outerDist) });
    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xa0);
    drawLine(pointInner, pointOuter, 1.0);

    //left side
    angleRad = ((value - 2 - 90) / 180) * M_PI;
    XY leftPointInner = xyAdd(origin, { (int)(cos(angleRad) * innerDistOffs), (int)(sin(angleRad) * innerDistOffs) });
    XY leftPointOuter = xyAdd(origin, { (int)(cos(angleRad) * outerDistOffs), (int)(sin(angleRad) * outerDistOffs) });
    SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0xff);
    drawLine(leftPointInner, leftPointOuter, 1.0);
    angleRad = ((value - 3 - 90) / 180) * M_PI;
    leftPointInner = xyAdd(origin, { (int)(cos(angleRad) * innerDistOffs), (int)(sin(angleRad) * innerDistOffs) });
    leftPointOuter = xyAdd(origin, { (int)(cos(angleRad) * outerDistOffs), (int)(sin(angleRad) * outerDistOffs) });
    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xff);
    drawLine(leftPointInner, leftPointOuter, 1.0);

    //right side
    angleRad = ((value + 2 - 90) / 180) * M_PI;
    XY rightPointInner = xyAdd(origin, { (int)(cos(angleRad) * innerDistOffs), (int)(sin(angleRad) * innerDistOffs) });
    XY rightPointOuter = xyAdd(origin, { (int)(cos(angleRad) * outerDistOffs), (int)(sin(angleRad) * outerDistOffs) });
    SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0xff);
    drawLine(rightPointInner, rightPointOuter, 1.0);
    angleRad = ((value + 3 - 90) / 180) * M_PI;
    rightPointInner = xyAdd(origin, { (int)(cos(angleRad) * innerDistOffs), (int)(sin(angleRad) * innerDistOffs) });
    rightPointOuter = xyAdd(origin, { (int)(cos(angleRad) * outerDistOffs), (int)(sin(angleRad) * outerDistOffs) });
    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xff);
    drawLine(rightPointInner, rightPointOuter, 1.0);


    drawLine(leftPointInner, rightPointInner, 1.0);
    drawLine(leftPointOuter, rightPointOuter, 1.0);
    drawLine(leftPointOuter, pointOuter, 1.0);
    drawLine(rightPointOuter, pointOuter, 1.0);
    drawLine(leftPointInner, pointInner, 1.0);
    drawLine(rightPointInner, pointInner, 1.0);
}

SDL_Rect UIHueWheel::innerRect()
{
    SDL_Rect bounds = { 0,0, wxWidth, wxHeight };
    XY origin = { bounds.x + bounds.w / 2, bounds.y + bounds.h / 2 };

    double pointDeg = ((-135.0) / 180) * M_PI;
    int pointX = origin.x + (int)(cos(pointDeg) * innerDistance());
    int pointY = origin.y + (int)(sin(pointDeg) * innerDistance());

    int squareSide = (origin.x - pointX) * 2;

    return { pointX, pointY, squareSide, squareSide };
}

double UIHueWheel::angleFromOriginAt(XY at, XY mousePos)
{
    SDL_Rect bounds = { at.x,at.y, wxWidth, wxHeight };
    XY origin = { bounds.x + bounds.w / 2, bounds.y + bounds.h / 2 };

    XY diff = xySubtract(mousePos, origin);

    double angle = atan2(diff.y, diff.x);
    angle *= 180.0/M_PI;
    angle += 90.0;
    if (angle < 0) {
        angle += 360;
    }
    if (angle >= 360) {
        angle -= 360;
    }
    return angle;
}

bool UIHueWheel::onScreenPosInWheelSlider(XY at, XY mousePos)
{
    SDL_Rect bounds = { at.x,at.y, wxWidth, wxHeight };
    XY origin = { bounds.x + bounds.w / 2, bounds.y + bounds.h / 2 };

    double outer = outerDistance();
    double inner = innerDistance();

    double distance = xyDistance(origin, mousePos);

    return distance <= outer && distance >= inner;
}

double UIHueWheel::outerDistance()
{
    return dxmin(wxWidth / 2.0, wxHeight / 2.0);
}

double UIHueWheel::innerDistance()
{
    return outerDistance() / 4 * 3;
}
