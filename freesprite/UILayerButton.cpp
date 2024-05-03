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
        subButtons.tryFocusOnPoint(XY{ evt.button.x, evt.button.y }, position);
    }
    if (!subButtons.anyFocused()) {

    }
    else {
        subButtons.passInputToFocused(evt, gPosOffset);
    }
}
