#pragma once
#include "PopupChooseAction.h"
#include "UIButton.h"
#include "FontRenderer.h"
#include "UILabel.h"

class PopupYesNo :
    public PopupChooseAction
{
public:
    bool result = false;

    std::function<void(PopupYesNo*, bool)> onFinishCallback = NULL;

    PopupYesNo(std::string tt, std::string tx) : PopupChooseAction(tt, tx, {
            {SDL_SCANCODE_N, {TL("vsp.cmn.no"), [this]() { finish(false); }}},
            {SDL_SCANCODE_Y, {TL("vsp.cmn.yes"), [this]() { finish(true); }}},
        }) 
    {
    }

    void finish(bool r) {
        result = r;
        if (onFinishCallback != NULL) {
            onFinishCallback(this, result);
        }
    }
};

