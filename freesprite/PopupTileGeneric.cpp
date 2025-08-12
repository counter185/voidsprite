#include "PopupTileGeneric.h"
#include "FontRenderer.h"
#include "Notification.h"

PopupTileGeneric::PopupTileGeneric(EventCallbackListener* callback, std::string tt, std::string tx, XY defaultValues, int event_id)
{
    wxHeight = 240;

    this->popupEvtID = event_id;
    this->callback = callback;
    this->result = defaultValues;

    UIButton* nbutton = actionButton(TL("vsp.cmn.apply"));
    nbutton->setCallbackListener(0, this);

    UIButton* nbutton2 = actionButton(TL("vsp.cmn.cancel"));
    nbutton2->setCallbackListener(1, this);

    tboxX = new UITextField();
    tboxX->position = XY{ 20, 80 };
    tboxX->isNumericField = true;
    tboxX->wxWidth = 120;
    tboxX->setText(std::to_string(defaultValues.x));
    tboxX->onTextChangedCallback = [this](UITextField* txf, std::string data) {
        if (!txf->textEmpty()) {
            result.x = std::stoi(data);
            resultUpdated(result);
        }
    };
    
    wxsManager.addDrawable(tboxX);

    tboxY = new UITextField();
    tboxY->position = XY{ 160, 80 };
    tboxY->isNumericField = true;
    tboxY->wxWidth = 120;
    tboxY->setText(std::to_string(defaultValues.y));
    tboxY->onTextChangedCallback = [this](UITextField* txf, std::string data) {
        if (!txf->textEmpty()) {
            result.y = std::stoi(data);
            resultUpdated(result);
        }
    };
    tboxY->onTextChangedConfirmCallback = [this](UITextField*, std::string data) {
        eventButtonPressed(0);
    };
    wxsManager.addDrawable(tboxY);

    makeTitleAndDesc(tt, tx);
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
