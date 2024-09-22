#pragma once
#include "drawable.h"
#include "DrawableManager.h"
class DraggablePanel :
    public Drawable
{
public:
    int wxWidth = 400;
    int wxHeight = 390;
    DrawableManager subWidgets;

    bool dragging = false;

    bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override;
    void render(XY position) override;
    void handleInput(SDL_Event evt, XY gPosOffset) override;
    void mouseHoverMotion(XY mousePos, XY gPosOffset) override;
    void focusOut() override {
        Drawable::focusOut();
        subWidgets.forceUnfocus();
    }
    XY getDimensions() override { return XY{ wxWidth,wxHeight }; };

    void processDrag(SDL_Event evt);
};

