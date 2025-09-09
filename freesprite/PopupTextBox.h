#pragma once
#include "BasePopup.h"
#include "EventCallbackListener.h"
#include "UITextField.h"

class PopupTextBox : public BasePopup, public EventCallbackListener
{
public:

    UITextField* tbox;
    std::function<void(PopupTextBox*, std::string)> onTextInputConfirmedCallback = NULL;
    bool allowEmptyText = false;

    PopupTextBox(std::string tt, std::string tx, std::string defaultValue = "", int textFieldWidth = 320);

    void eventButtonPressed(int evt_id) override {
        if (evt_id == 0) {
            if (allowEmptyText || !tbox->textEmpty()) {
                if (onTextInputConfirmedCallback != NULL) {
                    onTextInputConfirmedCallback(this, tbox->getText());
                }
                else if (callback != NULL) {
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

