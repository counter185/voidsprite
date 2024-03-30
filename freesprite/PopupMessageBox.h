#pragma once
#include "BasePopup.h"
#include "UIButton.h"
#include "EventCallbackListener.h"

class PopupMessageBox :
    public BasePopup, public EventCallbackListener
{
public:
    std::string title = "";
    std::string text = "";

    PopupMessageBox(std::string tt, std::string tx) {
        this->title = tt;
        this->text = tx;
        wxHeight = 200;
        UIButton* nbutton = new UIButton();
        nbutton->text = "OK";
        nbutton->position = XY{ wxWidth - 130, wxHeight - 40 };
        nbutton->wxHeight = 35;
        nbutton->wxWidth = 120;
        nbutton->setCallbackListener(0, this);
        wxsManager.addDrawable(nbutton);
    }

    void render() override;

    void eventButtonPressed(int evt_id) override {
        closePopup();
    }
};

