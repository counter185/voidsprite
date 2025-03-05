#include "UIColorInputField.h"
#include "UITextField.h"

UIColorInputField::UIColorInputField()
{
	textField = new UITextField();
	textField->wxWidth = 100;
	textField->wxHeight = 30;
	textField->position = XY{ 30, 0 };
	textField->bgColor = SDL_Color{ 0,0,0,0x30 };
	textField->isColorField = true;
	textField->setCallbackListener(EVENT_COLORPICKER_TEXTFIELD, this);
	textField->setText("000000");	//todo: lmao fix this
	wxsManager.addDrawable(textField);

}

void UIColorInputField::render(XY pos)
{
	SDL_SetRenderDrawColor(g_rd, (pickedColor >> 16) & 0xff, (pickedColor >> 8) & 0xff, pickedColor & 0xff, 0xff);
	SDL_Rect colorPreviewRect = { pos.x, pos.y, 30, 30 };
	SDL_RenderFillRect(g_rd, &colorPreviewRect);
	wxsManager.renderAll(pos);
}

void UIColorInputField::handleInput(SDL_Event evt, XY gPosOffset)
{
	if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == 1 && evt.button.down) {
		wxsManager.tryFocusOnPoint(xySubtract(XY{ (int)evt.button.x, (int)evt.button.y }, gPosOffset));
	}
	if (wxsManager.anyFocused()) {
		wxsManager.passInputToFocused(evt);
	}
	else {

	}
}
