#pragma once
#include "UIButton.h"
#include "DrawableManager.h"
#include "EventCallbackListener.h"
#include "Panel.h"
#include "UIButton.h"

class LayerActiveButton : public UIButton {
private:
	Layer* ll = NULL;
public:
	LayerActiveButton(Layer* l) : UIButton(), ll(l) {}

	void render(XY at) override;
};

class UILayerButton : public Panel, public EventCallbackListener
{
private:
	Layer* layer;
public:
	UIButton* mainButton;
	UIButton* hideButton;

	UILayerButton(std::string mainName, Layer* linkedLayer) {
		wxWidth = 240;
		wxHeight = 30;

		layer = linkedLayer;

		mainButton = new LayerActiveButton(layer);
		mainButton->text = mainName;
		mainButton->position = XY{ 0,0 };
		mainButton->wxWidth = 200;
		mainButton->setCallbackListener(0, this);
		subWidgets.addDrawable(mainButton);

		hideButton = new UIButton();
		//hideButton->text = "H";
		hideButton->tooltip = "Hide";
		hideButton->icon = g_iconLayerHide;
		hideButton->position = XY{ mainButton->wxWidth + 10,0 };
		hideButton->wxWidth = 30;
		hideButton->setCallbackListener(1, this);
		subWidgets.addDrawable(hideButton);
	}

	void eventButtonPressed(int evt_id) override;
	bool takesMouseWheelEvents() override { return false; }
};

