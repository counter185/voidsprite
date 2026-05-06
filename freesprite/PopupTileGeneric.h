#pragma once
#include "BasePopup.h"
#include "EventCallbackListener.h"
#include "UITextField.h"
#include "UIButton.h"

class PopupTileGeneric :
    public BasePopup
{
public:


    XY result{};
    int popupEvtID;

    EventCallbackListener* callback;

    PopupTileGeneric(EventCallbackListener* callback, std::string tt, std::string tx, XY defaultValues, int event_id);

    virtual void resultUpdated(XY) {}

    virtual bool xValid(int v) { return true; }
    virtual bool yValid(int v) { return true; }
};

