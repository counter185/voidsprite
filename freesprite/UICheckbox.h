#pragma once
#include "Panel.h"
#include "UIButton.h"
#include "Timer64.h"
#include "EventCallbackListener.h"

class UICheckboxButton : public UIButton {
public:
    bool state = false;
    Timer64 stateChangeTimer;

    UICheckboxButton(bool defaultState = false) {
        wxHeight = wxWidth = 30;
        icon = NULL;
        text = "";
        state = defaultState;
        stateChangeTimer.start();
    }
    void render(XY pos) override;

    void click() override;
};

class UICheckbox :
    public Panel, public EventCallbackListener
{
public:
    UILabel* label;
    UICheckboxButton* checkbox;

    UICheckbox(std::string text, bool defaultState = false);

    bool isChecked() {
        return checkbox->state;
    }

    void eventButtonPressed(int evt_id) override {
		if (evt_id == 0) {
            if (callback != NULL) {
                callback->eventCheckboxToggled(callback_id, isChecked());
            }
		}
	}
};

