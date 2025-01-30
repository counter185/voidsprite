#include "BasePopup.h"
#include "EventCallbackListener.h"

void BasePopup::renderDefaultBackground(SDL_Color bgColor) {

    int topBarY = g_windowH / 5;
    int bottomBarY = g_windowH / 5 * 4;

    SDL_SetRenderDrawColor(g_rd, 0, 0, 0, (uint8_t)(0x60 * startTimer.percentElapsedTime(300)));
    //SDL_RenderFillRect(g_rd, NULL);
    renderGradient({ 0,0,g_windowW,g_windowH / 2 }, 0x70000000, 0x70000000, 0xFF000000, 0xFF000000);
    renderGradient({ 0,g_windowH/2,g_windowW,g_windowH / 2 }, 0xFF000000, 0xFF000000, 0x70000000,0x70000000);

    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x90);
    drawLine({ 0, topBarY }, { g_windowW, topBarY }, XM1PW3P1(startTimer.percentElapsedTime(700)));
    drawLine({ g_windowW, bottomBarY }, { 0, bottomBarY }, XM1PW3P1(startTimer.percentElapsedTime(700)));

    XY origin = getPopupOrigin();
    SDL_Rect bgRect = SDL_Rect{ origin.x, origin.y, wxWidth, (int)(wxHeight * XM1PW3P1(startTimer.percentElapsedTime(300))) };
    SDL_SetRenderDrawColor(g_rd, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(g_rd, &bgRect);
    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x30 * XM1PW3P1(startTimer.percentElapsedTime(300)));
    SDL_RenderDrawRect(g_rd, &bgRect);
}

void BasePopup::closePopup() {
    g_popDisposeLastPopup(false);
    if (callback != NULL) {
        callback->eventPopupClosed(callback_id, this);
    }
    delete this;
}
