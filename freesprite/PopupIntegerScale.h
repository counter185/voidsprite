#pragma once
#include "PopupTileGeneric.h"
class PopupIntegerScale :
    public PopupTileGeneric
{
public:
    UICheckbox* downscaleCheckbox;
    UILabel* outputScaleLabel;
    XY sizeNow;

    PopupIntegerScale(EventCallbackListener* callback, std::string tt, std::string tx, XY sizeNow, XY defaultValues, int event_id, bool allowDownscale = true);

    void resultUpdated(XY result) override;

    bool inputValid() override;
};

