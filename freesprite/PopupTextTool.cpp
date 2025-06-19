#include "PopupTextTool.h"
#include "globals.h"
#include "FontRenderer.h"
#include "brush/ToolText.h"
#include "UITextField.h"
#include "UIButton.h"
#include "UILabel.h"

PopupTextTool::PopupTextTool(ToolText* parent, std::string tt, std::string tx)
{
	caller = parent;
	textSize = parent->textSize;

	textbox = new UITextField();
	textbox->position = XY{ 20, 80 };
	textbox->setText(parent->text);
	textbox->wxWidth = 260;
	wxsManager.addDrawable(textbox);

	UILabel* label = new UILabel("Text Size");
	label->position = XY{ 20, 120 };
	wxsManager.addDrawable(label);

	textboxSize = new UITextField();
	textboxSize->position = XY{ 120, 120 };
	textboxSize->setText(std::to_string(parent->textSize));
	textboxSize->isNumericField = true;
	textboxSize->wxWidth = 120;
	wxsManager.addDrawable(textboxSize);

	UIButton* nbutton = actionButton(TL("vsp.cmn.apply"));
	nbutton->setCallbackListener(0, this);

	UIButton* nbutton2 = actionButton(TL("vsp.cmn.cancel"));
	nbutton2->setCallbackListener(1, this);

	makeTitleAndDesc(tt, tx);
}

void PopupTextTool::eventButtonPressed(int evt_id) {
	if (evt_id == 0) {
		textSize = std::stoi(textboxSize->getText());
		caller->eventPopupClosed(EVENT_TOOLTEXT_POSTCONFIG, this);
		closePopup();
	}
	else {
		closePopup();
	}

}
