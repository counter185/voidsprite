#include "UILayerButton.h"

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

void LayerActiveButton::render(XY at)
{
    if (hovered && ll != NULL) {
        SDL_Rect previewRect = { 0,0, 200, 200 };
        if (ll->w >= ll->h) {
            //horizontal
            previewRect.w = (int)(ll->w * 200.0 / ll->h);
        }
        else {
            //vertical
            previewRect.h = (int)(ll->h * 200.0 / ll->w);
        }
        previewRect.w = (int)(previewRect.w * XM1PW3P1(hoverTimer.percentElapsedTime(200)));
        previewRect.x = at.x - previewRect.w;
        previewRect.y = at.y;
        
        g_pushClip({ 0,0,g_windowW, g_windowH });
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
        SDL_RenderFillRect(g_rd, &previewRect);
        ll->render(previewRect, ll->layerAlpha);
        //SDL_RenderCopy(g_rd, ll->tex, NULL, &previewRect);
        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xa0);
        SDL_RenderDrawRect(g_rd, &previewRect);
        g_popClip();
    }
    UIButton::render(at);
}
