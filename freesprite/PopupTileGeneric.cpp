#include "PopupTileGeneric.h"
#include "FontRenderer.h"
#include "Notification.h"

PopupTileGeneric::PopupTileGeneric(EventCallbackListener* callback, std::string tt, std::string tx, XY defaultValues, int event_id)
{
    wxHeight = 240;

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
    tboxX->setText(std::to_string(defaultValues.x));
    tboxX->isNumericField = true;
    tboxX->wxWidth = 120;
    wxsManager.addDrawable(tboxX);

    tboxY = new UITextField();
    tboxY->position = XY{ 160, 80 };
    tboxY->setText(std::to_string(defaultValues.y));
    tboxY->isNumericField = true;
    tboxY->wxWidth = 120;
    tboxY->setCallbackListener(2, this);
    wxsManager.addDrawable(tboxY);

    makeTitleAndDesc(tt, tx);
}

void PopupTileGeneric::eventTextInputConfirm(int evt_id, std::string data)
{
    if (evt_id == 2) {
        eventButtonPressed(0);
    }
}

void PopupTileGeneric::eventButtonPressed(int evt_id) {
    if (evt_id == 0) {
        if (!tboxX->textEmpty() && !tboxY->textEmpty()) {
            result.x = std::stoi(tboxX->getText());
            result.y = std::stoi(tboxY->getText());
            callback->eventPopupClosed(popupEvtID, this);
            closePopup();
        }
        else {
            g_addNotification(ErrorNotification("Error", "Both dimensions must be set"));
        }
    }
    else {
        closePopup();
    }

}
