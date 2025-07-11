#include "UILayerButton.h"
#include "Layer.h"

UILayerButton::UILayerButton(std::string mainName, Layer* linkedLayer) {
    wxWidth = 240;
    wxHeight = 30;

    layer = linkedLayer;

    mainButton = new LayerActiveButton(layer);
    mainButton->text = mainName;
    mainButton->position = XY{ 0,0 };
    mainButton->wxWidth = 200;
    mainButton->setCallbackListener(0, this);
    subWidgets.addDrawable(mainButton);

    hideButton = new UIButton();
    //hideButton->text = "H";
    hideButton->tooltip = "Hide";
    hideButton->icon = g_iconLayerHide;
    hideButton->position = XY{ mainButton->wxWidth + 10,0 };
    hideButton->wxWidth = 30;
    hideButton->setCallbackListener(1, this);
    subWidgets.addDrawable(hideButton);

    if (layer != NULL) {
        int variantY = mainButton->wxHeight;
        for (int v = 0; v < layer->layerData.size(); v++) {
            LayerVariantButton* vbtn = new LayerVariantButton(layer, v);
            vbtn->position = XY{ 30, variantY };
            vbtn->wxWidth = wxWidth - 30;
            vbtn->onClickCallback = [this, v](UIButton* btn) {
                if (callback != NULL) {
                    callback->eventGeneric(callback_id, 2, v);
                }
            };
            variantButtons.push_back(vbtn);
            subWidgets.addDrawable(vbtn);

            variantY += vbtn->wxHeight;
        }
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

LayerVariantButton::LayerVariantButton(Layer* l, int variantIndex) : UIButton(), ll(l), variantIndex(variantIndex) {
    text = l->layerData[variantIndex].name;
    wxHeight = 24;
    fontSize = 14;
    if (l->currentLayerVariant == variantIndex) {
		fill = Fill::Gradient(0x00FFFFFF, 0x70FFFFFF, 0x00FFFFFF, 0x70FFFFFF);
    }
}
