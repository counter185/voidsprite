#include "PopupPickColor.h"
#include "UIColorInputField.h"
#include "UIButton.h"
#include "FontRenderer.h"

PopupPickColor::PopupPickColor(std::string tt, std::string tx) {
    this->title = tt;
    this->text = tx;
    wxHeight = 200;

    colorInput = new UIColorInputField();
    colorInput->position = XY{ 30, wxHeight / 2 };
    colorInput->wxWidth = 200;
    colorInput->wxHeight = 30;
    wxsManager.addDrawable(colorInput);

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
}

void PopupPickColor::render()
{
    renderDefaultBackground();

    XY titlePos = getDefaultTitlePosition();
    XY contentPos = getDefaultContentPosition();

    g_fnt->RenderString(title, titlePos.x, titlePos.y);
    g_fnt->RenderString(text, contentPos.x, contentPos.y);

    renderDrawables();
}

void PopupPickColor::eventButtonPressed(int evt_id)
{
    if (evt_id == 0) {
        if (callback != NULL) {
            callback->eventColorSet(callback_id, colorInput->pickedColor);
        }

    }
    g_closePopup(this);
}
