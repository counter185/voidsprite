#pragma once
#include "drawable.h"
#include "DrawableManager.h"
#include "Panel.h"

class DraggablePanel :
    public Panel
{
protected:
    bool dragging = false;
    bool wasDragged = false;
public:
    void handleInput(SDL_Event evt, XY gPosOffset) override;

    void processDrag(SDL_Event evt);
    void tryMoveOutOfOOB();
};

