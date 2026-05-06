#pragma once
#include "PopupTileGeneric.h"
#include "UIAnchorSelect.h"

class PopupCanvasResize :
    public PopupTileGeneric
{
public:
    UIAnchorSelect* anchorSelect;

    PopupCanvasResize(EventCallbackListener* callback, std::string tt, std::string tx, XY defaultValues, int event_id);
};

