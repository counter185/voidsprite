#pragma once
#include "UIButton.h"
#include "DrawableManager.h"
#include "EventCallbackListener.h"

class UILayerButton : public Drawable, public EventCallbackListener
{
public:
	DrawableManager subButtons;
	UIButton* mainButton;
	UIButton* hideButton;
	
	UILayerButton(std::string mainName) {
		mainButton = new UIButton();
		mainButton->text = mainName;
		mainButton->position = XY{ 0,0 };
		mainButton->wxWidth = 200;
		mainButton->setCallbackListener(0, this);
		subButtons.addDrawable(mainButton);

		hideButton = new UIButton();
		hideButton->text = "H";
		hideButton->position = XY{ mainButton->wxWidth + 10,0 };
		hideButton->wxWidth = 30;
		hideButton->setCallbackListener(1, this);
		subButtons.addDrawable(hideButton);
	}
	~UILayerButton() {
		subButtons.freeAllDrawables();
	}

	bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override;
	void render(XY position) override;
	void handleInput(SDL_Event evt, XY gPosOffset = { 0,0 }) override;

	void eventButtonPressed(int evt_id) override;
};

