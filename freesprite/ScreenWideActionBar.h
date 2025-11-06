#pragma once
#include "Panel.h"

class ScreenWideActionBar :
    public Panel
{
public:
    ScreenWideActionBar(std::vector<Drawable*> children) 
    {
        for (auto*& child : children) {
            subWidgets.addDrawable(child);
        }
        evalHeight();
    }

    void render(XY at) override {
        position.x = 0;

        SDL_Rect bgRect = { 0,at.y, g_windowW, wxHeight };
        static Fill bg = visualConfigFill("actionbar/bg");
        bg.fill(bgRect);

        static SDL_Color separator = visualConfigColor("actionbar/separator");

        XY origin = xyAdd(at, { 5,3 });
        XY end = xyAdd(origin, { 100, 0 });
        SDL_SetRenderDrawColor(g_rd, separator.r, separator.g, separator.b, separator.a);
        SDL_RenderDrawLine(g_rd, origin.x, origin.y, end.x, end.y);

        Panel::render(at);
    }
    bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override {
        return pointInBox(mousePos, { 0, thisPositionOnScreen.y, g_windowW, wxHeight }) || subWidgets.mouseInAny(thisPositionOnScreen, mousePos);
    }

    void addDrawable(Drawable* d) {
        subWidgets.addDrawable(d);
        evalHeight();
    }
    void removeDrawable(Drawable* d) {
        subWidgets.removeDrawable(d);
        evalHeight();
    }

    void evalHeight() {
        int h = 0;
        for (auto*& child : subWidgets.drawablesList) {
            h = ixmax(h, child->position.y + child->getDimensions().y);
        }
        wxHeight = h;
    }
};

