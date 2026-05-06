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
    nbutton->onClickCallback = [this](...) {
        if (xValid(result.x) && yValid(result.y)) {
            this->callback->eventPopupClosed(popupEvtID, this);
            closePopup();
        }
        else {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.error.invalidinput")));
        }
    };

    UIButton* nbutton2 = actionButton(TL("vsp.cmn.cancel"));
    nbutton2->onClickCallback = [this](...) { closePopup(); };

    UINumberInputField* tboxX = new UINumberInputField(&result.x);
    tboxX->position = XY{ 20, 80 };
    tboxX->wxWidth = 120;
    tboxX->validateFunction = [this](int v) { return xValid(v); };
    tboxX->valueUpdatedCallback = [this](...) { resultUpdated(result); };
    tboxX->onTextChangedConfirmCallback = [this, nbutton](UITextField*, std::string data) {
        nbutton->click();
     };
    
    wxsManager.addDrawable(tboxX);

    UINumberInputField* tboxY = new UINumberInputField(&result.y);
    tboxY->position = XY{ 160, 80 };
    tboxY->wxWidth = 120;
    tboxY->validateFunction = [this](int v) { return yValid(v); };
    tboxY->valueUpdatedCallback = [this](...) { resultUpdated(result); };
    tboxY->onTextChangedConfirmCallback = [this, nbutton](UITextField*, std::string data) {
        nbutton->click();
    };
    wxsManager.addDrawable(tboxY);

    makeTitleAndDesc(tt, tx);

    wxsManager.forceFocusOn(tboxX);
}