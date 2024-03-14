#pragma once
#include "globals.h"
#include "UISlider.h"
class UIColorSlider : public UISlider
{
public:
	EditorColorPicker* parent;

	UIColorSlider(EditorColorPicker* parent) {
		wxWidth = 360;
		wxHeight = 40;
		this->parent = parent;
	}
	void render(XY pos) override;
	void onSliderPosChanged() override;
	//void drawPosIndicator()
};

