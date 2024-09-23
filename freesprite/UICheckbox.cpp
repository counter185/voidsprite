#include "UICheckbox.h"
#include "UILabel.h"

UICheckbox::UICheckbox(std::string text, bool defaultState)
{
	checkbox = new UICheckboxButton(defaultState);
	checkbox->position = { 0,0 };
	checkbox->setCallbackListener(0, this);
	subWidgets.addDrawable(checkbox);

	label = new UILabel();
	label->text = text;
	label->position = { 35,0 };
	subWidgets.addDrawable(label);
}

void UICheckboxButton::render(XY pos) {
	UIButton::render(pos);

	SDL_Rect r = { pos.x + position.x + 5, pos.y + position.y + 5, wxWidth - 10, wxHeight - 10 };
	SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 255);

	XY lineP1 = { r.x, r.y + r.h / 2 };
	XY lineP2 = { r.x + r.w / 3, r.y + r.h / 5 * 4 };
	XY lineP3 = { r.x + r.w, lineP2.y - ((r.x+r.w) - lineP2.x)};

	double timer = XM1PW3P1(stateChangeTimer.percentElapsedTime(400));
	if (state) {
		drawLine(lineP2, lineP1, timer);
		drawLine(lineP2, lineP3, timer);
	}
	else {
		drawLine(lineP1, lineP2, 1.0-timer);
		drawLine(lineP3, lineP2, 1.0-timer);
	}
}

void UICheckboxButton::click()
{
	state = !state;
	stateChangeTimer.start();
	UIButton::click();
}
