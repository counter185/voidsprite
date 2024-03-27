#pragma once
#include "drawable.h"
#include "DrawableManager.h"
#include "BaseBrush.h"
#include "UIButton.h"
#include "EventCallbackListener.h"

class EditorBrushPicker : public Drawable, public EventCallbackListener
{
public:
	int wxWidth = 200;
	int wxHeight = 100;
	MainEditor* caller;

	DrawableManager subWidgets;
	std::vector<UIButton*> brushButtons;

	EditorBrushPicker(MainEditor* caller) {
		this->caller = caller;

		int px = 5;
		int py = 40;
		int i = 0;
		for (BaseBrush*& brush : g_brushes) {
			UIButton* newBtn = new UIButton();
			if (px + 26 > wxWidth) {
				py += 30;
				px = 5;
			}
			newBtn->position = XY{ px, py };
			px += 30;
			newBtn->icon = brush->cachedIcon;
			//newBtn->text = brush->getName();
			newBtn->wxWidth = 26;
			newBtn->wxHeight = 26;
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

