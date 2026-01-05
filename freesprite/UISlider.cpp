#include "FontRenderer.h"
#include "UISlider.h"
#include "mathops.h"

UISlider::UISlider()
{
    backgroundFill = visualConfigFill("ui/slider/bg");
}

void UISlider::drawPosIndicator(XY origin) {

    static SDL_Color colorShadow = visualConfigColor("ui/slider/ind_shadow");
    static SDL_Color colorPrimary = visualConfigColor("ui/slider/ind_body");

    int xdist = 3;
    SDL_SetRenderDrawColor(g_rd, colorShadow.r, colorShadow.g, colorShadow.b, colorShadow.a);

    if (verticalSlider) {
        XY centerPoint = xyAdd(origin, XY{ 0, (int)(wxHeight * (1.0-sliderPos)) });

        for (int x = 0; x < 2; x++) {
            int fxdist = xdist + x;
            SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y - fxdist, centerPoint.x + wxWidth + fxdist, centerPoint.y - fxdist);
            SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y + fxdist, centerPoint.x + wxWidth + fxdist, centerPoint.y + fxdist);
            //draws first triangle
            SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y - fxdist, centerPoint.x, centerPoint.y);
            SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y + fxdist, centerPoint.x, centerPoint.y);
            //draws left/right bars
            SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y - fxdist, centerPoint.x - fxdist, centerPoint.y + fxdist);
            SDL_RenderDrawLine(g_rd, centerPoint.x + wxWidth + fxdist, centerPoint.y - fxdist, centerPoint.x + wxWidth + fxdist, centerPoint.y + fxdist);
            //draws second triangle
            SDL_RenderDrawLine(g_rd, centerPoint.x + wxWidth + fxdist, centerPoint.y - fxdist, centerPoint.x + wxWidth, centerPoint.y);
            SDL_RenderDrawLine(g_rd, centerPoint.x + wxWidth + fxdist, centerPoint.y + fxdist, centerPoint.x + wxWidth, centerPoint.y);

            SDL_SetRenderDrawColor(g_rd, colorPrimary.r, colorPrimary.g, colorPrimary.b, colorPrimary.a);
        }
    }
    else {
        XY centerPoint = xyAdd(origin, XY{ (int)(wxWidth * sliderPos), 0 });

        for (int x = 0; x < 2; x++) {
            int fxdist = xdist + x;
            SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y - fxdist, centerPoint.x - fxdist, centerPoint.y + wxHeight + fxdist);
            SDL_RenderDrawLine(g_rd, centerPoint.x + fxdist, centerPoint.y - fxdist, centerPoint.x + fxdist, centerPoint.y + wxHeight + fxdist);
            //draws first triangle
            SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y - fxdist, centerPoint.x, centerPoint.y);
            SDL_RenderDrawLine(g_rd, centerPoint.x + fxdist, centerPoint.y - fxdist, centerPoint.x, centerPoint.y);
            //draws top/bottom bars
            SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y + wxHeight + fxdist, centerPoint.x + fxdist, centerPoint.y + wxHeight + fxdist);
            SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y - fxdist, centerPoint.x + fxdist, centerPoint.y - fxdist);
            //draws second triangle
            SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y + wxHeight + fxdist, centerPoint.x, centerPoint.y + wxHeight);
            SDL_RenderDrawLine(g_rd, centerPoint.x + fxdist, centerPoint.y + wxHeight + fxdist, centerPoint.x, centerPoint.y + wxHeight);
            SDL_SetRenderDrawColor(g_rd, colorPrimary.r, colorPrimary.g, colorPrimary.b, colorPrimary.a);
        }
    }
}

void UISlider::render(XY pos)
{
    SDL_Rect drawrect = { pos.x, pos.y, wxWidth, wxHeight };
    backgroundFill.fill(drawrect);
    SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, focused ? 0x80 : 0x30);
    SDL_RenderDrawRect(g_rd, &drawrect);
    drawPosIndicator(pos);
}

void UISlider::handleInput(SDL_Event evt, XY gPosOffset)
{
    switch (evt.type) {
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            if (evt.button.button == 1) {
                if (evt.button.down) {
                    XY mousePos = xySubtract(XY{ (int)evt.button.x, (int)evt.button.y }, gPosOffset);
                    if (mousePos.y >= 0 && mousePos.y <= wxHeight) {
                        sliderPos = verticalSlider ? fclamp(0.0f, 1.0 - (mousePos.y / (float)wxHeight), 1.0f)
                                                   : fclamp(0.0f, mousePos.x / (float)wxWidth, 1.0f);
                        this->onSliderPosChanged();
                    }
                }
                else {
                    if (mouseHeld) {
                        this->onSliderPosFinishedChanging();
                    }
                }
                mouseHeld = evt.button.down;
            }
            break;
        case SDL_MOUSEMOTION:
            if (mouseHeld) {
                XY mousePos = xySubtract(XY{ (int)(evt.motion.x), (int)(evt.motion.y) }, gPosOffset);
                sliderPos = verticalSlider ? fclamp(0.0f, 1.0 - (mousePos.y / (float)wxHeight), 1.0f) 
                                           : fclamp(0.0f, mousePos.x / (float)wxWidth, 1.0f);
                this->onSliderPosChanged();
            }
            break;
    }
}
