#pragma once
#include "Panel.h"
#include "Canvas.h"
#include "EventCallbackListener.h"

class PanelReference : public Panel, public EventCallbackListener
{
private:
    Layer* previewTex = NULL;
public:
    Canvas c;
    int dragging = 0;

    int currentMode = 0;

    PanelReference(Layer* t);
    ~PanelReference();

    bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override;
    void handleInput(SDL_Event evt, XY gPosOffset) override;

    void render(XY at) override;

    void eventDropdownItemSelected(int evt_id, int index, std::string name) override;

    void initWidgets();
    SDL_Rect getCanvasDrawRect(XY at);
};

