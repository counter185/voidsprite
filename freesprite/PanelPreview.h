#pragma once
#include "PanelUserInteractable.h"
#include "Canvas.h"

class PanelPreview :
    public PanelUserInteractable
{
private:
    MainEditor* parent;
public:
    Canvas c;
    int dragging = 0;

    PanelPreview(MainEditor* t);

    bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override;
    bool defaultInputAction(SDL_Event evt, XY gPosOffset) override;

    void renderAfterBG(XY at) override;

    void initWidgets();
    void renderViewportBound(XY at);
    SDL_Rect getCanvasDrawRect(XY at);
};

