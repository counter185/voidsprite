#pragma once
#include "Panel.h"
#include "Canvas.h"

class PanelPreview :
    public Panel
{
private:
    MainEditor* parent;
public:
    Canvas c;
    int dragging = 0;

    PanelPreview(MainEditor* t);

    bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override;
    void handleInput(SDL_Event evt, XY gPosOffset) override;

    void render(XY at) override;

    void initWidgets();
    void renderViewportBound(XY at);
    SDL_Rect getCanvasDrawRect(XY at);
};

