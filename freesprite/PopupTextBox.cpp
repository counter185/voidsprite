#include "PopupTextBox.h"
#include "FontRenderer.h"
#include "UIButton.h"

PopupTextBox::PopupTextBox(std::string tt, std::string tx, std::string defaultValue, int textFieldWidth)
{
	wxHeight = 240;

    UIButton* nbutton = new UIButton();
    nbutton->text = "Confirm";
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

    tbox = new UITextField();
    tbox->position = XY{ 20, 80 };
    tbox->wxWidth = textFieldWidth;
    tbox->setCallbackListener(0, this);
    tbox->setText(defaultValue);
    wxsManager.addDrawable(tbox);

    wxsManager.forceFocusOn(tbox);

    makeTitleAndDesc(tt, tx);
}