#include "DraggablePanel.h"

void DraggablePanel::handleInput(SDL_Event evt, XY gPosOffset)
{
    if (!DrawableManager::processInputEventInMultiple({ subWidgets }, evt, gPosOffset)) {
        processDrag(evt);
    }
}

void DraggablePanel::processDrag(SDL_Event evt)
{
    if (passThroughMouse) {
        return;
    }
    switch (evt.type) {
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        if (evt.button.button == SDL_BUTTON_LEFT) {
            dragging = evt.button.down;
        }
        break;
    case SDL_MOUSEMOTION:
        if (dragging) {
            wasDragged = true;
            position.x += (int)(evt.motion.xrel);
            position.y += (int)(evt.motion.yrel);

            tryMoveOutOfOOB();
        }
        break;
    }
}

void DraggablePanel::tryMoveOutOfOOB()
{
    if (position.x < 0) {
        position.x = 0;
    }
    if (position.y < 0) {
        position.y = 0;
    }
    if (position.x + wxWidth > g_windowW) {
        position.x = g_windowW - wxWidth;
    }
    if (position.y + wxHeight > g_windowH) {
        position.y = g_windowH - wxHeight;
    }
}
