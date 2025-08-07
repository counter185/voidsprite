#pragma once
#include "globals.h"
#include "BasePopup.h"
#include "EventCallbackListener.h"


class PopupPickColor :
    public BasePopup, public EventCallbackListener
{
public:
    
    std::function<void(PopupPickColor*, u32)> onColorConfirmedCallback = NULL;

    //UIColorInputField* colorInput;
    UIColorPicker* colorPicker = NULL;
    UITextField* alphaInput = NULL;

    PopupPickColor(std::string tt, std::string tx, bool acceptAlpha = false);

    void eventTextInput(int evt_id, std::string data) override;

    void render() override;

    void updateRGBTextBoxOnInputEvent(std::string data, uint8_t* value);
    void setRGB(u32 rgb);
    void setAlpha(uint8_t a);

    uint32_t getColor();
protected:
    bool acceptAlpha = false;
    uint8_t alpha = 255;
};

