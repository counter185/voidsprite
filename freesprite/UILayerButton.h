#pragma once
#include "UIButton.h"
#include "DrawableManager.h"
#include "EventCallbackListener.h"
#include "Panel.h"

class LayerActiveButton : public UIButton {
private:
	Layer* ll = NULL;
public:
	LayerActiveButton(Layer* l) : UIButton(), ll(l) {}

	void render(XY at) override;
};

class LayerVariantButton : public UIButton {
private:
	Layer* ll = NULL;
	int variantIndex = -1;
public:
	LayerVariantButton(Layer* l, int variantIndex);
};

class UILayerButton : public Panel, public EventCallbackListener
{
private:
	Layer* layer;
public:
	UIButton* mainButton;
	UIButton* hideButton;
	std::vector<LayerVariantButton*> variantButtons;

	UILayerButton(std::string mainName, Layer* linkedLayer);

	void eventButtonPressed(int evt_id) override;
	bool takesMouseWheelEvents() override { return false; }
};

