#pragma once
#include "BasePopup.h"
#include "EventCallbackListener.h"

class PopupGlobalConfig :
    public BasePopup, public EventCallbackListener
{
private:
    GlobalConfig previousConfig;
public:
    PopupGlobalConfig();

    void render() override;

    void eventButtonPressed(int evt_id) override;
    void eventCheckboxToggled(int evt_id, bool checked) override;
    void eventTextInput(int evt_id, std::string text) override;
};

