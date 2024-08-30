#include "PopupTileGeneric.h"
#include "FontRenderer.h"

PopupTileGeneric::PopupTileGeneric(EventCallbackListener* callback, std::string tt, std::string tx, XY defaultValues, int event_id)
{
    wxHeight = 240;

    this->title = tt;
    this->text = tx;
    this->popupEvtID = event_id;
    this->callback = callback;

    UIButton* nbutton = new UIButton();
    nbutton->text = "Set";
    nbutton->position = XY{ wxWidth - 260, wxHeight - 40 };
    nbutton->wxWidth = 120;
    nbutton->setCallbackListener(0, this);
    wxsManager.addDrawable(nbutton);

    UIButton* nbutton2 = new UIButton();
    nbutton2->text = "Cancel";
    nbutton2->position = XY{ wxWidth - 130, wxHeight - 40 };
    nbutton2->wxWidth = 120;
    nbutton2->setCallbackListener(1, this);
    wxsManager.addDrawable(nbutton2);

    tboxX = new UITextField();
    tboxX->position = XY{ 20, 80 };
    tboxX->text = std::to_string(defaultValues.x);
    tboxX->numeric = true;
    tboxX->wxWidth = 120;
    wxsManager.addDrawable(tboxX);

    tboxY = new UITextField();
    tboxY->position = XY{ 160, 80 };
    tboxY->text = std::to_string(defaultValues.y);
    tboxY->numeric = true;
    tboxY->wxWidth = 120;
    wxsManager.addDrawable(tboxY);
}

void PopupTileGeneric::render()
{
	renderDefaultBackground();

	XY titlePos = getDefaultTitlePosition();
	XY contentPos = getDefaultContentPosition();

	g_fnt->RenderString(title, titlePos.x, titlePos.y);
	g_fnt->RenderString(text, contentPos.x, contentPos.y);
	//XY opacityTextPos = xyAdd(getPopupOrigin(), XY{ 20, 140 });
	renderDrawables();
}

void PopupTileGeneric::eventButtonPressed(int evt_id) {
    if (evt_id == 0) {
        if (!tboxX->text.empty() && !tboxY->text.empty()) {
            result.x = std::stoi(tboxX->text);
            result.y = std::stoi(tboxY->text);
            callback->eventPopupClosed(popupEvtID, this);
            closePopup();
        }
    }
    else {
        closePopup();
    }

}
