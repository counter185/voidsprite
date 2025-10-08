#include "PopupIntegerScale.h"
#include "UICheckbox.h"
#include "UILabel.h"

PopupIntegerScale::PopupIntegerScale(EventCallbackListener* callback, std::string tt, std::string tx, XY sizeNow, XY defaultValues, int event_id, bool allowDownscale)
    : PopupTileGeneric(callback, tt, tx, defaultValues, event_id)
{
    this->sizeNow = sizeNow;

    downscaleCheckbox = new UICheckbox("Downscale", false);
    downscaleCheckbox->position = { 15, 125 };
    downscaleCheckbox->onStateChangeCallback = [this](UICheckbox* c, bool state) {
        resultUpdated(result);
    };
    wxsManager.addDrawable(downscaleCheckbox);
    downscaleCheckbox->enabled = allowDownscale;

    outputScaleLabel = new UILabel("");
    outputScaleLabel->position = { 5, wxHeight - 5 - 20 };
    outputScaleLabel->fontsize = 16;
    outputScaleLabel->color = { 0xff, 0xff, 0xff, 0x80 };
    wxsManager.addDrawable(outputScaleLabel);
    resultUpdated(defaultValues);
}

void PopupIntegerScale::resultUpdated(XY result)
{
    XY outSize = downscaleCheckbox->isChecked() ?
        ((inputValid() && (sizeNow.x % result.x == 0) && (sizeNow.y % result.y == 0)) ? XY{sizeNow.x / result.x, sizeNow.y / result.y} : XY{-1,-1})
        : XY{ sizeNow.x * result.x, sizeNow.y * result.y };

    if (outSize.x == -1 || outSize.y == -1) {
        outputScaleLabel->setText(frmt("<invalid size>", outSize.x, outSize.y));
    }
    else {
        outputScaleLabel->setText(frmt("Output size: {}x{}", outSize.x, outSize.y));
    }
}

bool PopupIntegerScale::inputValid()
{
    return result.x != 0 && result.y != 0;
}
