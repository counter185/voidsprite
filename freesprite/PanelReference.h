#pragma once
#include "Panel.h"
#include "Canvas.h"
#include "EventCallbackListener.h"

enum ReferencePanelMode {
    REFERENCE_PIXEL_PERFECT = 0,
    REFERENCE_FIT = 1,
    REFERENCE_UNDER_CANVAS = 2,
    REFERENCE_OVER_CANVAS = 3,
};

class PanelReference : public Panel, public EventCallbackListener
{
private:
    Layer* previewTex = NULL;
public:
    Canvas c;
    int dragging = 0;
    float opacity = 0.4f;

    ReferencePanelMode currentMode = REFERENCE_PIXEL_PERFECT;

    PanelReference(Layer* t);
    ~PanelReference();

    bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override;
    void handleInput(SDL_Event evt, XY gPosOffset) override;

    void render(XY at) override;

    void eventDropdownItemSelected(int evt_id, int index, std::string name) override;

    void initWidgets();
    SDL_Rect getCanvasDrawRect(XY at);
    Layer* getLayer() { return previewTex; }
};

