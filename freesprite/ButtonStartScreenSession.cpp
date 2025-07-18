#include "ButtonStartScreenSession.h"
#include "main.h"
#include "Notification.h"
#include "BaseScreen.h"
#include "TooltipsLayer.h"
#include "FontRenderer.h"
#include "multiwindow.h"

void ButtonStartScreenSession::render(XY pos)
{
    if (correspondingScreen >= 0 && correspondingScreen < targetWindow->screenStack.size()) {
        tooltip = targetWindow->screenStack[correspondingScreen]->getName();
    }

    int squareW = (int)(wxWidth * (screenSwitchTimer.started && correspondingScreen == targetWindow->currentScreen ? XM1PW3P1(screenSwitchTimer.percentElapsedTime(300)) : 1));
    SDL_Rect r = {
        pos.x,
        pos.y,
        squareW, squareW
    };

    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, correspondingScreen == targetWindow->currentScreen ? 0x80 : 0x20);
    if (favourite && correspondingScreen == fav_screen) {
        SDL_SetRenderDrawColor(g_rd, 0, 255, 0, correspondingScreen == targetWindow->currentScreen ? 0x80 : 0x20);

    }
    SDL_RenderFillRect(g_rd, &r);

    renderAnimations(pos);
    renderTooltip(pos);
}

void ButtonStartScreenSession::click()
{
    if (targetWindow->popupStack.empty()) {
        if (correspondingScreen >= 0 && correspondingScreen < targetWindow->screenStack.size()) {
            g_switchScreen(correspondingScreen);
        }
        else {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "this should never happen"));
        }
    }
    UIButton::click();
}

void ButtonStartScreenSession::renderTooltip(XY pos)
{
    if (targetWindow->popupStack.empty()) {
        if (hovered) {
            if (!tooltip.empty() && (instantTooltip || hoverTimer.percentElapsedTime(1000) == 1.0f)) {
                XY bounds = g_fnt->StatStringDimensions(tooltip);
                g_ttp->addTooltip(Tooltip{ xySubtract(pos, {0, bounds.y + 14}), tooltip, {255,255,255,255}, hoverTimer.percentElapsedTime(300, instantTooltip ? 0 : 1000) });
            }
            if (screenPreviewFramebuffer != NULL && targetWindow->currentScreen != correspondingScreen) {
                g_pushRenderTarget(screenPreviewFramebuffer);
                SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0);
                SDL_RenderClear(g_rd);
                g_ttp->takeTooltips = false;
                targetWindow->screenStack[correspondingScreen]->render();
                g_ttp->takeTooltips = true;
                g_popRenderTarget();

                int previewW = 300;
                double animTimer = XM1PW3P1(hoverTimer.percentElapsedTime(200));
                double animTimerLonger = XM1PW3P1(hoverTimer.percentElapsedTime(600));
                int previewH = (int)(previewW / ((float)g_windowW / g_windowH) * animTimer);
                SDL_SetTextureAlphaMod(screenPreviewFramebuffer, 255 * hoverTimer.percentElapsedTime(200));
                SDL_Rect r = { ixmin(position.x, g_windowW - previewW), position.y - 30 - previewH, previewW, previewH };

                SDL_RenderCopy(g_rd, screenPreviewFramebuffer, NULL, &r);

                SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x80);
                drawLine({ r.x, r.y }, { r.x + r.w, r.y }, animTimerLonger);
                drawLine({ r.x, r.y }, { r.x, r.y + r.h }, animTimerLonger);
            }
        }
    }
}
