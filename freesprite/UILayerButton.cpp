#include "UILayerButton.h"

bool UILayerButton::isMouseIn(XY thisPositionOnScreen, XY mousePos)
{
    return subButtons.mouseInAny(thisPositionOnScreen, mousePos);
}

void UILayerButton::render(XY position)
{
    subButtons.renderAll(position);
}

void UILayerButton::handleInput(SDL_Event evt, XY gPosOffset)
{
    if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == 1 && evt.button.state) {
        subButtons.tryFocusOnPoint(XY{ evt.button.x, evt.button.y }, gPosOffset);
    }
    if (!subButtons.anyFocused()) {

    }
    else {
        subButtons.passInputToFocused(evt, gPosOffset);
    }
}

void UILayerButton::eventButtonPressed(int evt_id)
{
    if (callback == NULL) {
        return;
    }
    if (evt_id == 0) {
        callback->eventGeneric(callback_id, 0, 0);
    }
    else if (evt_id == 1) {
        callback->eventGeneric(callback_id, 1, 0);
    }
}
