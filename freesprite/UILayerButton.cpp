#include "UILayerButton.h"
#include "Layer.h"
#include "PopupContextMenu.h"
#include "UIStackPanel.h"

UILayerButton::UILayerButton(std::string mainName, Layer* linkedLayer) {
    wxWidth = 220;
    wxHeight = 30;
    sizeToContent = true;

    layer = linkedLayer;

    mainButton = new LayerActiveButton(layer);
    mainButton->text = mainName;
    mainButton->wxWidth = wxWidth - 40;
    mainButton->onClickCallback = [this](UIButton* btn) {
        if (callback != NULL) {
            callback->eventGeneric(callback_id, LAYEREVENT_SWITCH, 0);
        }
    };

    if (layer != NULL) {
        mainButton->onRightClickCallback = [this](UIButton* btn) {
            int cbid = callback_id;
            EventCallbackListener* cb = callback;
            g_openContextMenu({
                {"Rename layer", [cb,cbid]() {
                    if (cb != NULL) {
                        cb->eventGeneric(cbid, LAYEREVENT_PROMPTRENAME, 0);
                    }
                }},
                {"Duplicate layer", [cb,cbid]() {
                    if (cb != NULL) {
                        cb->eventGeneric(cbid, LAYEREVENT_DUPLICATE, 0);
                    }
                }},
                {"Delete layer", [cb,cbid]() {
                    if (cb != NULL) {
                        cb->eventGeneric(cbid, LAYEREVENT_DELETE, 0);
                    }
                }},
                {"Duplicate current variant", [cb,cbid]() {
                    if (cb != NULL) {
                        cb->eventGeneric(cbid, LAYEREVENT_VARIANT_DUPLICATECURRENT, 0);
                    }
                }}
            });
        };
    }

    hideButton = new UIButton();
    //hideButton->text = "H";
    hideButton->tooltip = "Hide";
    hideButton->icon = g_iconLayerHide;
    hideButton->wxWidth = 30;
    hideButton->onClickCallback = [this](UIButton* btn) {
        if (callback != NULL) {
            callback->eventGeneric(callback_id, LAYEREVENT_TOGGLE_HIDE, 0);
        }
    };

    subWidgets.addDrawable(UIStackPanel::Horizontal(10, {mainButton, hideButton}));

    if (layer != NULL && layer->layerData.size() > 1) {
        for (int v = 0; v < layer->layerData.size(); v++) {

            LayerVariantButton* vbtn = new LayerVariantButton(layer, v);
            vbtn->wxWidth = wxWidth - 50;
            vbtn->onClickCallback = [this, v](UIButton* btn) {
                if (callback != NULL) {
                    callback->eventGeneric(callback_id, LAYEREVENT_VARIANT_SWITCH, v);
                }
            };
            vbtn->onRightClickCallback = [this, v](UIButton* btn) {
                int cbid = callback_id;
                EventCallbackListener* cb = callback;
                g_openContextMenu({
                    {"Delete variant", [cb,cbid, v]() {
                        if (cb != NULL) {
                            cb->eventGeneric(cbid, LAYEREVENT_VARIANT_DELETE, v);
                        }
                    }},
                    {"Duplicate variant", [cb,cbid, v]() {
                        if (cb != NULL) {
                            cb->eventGeneric(cbid, LAYEREVENT_VARIANT_DUPLICATE, v);
                        }
                    }},
                });
            };

            UIButton* delbtn = new UIButton();
            delbtn->text = "-";
            delbtn->tooltip = "Delete variant";
            delbtn->wxWidth = 20;
            delbtn->wxHeight = vbtn->wxHeight;
            delbtn->onClickCallback = [this, v](UIButton* btn) {
                if (callback != NULL) {
                    callback->eventGeneric(callback_id, LAYEREVENT_VARIANT_DELETE, v);
                }
            };
            variantButtons.push_back(UIStackPanel::Horizontal(0, {
                Panel::Space(30,0),
                vbtn, 
                delbtn}
            ));
        }
        UIStackPanel* variantsStack = UIStackPanel::Vertical(0, variantButtons);
        variantsStack->position = {0, mainButton->wxHeight};
        subWidgets.addDrawable(variantsStack);
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
