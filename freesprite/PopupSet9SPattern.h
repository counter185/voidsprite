#pragma once
#include "BasePopup.h"
#include "EventCallbackListener.h"

class PopupSet9SPattern :
    public BasePopup, public EventCallbackListener
{
public:
    std::string title = "Set 9 segment pattern", text = "";

    PopupSet9SPattern();

    void render() override;

    void eventButtonPressed(int evt_id) override;
};

