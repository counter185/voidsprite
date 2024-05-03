#pragma once
#include "UIButton.h"
#include "DrawableManager.h"
#include "EventCallbackListener.h"

class UILayerButton : public Drawable, public EventCallbackListener
{
public:
	DrawableManager subButtons;
	
	UILayerButton(std::string mainName) {
		UIButton* mainButton = new UIButton();
		mainButton->text = mainName;
		mainButton->position = XY{ 0,0 };
		mainButton->wxWidth = 150;
		mainButton->setCallbackListener(0, this);
		subButtons.addDrawable(mainButton);
	}
	~UILayerButton() {
		subButtons.freeAllDrawables();
	}

	bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override;
	void render(XY position) override;
	void handleInput(SDL_Event evt, XY gPosOffset = { 0,0 }) override;


};

