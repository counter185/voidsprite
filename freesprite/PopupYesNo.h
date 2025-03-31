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

        UIButton* nbutton = new UIButton();
        nbutton->text = TL("vsp.cmn.no");
        nbutton->position = XY{ wxWidth - 130, wxHeight - 40 };
        nbutton->wxHeight = 35;
        nbutton->wxWidth = 120;
        nbutton->setCallbackListener(false, this);
        wxsManager.addDrawable(nbutton);

        UIButton* ybutton = new UIButton();
        ybutton->text = TL("vsp.cmn.yes");
        ybutton->position = XY{ wxWidth - 260, wxHeight - 40 };
        ybutton->wxHeight = 35;
        ybutton->wxWidth = 120;
        ybutton->setCallbackListener(true, this);
        wxsManager.addDrawable(ybutton);

        makeTitleAndDesc(tt, tx);
    }

    void eventButtonPressed(int evt_id) override {
        result = evt_id;
        closePopup();
    }
};

