#include "PopupTextBox.h"
#include "FontRenderer.h"
#include "UIButton.h"

PopupTextBox::PopupTextBox(std::string tt, std::string tx, std::string defaultValue, int textFieldWidth)
{
	wxHeight = 240;

    actionButton(TL("vsp.cmn.confirm"))->onClickCallback = [this](...) {accept(); };

    actionButton(TL("vsp.cmn.cancel"))->onClickCallback = [this](...) {close(); };

    XY bodyEndpoint = makeTitleAndDesc(tt, tx);

    tbox = new UITextField();
    tbox->position = XY{ 20, ixmax(80, bodyEndpoint.y+10) };
    tbox->wxWidth = textFieldWidth;
    tbox->onTextChangedConfirmCallback = [this](...) {accept(); };
    tbox->setText(defaultValue);
    wxsManager.addDrawable(tbox);

    wxsManager.forceFocusOn(tbox);

}