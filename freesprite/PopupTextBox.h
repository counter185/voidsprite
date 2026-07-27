#pragma once
#include "BasePopup.h"
#include "EventCallbackListener.h"
#include "UITextField.h"

class PopupTextBox : public BasePopup
{
protected:
    void accept() {
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
    void close() {
        closePopup();
    }
public:

    UITextField* tbox;
    std::function<void(PopupTextBox*, std::string)> onTextInputConfirmedCallback = NULL;
    bool allowEmptyText = false;

    PopupTextBox(std::string tt, std::string tx, std::string defaultValue = "", int textFieldWidth = 320);

    void setMultiline(int textBoxHeight = 90) {
        tbox->multiline = true;
        tbox->wxHeight = textBoxHeight;
    }
};

