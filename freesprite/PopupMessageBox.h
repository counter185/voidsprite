#pragma once
#include "BasePopup.h"
#include "UIButton.h"
#include "EventCallbackListener.h"

class PopupMessageBox :
    public BasePopup, public EventCallbackListener
{
public:
    PopupMessageBox(std::string tt, std::string tx, XY size = {600, 200}) {

        setSize(size);
        UIButton* nbutton = new UIButton();
        nbutton->text = "OK";
        nbutton->position = XY{ wxWidth - 130, wxHeight - 40 };
        nbutton->wxHeight = 35;
        nbutton->wxWidth = 120;
        nbutton->setCallbackListener(0, this);
        wxsManager.addDrawable(nbutton);

        makeTitleAndDesc(tt, tx);
    }

    void eventButtonPressed(int evt_id) override {
        closePopup();
    }
};

