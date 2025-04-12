#pragma once
#include "Panel.h"
#include "UIButton.h"
#include "Timer64.h"
#include "EventCallbackListener.h"

class UICheckboxButton : public UIButton {
private:
    bool state = false;
    bool* statePtr = NULL;
public:
    Timer64 stateChangeTimer;

    UICheckboxButton(bool* linkTo) : statePtr(linkTo) {
        wxHeight = wxWidth = 30;
        icon = NULL;
        text = "";
        stateChangeTimer.start();
    }

    UICheckboxButton(bool defaultState = false) : UICheckboxButton(&state) {
        state = defaultState;
    }
    void render(XY pos) override;

    void click() override;

    bool isChecked() { return *statePtr; }
    void setChecked(bool s) { 
        if (s != isChecked()) {
            stateChangeTimer.start();
        }
        *statePtr = s;
    }
};

class UICheckbox :
    public Panel, public EventCallbackListener
{
private:
    UICheckbox(std::string text);
public:
    UILabel* label;
    UICheckboxButton* checkbox;

	std::function<void(UICheckbox*, bool)> onStateChangeCallback = NULL;

    UICheckbox(std::string text, bool* linkTo);
    UICheckbox(std::string text, bool defaultState);

    bool isChecked() {
        return checkbox->isChecked();
    }

    void eventButtonPressed(int evt_id) override {
		if (evt_id == 0) {
			if (onStateChangeCallback != NULL) {
				onStateChangeCallback(this, isChecked());
			}
            else if (callback != NULL) {
                callback->eventCheckboxToggled(callback_id, isChecked());
            }
		}
	}
};

