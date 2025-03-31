#pragma once
#include "BasePopup.h"
#include "EventCallbackListener.h"
#include "UITextField.h"
#include "UIButton.h"

class PopupTileGeneric :
    public BasePopup, public EventCallbackListener
{
public:

    UITextField* tboxX;
    UITextField* tboxY;

    XY result;
    int popupEvtID;

    EventCallbackListener* callback;

    PopupTileGeneric(EventCallbackListener* callback, std::string tt, std::string tx, XY defaultValues, int event_id);

    void eventTextInputConfirm(int evt_id, std::string data) override;
    void eventButtonPressed(int evt_id) override;
};

