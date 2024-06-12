#pragma once
#include "BasePopup.h"
#include "UIButton.h"
#include "EventCallbackListener.h"
#include "UIColorInputField.h"

class PopupSetColorKey :
    public BasePopup, public EventCallbackListener
{
public:
    std::string title = "";
    std::string text = "";
    Layer* target;

    UIColorInputField* colorInput;

    PopupSetColorKey(Layer* target, std::string tt, std::string tx) {
        this->title = tt;
        this->text = tx;
        this->target = target;
        wxHeight = 200;

        colorInput = new UIColorInputField();
        colorInput->position = XY{ 30, wxHeight / 2 };
        colorInput->wxWidth = 200;
        colorInput->wxHeight = 30;
        wxsManager.addDrawable(colorInput);

        UIButton* nbutton = new UIButton();
        nbutton->text = "Set";
        nbutton->position = XY{ wxWidth - 260, wxHeight - 40 };
        nbutton->wxWidth = 120;
        nbutton->setCallbackListener(0, this);
        wxsManager.addDrawable(nbutton);

        UIButton* nbutton2 = new UIButton();
        nbutton2->text = "Cancel";
        nbutton2->position = XY{ wxWidth - 130, wxHeight - 40 };
        nbutton2->wxWidth = 120;
        nbutton2->setCallbackListener(1, this);
        wxsManager.addDrawable(nbutton2);
    }

    void render() override;

    void eventButtonPressed(int evt_id) override;
};

