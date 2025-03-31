#pragma once
#include "BasePopup.h"
#include "EventCallbackListener.h"
#include "UITextField.h"

class PopupTextBox : public BasePopup, public EventCallbackListener
{
public:

	UITextField* tbox;

	PopupTextBox(std::string tt, std::string tx, std::string defaultValue = "", int textFieldWidth = 220);

    void eventButtonPressed(int evt_id) override {
        if (evt_id == 0) {
            if (!tbox->textEmpty()) {
                if (callback != NULL) {
                    callback->eventTextInputConfirm(callback_id, tbox->getText());
                }
                closePopup();
            }
        }
        else {
            closePopup();
        }

    }

    void eventTextInputConfirm(int evt_id, std::string text) override {
        eventButtonPressed(0);
    }
};

