#include "PopupCanvasResize.h"

PopupCanvasResize::PopupCanvasResize(EventCallbackListener* callback, std::string tt, std::string tx, XY defaultValues, int event_id)
	: PopupTileGeneric(callback, tt, tx, defaultValues, event_id)
{
	anchorSelect = new UIAnchorSelect();
	anchorSelect->position = XY{ 20, 120 };
	wxsManager.addDrawable(anchorSelect);
}
