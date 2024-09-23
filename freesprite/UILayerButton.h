#pragma once
#include "UIButton.h"
#include "DrawableManager.h"
#include "EventCallbackListener.h"
#include "Panel.h"

class UILayerButton : public Panel, public EventCallbackListener
{
public:
	UIButton* mainButton;
	UIButton* hideButton;
	
	UILayerButton(std::string mainName) {
		mainButton = new UIButton();
		mainButton->text = mainName;
		mainButton->position = XY{ 0,0 };
		mainButton->wxWidth = 200;
		mainButton->setCallbackListener(0, this);
		subWidgets.addDrawable(mainButton);

		hideButton = new UIButton();
		hideButton->text = "H";
		hideButton->position = XY{ mainButton->wxWidth + 10,0 };
		hideButton->wxWidth = 30;
		hideButton->setCallbackListener(1, this);
		subWidgets.addDrawable(hideButton);
	}

	void eventButtonPressed(int evt_id) override;
};

