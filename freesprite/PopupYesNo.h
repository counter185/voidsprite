#pragma once
#include "BasePopup.h"
#include "UIButton.h"
#include "EventCallbackListener.h"
#include "FontRenderer.h"

class PopupYesNo :
    public BasePopup, public EventCallbackListener
{
public:
    bool result = false;

    std::function<void(PopupYesNo*, bool)> onFinishCallback = NULL;

    PopupYesNo(std::string tt, std::string tx) {

        wxHeight = 200;

        makeTitleAndDesc(tt, tx);

        wxWidth = ixmax(wxWidth, ixmax(g_fnt->StatStringDimensions(tx).x + 20, g_fnt->StatStringDimensions(tt, 22).x + 20));

        UIButton* nbutton = actionButton(TL("vsp.cmn.no"));
        nbutton->setCallbackListener(false, this);

        UIButton* ybutton = actionButton(TL("vsp.cmn.yes"));
        ybutton->setCallbackListener(true, this);
    }

    void eventButtonPressed(int evt_id) override {
        result = evt_id;
        if (onFinishCallback != NULL) {
            onFinishCallback(this, evt_id == 1);
        }
        closePopup();
    }
};

