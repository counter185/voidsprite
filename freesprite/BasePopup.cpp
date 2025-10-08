#include "BasePopup.h"
#include "EventCallbackListener.h"
#include "UILabel.h"
#include "UIButton.h"

void BasePopup::renderDefaultBackground() {

    int topBarY = g_windowH / 5;
    int bottomBarY = g_windowH / 5 * 4;

    SDL_SetRenderDrawColor(g_rd, 0, 0, 0, (uint8_t)(0x60 * startTimer.percentElapsedTime(300)));
    //SDL_RenderFillRect(g_rd, NULL);
    renderGradient({ 0,0,g_windowW,g_windowH / 2 }, 0x70000000, 0x70000000, 0xFF000000, 0xFF000000);
    renderGradient({ 0,g_windowH/2,g_windowW,g_windowH / 2 }, 0xFF000000, 0xFF000000, 0x70000000,0x70000000);

    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xA0);
    drawLine({ 0, topBarY }, { g_windowW, topBarY }, XM1PW3P1(startTimer.percentElapsedTime(700)));
    drawLine({ g_windowW, bottomBarY }, { 0, bottomBarY }, XM1PW3P1(startTimer.percentElapsedTime(700)));

    int offset = 1;

    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x60);
    drawLine({ 0, topBarY - offset }, { g_windowW, topBarY - offset }, XM1PW3P1(startTimer.percentElapsedTime(700)));
    drawLine({ g_windowW, bottomBarY + offset }, { 0, bottomBarY + offset }, XM1PW3P1(startTimer.percentElapsedTime(700)));
    offset++;
    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x20);
    drawLine({ 0, topBarY - offset }, { g_windowW, topBarY - offset }, XM1PW3P1(startTimer.percentElapsedTime(700)));
    drawLine({ g_windowW, bottomBarY + offset }, { 0, bottomBarY + offset }, XM1PW3P1(startTimer.percentElapsedTime(700)));
    offset++;
    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x0a);
    drawLine({ 0, topBarY - offset }, { g_windowW, topBarY - offset }, XM1PW3P1(startTimer.percentElapsedTime(700)));
    drawLine({ g_windowW, bottomBarY + offset }, { 0, bottomBarY + offset }, XM1PW3P1(startTimer.percentElapsedTime(700)));

    XY origin = getPopupOrigin();
    SDL_Rect bgRect = SDL_Rect{ origin.x, origin.y, wxWidth, (int)(wxHeight * XM1PW3P1(startTimer.percentElapsedTime(300))) };

    static Fill popupBgFill = visualConfigFill("popup/bg");
    popupBgFill.fill(bgRect);

    u8 alpha = 0x30;
    SDL_Rect bgRect2 = bgRect;
    for (int i = 0; i < 3; i++) {
        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, alpha * XM1PW3P1(startTimer.percentElapsedTime(300)));
        SDL_RenderDrawRect(g_rd, &bgRect2);
        bgRect2.x--;
        bgRect2.y--;
        bgRect2.w += 2;
        bgRect2.h += 2;
        alpha /= 3;
        alpha *= 2;
    }
}

void BasePopup::playPopupCloseVFX()
{
    g_newVFX(VFX_POPUPCLOSE, 300, 0xFF000000, SDL_Rect{ getPopupOrigin().x, getPopupOrigin().y, wxWidth, wxHeight });
}

void BasePopup::closePopup() {
    playPopupCloseVFX();
    g_closePopup(this, false);
    if (callback != NULL) {
        callback->eventPopupClosed(callback_id, this);
    }
    delete this;
}

XY BasePopup::makeTitleAndDesc(std::string title, std::string desc) {
    XY titlePos = { 5,5 };
    XY contentPos = { 5,50 };

    XY returnEndpoint = { 0,0 };

    if (title != "") {
        UILabel* titleLbl = new UILabel(title);
        titleLbl->position = titlePos;
        titleLbl->fontsize = 22;
        wxsManager.addDrawable(titleLbl);
        returnEndpoint = titleLbl->calcEndpoint();
        returnEndpoint.y += 20;
    }

    if (desc != "") {
        UILabel* descLbl = new UILabel(desc);
        descLbl->position = contentPos;
        wxsManager.addDrawable(descLbl);
        returnEndpoint = descLbl->calcEndpoint();
        returnEndpoint.y += 20;
    }

    return returnEndpoint;
}

UIButton* BasePopup::actionButton(std::string text, int width) {
    if (!actionButtonXInit) {
        nextActionButtonX = wxWidth - (width + 10);
        actionButtonXInit = true;
    }
    UIButton* nbutton = new UIButton();
    nbutton->text = text;
    nbutton->position = XY{ nextActionButtonX, wxHeight - 40 };
    nextActionButtonX -= width + 10;
    nbutton->wxWidth = width;
    wxsManager.addDrawable(nbutton);
    return nbutton;
}
