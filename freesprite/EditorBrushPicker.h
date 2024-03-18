#pragma once
#include "drawable.h"
#include "DrawableManager.h"
#include "BaseBrush.h"
#include "UIButton.h"
#include "EventCallbackListener.h"

class EditorBrushPicker : public Drawable, public EventCallbackListener
{
public:
	int wxWidth = 400;
	int wxHeight = 200;
	MainEditor* caller;

	DrawableManager subWidgets;
	std::vector<UIButton*> brushButtons;

	EditorBrushPicker(MainEditor* caller) {
		this->caller = caller;

		int py = 0;
		int i = 0;
		for (BaseBrush*& brush : g_brushes) {
			UIButton* newBtn = new UIButton();
			newBtn->position = XY{ 5, py += 40 };
			newBtn->text = brush->getName();
			newBtn->wxWidth = 100;
			newBtn->setCallbackListener(10 + i++, this);
			brushButtons.push_back(newBtn);
			subWidgets.addDrawable(newBtn);
		}
	}

	bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override;
	void render(XY position) override;
	void handleInput(SDL_Event evt, XY gPosOffset) override;
	void focusOut() override {
		Drawable::focusOut();
		subWidgets.forceUnfocus();
	}
	void eventButtonPressed(int evt_id) override;

	void updateActiveBrushButton(int id);
};

