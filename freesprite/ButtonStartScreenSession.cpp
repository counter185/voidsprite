#include "ButtonStartScreenSession.h"
#include "main.h"
#include "Notification.h"
#include "BaseScreen.h"
#include "TooltipsLayer.h"
#include "FontRenderer.h"

void ButtonStartScreenSession::render(XY pos)
{
    if (correspondingScreen >= 0 && correspondingScreen < screenStack.size()) {
        tooltip = screenStack[correspondingScreen]->getName();
    }

    int squareW = (int)(wxWidth * (screenSwitchTimer.started && correspondingScreen == currentScreen ? XM1PW3P1(screenSwitchTimer.percentElapsedTime(300)) : 1));
    SDL_Rect r = {
        pos.x,
        pos.y,
        squareW, squareW
    };

    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, correspondingScreen == currentScreen ? 0x80 : 0x20);
    if (favourite && correspondingScreen == fav_screen) {
        SDL_SetRenderDrawColor(g_rd, 0, 255, 0, correspondingScreen == currentScreen ? 0x80 : 0x20);

    }
    SDL_RenderFillRect(g_rd, &r);

    renderAnimations(pos);
    renderTooltip(pos);
}

void ButtonStartScreenSession::click()
{
    if (correspondingScreen >= 0 && correspondingScreen < screenStack.size()) {
        g_switchScreen(correspondingScreen);
    }
    else {
        g_addNotification(ErrorNotification("Error", "this should never happen"));
    }
}

void ButtonStartScreenSession::renderTooltip(XY pos)
{
    if (hovered) {
        if (!tooltip.empty() && (instantTooltip || hoverTimer.percentElapsedTime(1000) == 1.0f)) {
            XY bounds = g_fnt->StatStringDimensions(tooltip);
            g_ttp->addTooltip(Tooltip{ xySubtract(pos, {0, bounds.y + 14}), tooltip, {255,255,255,255}, hoverTimer.percentElapsedTime(300, instantTooltip ? 0 : 1000) });
        }
    }
}
