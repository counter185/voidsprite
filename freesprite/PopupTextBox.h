#pragma once
#include "BasePopup.h"
#include "EventCallbackListener.h"
#include "UITextField.h"

class PopupTextBox : public BasePopup, public EventCallbackListener
{
public:
	std::string title = "";
	std::string text = "";

	UITextField* tbox;

	PopupTextBox(std::string tt, std::string tx, int textFieldWidth = 220);

	void render() override;

    void eventButtonPressed(int evt_id) override {
        if (evt_id == 0) {
            if (!tbox->text.empty()) {
                if (callback != NULL) {
                    callback->eventTextInputConfirm(callback_id, tbox->text);
                }
                closePopup();
            }
        }
        else {
            closePopup();
        }

    }
};

