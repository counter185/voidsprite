#pragma once
#include "globals.h"
#include "BasePopup.h"
#include "EventCallbackListener.h"


class PopupPickColor :
    public BasePopup, public EventCallbackListener
{
public:
    
    UIColorInputField* colorInput;
    UITextField* alphaInput = NULL;

    PopupPickColor(std::string tt, std::string tx, bool acceptAlpha = false);

    void eventButtonPressed(int evt_id) override;
    void eventTextInput(int evt_id, std::string data) override;

    void updateRGBTextBoxOnInputEvent(std::string data, uint8_t* value);
    void setAlpha(uint8_t a);

    uint32_t getColor();
protected:
    bool acceptAlpha = false;
    uint8_t alpha = 255;
};

