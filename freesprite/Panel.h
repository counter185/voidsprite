#pragma once
#include "drawable.h"
#include "DrawableManager.h"

class Panel :
    public Drawable
{
public:
    int wxWidth = 0;
    int wxHeight = 0;
    bool enabled = true;
    DrawableManager subWidgets;

    bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override;
    void render(XY position) override;
    void handleInput(SDL_Event evt, XY gPosOffset) override;
    void focusOut() override {
        Drawable::focusOut();
        subWidgets.forceUnfocus();
    }
    void mouseHoverOut() override {
        Drawable::mouseHoverOut();
        subWidgets.forceUnhover();
    }
    void mouseHoverMotion(XY mousePos, XY gPosOffset) override;
    bool isPanel() override { return true; }

    XY getDimensions() override { return XY{ wxWidth,wxHeight }; };
};

