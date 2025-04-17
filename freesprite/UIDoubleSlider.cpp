#include "UIDoubleSlider.h"
#include "globals.h"
#include "mathops.h"

void UIDoubleSlider::drawPosIndicator(XY origin) {
    //g_fnt->RenderString(std::string("pos:") + std::to_string(sliderPos), origin.x + 10, origin.y + wxHeight / 4);

    Fill::Gradient(bodyColor, bodyColor, 0, 0).fill(SDL_Rect {
        .x = origin.x + (int) (wxWidth * sliderPos.min) + 1,
        .y = origin.y + 2,
        .w = (int) (wxWidth * sliderPos.max) - (int) (wxWidth * sliderPos.min) - 1,
        .h = wxHeight - 4,
    });

    // Least
    {
        XY centerPoint = xyAdd(origin, XY{ (int)(wxWidth * sliderPos.min), 0});
        int xdist = 3;
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 255);
        
        for (int x = 0; x < 2; x++) {
            int fxdist = xdist + x;    
            SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y - fxdist, centerPoint.x - fxdist, centerPoint.y + wxHeight + fxdist);
            //SDL_RenderDrawLine(g_rd, centerPoint.x + fxdist, centerPoint.y - fxdist, centerPoint.x + fxdist, centerPoint.y + wxHeight + fxdist);
            //draws first triangle
            SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y - fxdist, centerPoint.x, centerPoint.y);
            //SDL_RenderDrawLine(g_rd, centerPoint.x + fxdist, centerPoint.y - fxdist, centerPoint.x, centerPoint.y);
            //draws top/bottom bars
            //SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y + wxHeight + fxdist, centerPoint.x, centerPoint.y + wxHeight + fxdist);
            //SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y - fxdist, centerPoint.x, centerPoint.y - fxdist);
            SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y + wxHeight + fxdist, centerPoint.x + fxdist, centerPoint.y + wxHeight + fxdist);
            SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y - fxdist, centerPoint.x + fxdist, centerPoint.y - fxdist);
            //draws second triangle
            SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y + wxHeight + fxdist, centerPoint.x, centerPoint.y + wxHeight);
            //SDL_RenderDrawLine(g_rd, centerPoint.x + fxdist, centerPoint.y + wxHeight + fxdist, centerPoint.x, centerPoint.y + wxHeight);
            SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 255);
        }
    }

    // Most
    {
        XY centerPoint = xyAdd(origin, XY{ (int)(wxWidth * sliderPos.max), 0});
        int xdist = 3;
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 255);
        
        for (int x = 0; x < 2; x++) {
            int fxdist = xdist + x;    
            //SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y - fxdist, centerPoint.x - fxdist, centerPoint.y + wxHeight + fxdist);
            SDL_RenderDrawLine(g_rd, centerPoint.x + fxdist, centerPoint.y - fxdist, centerPoint.x + fxdist, centerPoint.y + wxHeight + fxdist);
            //draws first triangle
            //SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y - fxdist, centerPoint.x, centerPoint.y);
            SDL_RenderDrawLine(g_rd, centerPoint.x + fxdist, centerPoint.y - fxdist, centerPoint.x, centerPoint.y);
            //draws top/bottom bars
            //SDL_RenderDrawLine(g_rd, centerPoint.x, centerPoint.y + wxHeight + fxdist, centerPoint.x + fxdist, centerPoint.y + wxHeight + fxdist);
            //SDL_RenderDrawLine(g_rd, centerPoint.x, centerPoint.y - fxdist, centerPoint.x + fxdist, centerPoint.y - fxdist);
            SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y + wxHeight + fxdist, centerPoint.x + fxdist, centerPoint.y + wxHeight + fxdist);
            SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y - fxdist, centerPoint.x + fxdist, centerPoint.y - fxdist);
            //draws second triangle
            //SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y + wxHeight + fxdist, centerPoint.x, centerPoint.y + wxHeight);
            SDL_RenderDrawLine(g_rd, centerPoint.x + fxdist, centerPoint.y + wxHeight + fxdist, centerPoint.x, centerPoint.y + wxHeight);
            SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 255);
        }
    }
}

void UIDoubleSlider::render(XY pos)
{
    SDL_Rect drawrect = { pos.x, pos.y, wxWidth, wxHeight };
    SDL_SetRenderDrawColor(g_rd, 0x0, 0x0, 0x0, focused ? 0xff : 0x80);
    SDL_RenderFillRect(g_rd, &drawrect);
    SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, focused ? 0x80 : 0x30);
    SDL_RenderDrawRect(g_rd, &drawrect);
    drawPosIndicator(pos);
}

void UIDoubleSlider::handleInput(SDL_Event evt, XY gPosOffset)
{
    switch (evt.type) {
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            if (evt.button.button == 1) {
                if (evt.button.down) {
                    XY mousePos = xySubtract(XY{ (int)evt.button.x, (int)evt.button.y }, gPosOffset);
                    if (mousePos.y >= 0 && mousePos.y <= wxHeight) {
                        double newPos = fclamp(0.0f, mousePos.x / (float)wxWidth, 1.0f);

                        focusMost = std::abs(newPos - sliderPos.max) < std::abs(newPos - sliderPos.min);

                        if (focusMost) sliderPos.max = newPos; else sliderPos.min = newPos;
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
                if (mousePos.y >= 0 && mousePos.y <= wxHeight) {
                    double newPos = fclamp(0.0f, mousePos.x / (float)wxWidth, 1.0f);
                    if (focusMost ? newPos < sliderPos.min : newPos > sliderPos.max) {
                        double t = sliderPos.min;
                        sliderPos.min = sliderPos.max;
                        sliderPos.max = t;
                        focusMost =! focusMost;
                    }
                    if (focusMost) sliderPos.max = newPos; else sliderPos.min = newPos;
                    this->onSliderPosChanged();
                }
            }
            break;
    }
}
