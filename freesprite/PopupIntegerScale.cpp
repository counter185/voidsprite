#include "PopupIntegerScale.h"
#include "UICheckbox.h"

PopupIntegerScale::PopupIntegerScale(EventCallbackListener* callback, std::string tt, std::string tx, XY defaultValues, int event_id)
	: PopupTileGeneric(callback, tt, tx, defaultValues, event_id)
{
	downscaleCheckbox = new UICheckbox("Downscale", false);
	downscaleCheckbox->position = { 15, 125 };
	wxsManager.addDrawable(downscaleCheckbox);
}
