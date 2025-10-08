#include "UIColorInputField.h"
#include "UITextField.h"
#include "UIButton.h"
#include "PopupPickColor.h"

UIColorInputField::UIColorInputField(bool alpha)
{
	allowAlpha = alpha;
	button = new UIButton();
	button->position = XY{ 0,0 };
	button->wxWidth = 125;
	button->onClickCallback = [this](UIButton* b) {
		PopupPickColor* colorPicker = new PopupPickColor(TL("vsp.cmn.pickcolor"), "", allowAlpha);
		colorPicker->onColorConfirmedCallback = [this](PopupPickColor* p, u32 c) {
			setColor(c);
		};
		g_addPopup(colorPicker);
	};
	subWidgets.addDrawable(button);
	setColor(0xFFFFFFFF);
}

void UIColorInputField::setColor(u32 c)
{
	pickedColor = c;
	button->fill = Fill::Solid(pickedColor);
	if (onColorChangedCallback != NULL) {
		onColorChangedCallback(this, pickedColor);
	}
}
