#include "PopupTextTool.h"
#include "globals.h"
#include "FontRenderer.h"
#include "ToolText.h"
#include "UITextField.h"
#include "UIButton.h"
#include "UILabel.h"

PopupTextTool::PopupTextTool(ToolText* parent, std::string tt, std::string tx)
{
	caller = parent;
	title = tt;
	text = tx;
	textSize = parent->textSize;

	textbox = new UITextField();
	textbox->position = XY{ 20, 80 };
	textbox->text = parent->text;
	textbox->wxWidth = 260;
	wxsManager.addDrawable(textbox);

	UILabel* label = new UILabel();
	label->text = "Text Size";
	label->position = XY{ 20, 120 };
	wxsManager.addDrawable(label);

	textboxSize = new UITextField();
	textboxSize->position = XY{ 120, 120 };
	textboxSize->text = std::to_string(parent->textSize);
	textboxSize->isNumericField = true;
	textboxSize->wxWidth = 120;
	wxsManager.addDrawable(textboxSize);

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

void PopupTextTool::render()
{
	renderDefaultBackground();

	XY titlePos = getDefaultTitlePosition();
	XY contentPos = getDefaultContentPosition();

	g_fnt->RenderString(title, titlePos.x, titlePos.y);
	g_fnt->RenderString(text, contentPos.x, contentPos.y);
	//XY opacityTextPos = xyAdd(getPopupOrigin(), XY{ 20, 140 });
	//g_fnt->RenderString("Opacity", opacityTextPos.x, opacityTextPos.y);
	renderDrawables();
}

void PopupTextTool::eventButtonPressed(int evt_id) {
	if (evt_id == 0) {
		textSize = std::stoi(textboxSize->text);
		caller->eventPopupClosed(EVENT_TOOLTEXT_POSTCONFIG, this);
		closePopup();
	}
	else {
		closePopup();
	}

}
