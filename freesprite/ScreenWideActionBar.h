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
        renderGradient({ 0,at.y, g_windowW, wxHeight }, 0x70000000, 0x70000000, 0x40000000, 0x40000000);

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

