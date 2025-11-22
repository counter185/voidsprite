#pragma once
#include "BasePopup.h"
#include "UIButton.h"
#include "EventCallbackListener.h"
#include "FontRenderer.h"
#include "UILabel.h"

class PopupYesNo :
    public BasePopup, public EventCallbackListener
{
protected:
    UIButton* nbutton;
    UIButton* ybutton;
public:
    bool result = false;

    std::function<void(PopupYesNo*, bool)> onFinishCallback = NULL;

    PopupYesNo(std::string tt, std::string tx) {

        wxHeight = 200;

        makeTitleAndDesc(tt, tx);

        wxWidth = ixmax(wxWidth, ixmax(g_fnt->StatStringDimensions(tx).x + 20, g_fnt->StatStringDimensions(tt, 22).x + 20));

        nbutton = actionButton(TL("vsp.cmn.no"));
        nbutton->setCallbackListener(false, this);
        UILabel* nl = new UILabel(frmt("[{}]", SDL_GetScancodeName(SDL_SCANCODE_N)), xySubtract(nbutton->position, { 0,16 }), 12);
        nl->color = uint32ToSDLColor(0x60FFFFFF);
        wxsManager.addDrawable(nl);

        ybutton = actionButton(TL("vsp.cmn.yes"));
        ybutton->setCallbackListener(true, this);
        UILabel* yl = new UILabel(frmt("[{}]", SDL_GetScancodeName(SDL_SCANCODE_Y)), xySubtract(ybutton->position, { 0,16 }), 12);
        yl->color = uint32ToSDLColor(0x60FFFFFF);
        wxsManager.addDrawable(yl);
    }

    void eventButtonPressed(int evt_id) override {
        result = evt_id;
        if (onFinishCallback != NULL) {
            onFinishCallback(this, evt_id == 1);
        }
        closePopup();
    }

    void defaultInputAction(SDL_Event evt) {
        if (evt.type == SDL_EVENT_KEY_DOWN) {
            switch (evt.key.scancode) {
                case SDL_SCANCODE_Y:
                    ybutton->click();
                    break;
                case SDL_SCANCODE_ESCAPE:
                case SDL_SCANCODE_N:
                    nbutton->click();
                    break;
            }
        }
    }
};

