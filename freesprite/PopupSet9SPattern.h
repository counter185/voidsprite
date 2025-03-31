#pragma once
#include "BasePopup.h"
#include "EventCallbackListener.h"

class PopupSet9SPattern :
    public BasePopup, public EventCallbackListener
{
public:
    PopupSet9SPattern();

    void eventButtonPressed(int evt_id) override;
};

