#pragma once
#include "PopupTileGeneric.h"
class PopupIntegerScale :
    public PopupTileGeneric
{
public:
    UICheckbox* downscaleCheckbox;

    PopupIntegerScale(EventCallbackListener* callback, std::string tt, std::string tx, XY defaultValues, int event_id);
};

