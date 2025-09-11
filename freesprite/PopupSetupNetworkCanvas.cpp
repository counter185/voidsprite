#include "PopupSetupNetworkCanvas.h"
#include "UITextField.h"
#include "UIButton.h"
#include "PopupPickColor.h"
#include "Notification.h"

PopupSetupNetworkCanvas::PopupSetupNetworkCanvas(std::string tt, std::string tx, bool ipField, bool portField)
{
    wxHeight = 290;

    userColor = sdlcolorToUint32(rgb2sdlcolor(hsv2rgb(
        hsv{
            randomInt(0, 360)*1.0,
            randomInt(80,100)/100.0,
            randomInt(90,100)/100.0
        }
    )));

    actionButton(TL("vsp.cmn.confirm"))->onClickCallback = [this, ipField, portField](UIButton*) {
        if (onInputConfirmCallback != NULL) {
            if (!ipField || !textboxIP->textEmpty()) {
                PopupSetNetworkCanvasData ret;
                ret.ip = ipField ? textboxIP->getText() : "";
                ret.port = (!portField || textboxPort->textEmpty()) ? 6600 : std::stoi(textboxPort->getText());
                ret.username = textboxUsername->textEmpty() ? "User" : textboxUsername->getText();
                ret.userColor = userColor;
                ret.password = textboxPassword->getText();
                onInputConfirmCallback(this, ret);
                closePopup();
            }
            else {
                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.collabeditor.error.emptyip")));
            }
        }
    };

    actionButton(TL("vsp.cmn.cancel"))->onClickCallback = [this](UIButton*) {
        closePopup();
    };

    if (ipField) {
        textboxIP = new UITextField();
        textboxIP->position = XY{ 20, 80 };
        textboxIP->wxWidth = 180;
        textboxIP->placeholderText = TL("vsp.collabeditor.popup.ip.placeholder");
        wxsManager.addDrawable(textboxIP);
    }

    if (portField) {
        textboxPort = new UITextField();
        textboxPort->position = XY{ 210, 80 };
        textboxPort->wxWidth = 60;
        textboxPort->placeholderText = "6600";
        textboxPort->isNumericField = true;
        wxsManager.addDrawable(textboxPort);
    }

    UILabel* labelUsername = new UILabel(TL("vsp.collabeditor.popup.username"));
    labelUsername->position = XY{ 30, 120 };
    wxsManager.addDrawable(labelUsername);

    textboxUsername = new UITextField();
    textboxUsername->position = XY{ ixmax(labelUsername->calcEndpoint().x + 30, 90), 120 };
    textboxUsername->wxWidth = 190;
    textboxUsername->setText("User");
    wxsManager.addDrawable(textboxUsername);

    UILabel* labelUsercolor = new UILabel(TL("vsp.collabeditor.popup.usercolor"));
    labelUsercolor->position = XY{ 30, 150 };
    wxsManager.addDrawable(labelUsercolor);

    buttonSetUserColor = new UIButton();
    buttonSetUserColor->position = XY{ ixmax(labelUsercolor->calcEndpoint().x + 30, 90), 150 };
    buttonSetUserColor->wxWidth = 100;
    buttonSetUserColor->onClickCallback = [this](UIButton*) {
        PopupPickColor* colorPicker = new PopupPickColor(TL("vsp.collabeditor.popup.usercolor"), "", false);
        colorPicker->setRGB(userColor);
        colorPicker->onColorConfirmedCallback = [this](PopupPickColor* ppc, u32 color) {
            userColor = color;
            updateUserColorButton();
        };
        g_addPopup(colorPicker);
    };
    wxsManager.addDrawable(buttonSetUserColor);


    UILabel* labelPassword = new UILabel(TL("vsp.collabeditor.popup.password"));
    labelPassword->position = XY{ 30, 180 };
    wxsManager.addDrawable(labelPassword);

    textboxPassword = new UITextField();
    textboxPassword->position = XY{ ixmax(labelPassword->calcEndpoint().x + 30, 90), 180 };
    textboxPassword->wxWidth = 190;
    textboxPassword->setText("");
    textboxPassword->placeholderText = TL("vsp.collabeditor.popup.password.none");
    wxsManager.addDrawable(textboxPassword);

    updateUserColorButton();


    wxsManager.forceFocusOn(textboxIP);

    makeTitleAndDesc(tt, tx);
}

void PopupSetupNetworkCanvas::updateUserColorButton()
{
    buttonSetUserColor->fill = Fill::Solid(0xFF000000 | userColor);
}
