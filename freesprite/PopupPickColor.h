#pragma once
#include "globals.h"
#include "BasePopup.h"
#include "EventCallbackListener.h"


class PopupPickColor :
    public BasePopup, public EventCallbackListener
{
public:
    std::string title = "";
    std::string text = "";

    UIColorInputField* colorInput;

    PopupPickColor(std::string tt, std::string tx);

    void render() override;

    void eventButtonPressed(int evt_id) override;
};

