#pragma once
#include "BasePopup.h"
#include "UIButton.h"
#include "EventCallbackListener.h"

class PopupYesNo :
    public BasePopup, public EventCallbackListener
{
public:
    bool result = false;

    PopupYesNo(std::string tt, std::string tx) {

        wxHeight = 200;

        UIButton* nbutton = actionButton(TL("vsp.cmn.no"));
        nbutton->setCallbackListener(false, this);

        UIButton* ybutton = actionButton(TL("vsp.cmn.yes"));
        ybutton->setCallbackListener(true, this);

        makeTitleAndDesc(tt, tx);
    }

    void eventButtonPressed(int evt_id) override {
        result = evt_id;
        closePopup();
    }
};

