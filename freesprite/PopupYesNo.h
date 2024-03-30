#pragma once
#include "BasePopup.h"
#include "UIButton.h"

class PopupYesNo :
    public BasePopup, public EventCallbackListener
{
public:
    std::string title = "";
    std::string text = "";

    bool result = false;

    PopupYesNo(std::string tt, std::string tx) {
        this->title = tt;
        this->text = tx;

        wxHeight = 200;

        UIButton* nbutton = new UIButton();
        nbutton->text = "No";
        nbutton->position = XY{ wxWidth - 130, wxHeight - 40 };
        nbutton->wxHeight = 35;
        nbutton->wxWidth = 120;
        nbutton->setCallbackListener(false, this);
        wxsManager.addDrawable(nbutton);

        UIButton* ybutton = new UIButton();
        ybutton->text = "Yes";
        ybutton->position = XY{ wxWidth - 260, wxHeight - 40 };
        ybutton->wxHeight = 35;
        ybutton->wxWidth = 120;
        ybutton->setCallbackListener(true, this);
        wxsManager.addDrawable(ybutton);
    }

    void render() override;

    void eventButtonPressed(int evt_id) override {
        result = evt_id;
        closePopup();
    }
};

