#pragma once
#include "globals.h"
#include "UIColorSlider.h"
class UIHueSlider : public UIColorSlider
{
public:
	UIColorPicker* parent;

	UIHueSlider(UIColorPicker* parent) {
		wxWidth = 360;
		wxHeight = 20;
		this->parent = parent;
		this->colors = {
			0xFFFF0000,
			0xFFFFFF00,
			0xFF00FF00,
			0xFF00FFFF,
			0xFF0000FF,
			0xFFFF00FF,
			0xFFFF0000
		};
	}
	//void render(XY pos) override;
	void onSliderPosChanged() override;
};

