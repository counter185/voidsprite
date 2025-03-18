#pragma once
#include "Panel.h"
#include "Canvas.h"

class PanelReference : public Panel
{
private:
    Layer* previewTex = NULL;
public:
    Canvas c;
    int dragging = 0;

    PanelReference(Layer* t);
    ~PanelReference();

    bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override;
    void handleInput(SDL_Event evt, XY gPosOffset) override;

    void render(XY at) override;
};

