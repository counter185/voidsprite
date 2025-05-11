#pragma once
#include "DrawableManager.h"
#include "Panel.h"
#include "UIButton.h"

class LayerActiveButton : public UIButton {
private:
	Layer* ll = NULL;
public:
	LayerActiveButton(Layer* l) : UIButton(), ll(l) {}

	void render(XY at) override;
};

class UILayerButton : public Panel
{
private:
	Layer* layer;
public:
	UIButton* mainButton;
	UIButton* hideButton;

	std::function<void(UILayerButton*)> onMainButtonClickedCallback = NULL;
	std::function<void(UILayerButton*)> onHideButtonClickedCallback = NULL;

	UILayerButton(std::string mainName, Layer* linkedLayer);

	bool takesMouseWheelEvents() override { return false; }
};

