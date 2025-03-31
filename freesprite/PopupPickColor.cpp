#include "PopupPickColor.h"
#include "UIColorInputField.h"
#include "UIButton.h"
#include "FontRenderer.h"

PopupPickColor::PopupPickColor(std::string tt, std::string tx, bool acceptAlpha) {
    this->acceptAlpha = acceptAlpha;
    wxHeight = 200;

    colorInput = new UIColorInputField();
    colorInput->position = XY{ 30, wxHeight / 2 };
    wxsManager.addDrawable(colorInput);

    if (acceptAlpha) {
        alphaInput = new UITextField();
        alphaInput->position = XY{ 160, wxHeight / 2 };
        alphaInput->wxWidth = 50;
        alphaInput->wxHeight = 30;
        alphaInput->setCallbackListener(2, this);
        wxsManager.addDrawable(alphaInput);
        setAlpha(255);
    }

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

    makeTitleAndDesc(tt, tx);
}

void PopupPickColor::eventButtonPressed(int evt_id)
{
    if (evt_id == 0) {
        if (callback != NULL) {
            callback->eventColorSet(callback_id, getColor());
        }

    }
    g_closePopup(this);
}

void PopupPickColor::updateRGBTextBoxOnInputEvent(std::string data, uint8_t* value)
{
    try {
        int val;
        if (data.size() == 3 && data[0] == 'x') {
            val = std::stoi(data.substr(1), 0, 16);
        }
        else {
            val = std::stoi(data);
        }
        if (val >= 0 && val <= 255) {
            *value = val;
            alphaInput->setText(std::to_string(alpha));
        }
    }
    catch (std::exception) {

    }
}

void PopupPickColor::setAlpha(uint8_t a)
{
    if (acceptAlpha) {
        alpha = a;
        alphaInput->setText(std::to_string(alpha));
    }
}

uint32_t PopupPickColor::getColor()
{
    return (colorInput->pickedColor & 0xFFFFFF) | (acceptAlpha ? (alpha << 24) : 0xFF000000);
}

void PopupPickColor::eventTextInput(int evt_id, std::string data)
{
    if (evt_id == 2) {
        updateRGBTextBoxOnInputEvent(data, &alpha);
    }
}
