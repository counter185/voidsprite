#include "Panel.h"

bool Panel::isMouseIn(XY thisPositionOnScreen, XY mousePos)
{
    if (enabled) {
        return pointInBox(mousePos, SDL_Rect{ thisPositionOnScreen.x, thisPositionOnScreen.y, wxWidth, wxHeight }) || subWidgets.mouseInAny(thisPositionOnScreen, mousePos);
    }
    return false;
}

void Panel::render(XY position)
{
    if (enabled) {
        subWidgets.renderAll(position);
    }
}

void Panel::handleInput(SDL_Event evt, XY gPosOffset)
{
    if (enabled) {
        DrawableManager::processInputEventInMultiple({ subWidgets }, evt, gPosOffset);
    }
}

void Panel::mouseHoverMotion(XY mousePos, XY gPosOffset)
{
    if (enabled) {
        subWidgets.processHoverEvent(xyAdd(gPosOffset, position), mousePos);
    }
}
