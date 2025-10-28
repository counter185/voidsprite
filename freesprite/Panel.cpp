#include "Panel.h"

bool Panel::isMouseIn(XY thisPositionOnScreen, XY mousePos)
{
    XY dim = getDimensions();
    if (enabled) {
        return (!passThroughMouse && pointInBox(mousePos, SDL_Rect{ thisPositionOnScreen.x, thisPositionOnScreen.y, dim.x, dim.y })) || subWidgets.mouseInAny(thisPositionOnScreen, mousePos);
    }
    return false;
}

void Panel::render(XY position)
{
    if (enabled) {
        XY renderDimensions = getRenderDimensions();
        SDL_Rect panelRect = { position.x, position.y, renderDimensions.x, renderDimensions.y };

        SDL_Color border = uint32ToSDLColor(this->borderColor);
        if (border.a != 0) {
            SDL_SetRenderDrawColor(g_rd, border.r, border.g, border.b, border.a);
            SDL_RenderDrawRect(g_rd, &panelRect);
        }
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

void Panel::mouseWheelEvent(XY mousePos, XY gPosOffset, XYf direction)
{
    if (enabled) {
        subWidgets.processMouseWheelEvent(xyAdd(gPosOffset, position), mousePos, direction);
    }
}

void Panel::renderFocusBorder(XY at, SDL_Color color, double lightup)
{
    XY dimensions = getDimensions();
    double percent = XM1PW3P1(thisOrParentFocusTimer().percentElapsedTime(300));
    int timedHeight = dimensions.y * percent;
    int timedWidth = dimensions.x * percent;

    if (lightup > 0) {
        renderFocusBorderLightup(at, color, { timedWidth, timedHeight }, lightup);
    }

    SDL_SetRenderDrawColor(g_rd, color.r, color.g, color.b, 255);
    drawLine({ at.x, at.y }, { at.x, at.y + dimensions.y }, percent);
    drawLine({ at.x, at.y }, { at.x + dimensions.x, at.y }, percent);
}

void Panel::renderFocusBorderLightup(XY at, SDL_Color c, XY size, double lightup)
{
    Fill colorGradient = Fill::Gradient(
        modAlpha(sdlcolorToUint32(c), (u8)(0x60 * lightup)),
        modAlpha(sdlcolorToUint32(c), 0),
        modAlpha(sdlcolorToUint32(c), 0),
        modAlpha(sdlcolorToUint32(c), 0)
    );
    SDL_Rect colorGradientRect = { at.x, at.y, size.x, size.y };
    colorGradient.fill(colorGradientRect);
}

void Panel::setDefaultOpaquePanelBackground()
{
    fillUnfocused = Fill::Gradient(0x90303030, 0x90101010, 0x90101010, 0x90101010);
    fillUnfocused = Fill::Gradient(0xA0303030, 0xA0101010, 0xA0101010, 0xA0101010);
}

void Panel::playPanelOpenVFX()
{
    XY dim = getDimensions();
    g_newVFX(VFX_PANELOPEN, 1000, 0, SDL_Rect{ position.x, position.y, dim.x, dim.y });
}
