#pragma once
#include "globals.h"
#include "UISlider.h"
class UIHueSlider : public UISlider
{
public:
	EditorColorPicker* parent;

	UIHueSlider(EditorColorPicker* parent) {
		wxWidth = 360;
		wxHeight = 40;
		this->parent = parent;
	}
	void render(XY pos) override;
	void onSliderPosChanged() override;
	//void drawPosIndicator()
};

