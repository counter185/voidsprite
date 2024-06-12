#include "UIColorInputField.h"
#include "UITextField.h"

UIColorInputField::UIColorInputField()
{
	textField = new UITextField();
	textField->wxWidth = 100;
	textField->wxHeight = 30;
	textField->position = XY{ 30, 0 };
	textField->bgColor = SDL_Color{ 0,0,0,0x30 };
	textField->color = true;
	textField->setCallbackListener(EVENT_COLORPICKER_TEXTFIELD, this);
	textField->text = "000000";	//lmao fix this
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
	if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == 1 && evt.button.state) {
		wxsManager.tryFocusOnPoint(xySubtract(XY{ evt.button.x, evt.button.y }, gPosOffset));
	}
	if (wxsManager.anyFocused()) {
		wxsManager.passInputToFocused(evt);
	}
	else {

	}
}
