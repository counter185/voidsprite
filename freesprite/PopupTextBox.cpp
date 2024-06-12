#include "PopupTextBox.h"
#include "FontRenderer.h"
#include "UIButton.h"

PopupTextBox::PopupTextBox(std::string tt, std::string tx, int textFieldWidth)
{
	wxHeight = 240;

	this->title = tt;
	this->text = tx;

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
    wxsManager.addDrawable(tbox);
}

void PopupTextBox::render()
{
	renderDefaultBackground();

	XY titlePos = getDefaultTitlePosition();
	XY contentPos = getDefaultContentPosition();

	g_fnt->RenderString(title, titlePos.x, titlePos.y);
	g_fnt->RenderString(text, contentPos.x, contentPos.y);
	//XY opacityTextPos = xyAdd(getPopupOrigin(), XY{ 20, 140 });
	renderDrawables();
}
