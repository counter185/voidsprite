#include "UILayerButton.h"
#include "EventCallbackListener.h"

UILayerButton::UILayerButton(std::string mainName, Layer* linkedLayer) {
    wxWidth = 240;
    wxHeight = 30;

    layer = linkedLayer;

    mainButton = new LayerActiveButton(layer);
    mainButton->text = mainName;
    mainButton->position = XY{ 0,0 };
    mainButton->wxWidth = 200;
    mainButton->onClickCallback = [this](UIButton*) { 
        if (onMainButtonClickedCallback != NULL) {
            onMainButtonClickedCallback(this);
        }
        else if (callback != NULL) {
            callback->eventGeneric(callback_id, 0, 0);
        }
    };
    subWidgets.addDrawable(mainButton);

    hideButton = new UIButton();
    //hideButton->text = "H";
    hideButton->tooltip = "Hide";
    hideButton->icon = g_iconLayerHide;
    hideButton->position = XY{ mainButton->wxWidth + 10,0 };
    hideButton->wxWidth = 30;
    hideButton->onClickCallback = [this](UIButton*) { 
        if (onHideButtonClickedCallback != NULL) {
            onHideButtonClickedCallback(this);
        }
        else if (callback != NULL) {
            callback->eventGeneric(callback_id, 1, 0);
        }
    };
    subWidgets.addDrawable(hideButton);
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
