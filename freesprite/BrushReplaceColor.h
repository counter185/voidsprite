#pragma once
#include "BaseBrush.h"
class BrushReplaceColor : public BaseBrush
{
public:
	std::string getName() override { return "Replace color"; }
	std::string getTooltip() override { return "Mouse Left to replace all occurrences of the clicked color with the current active color."; }
	std::string getIconPath() override { return VOIDSPRITE_ASSETS_PATH "assets/brush_replace.png"; }
	XY getSection() override { return XY{ 0,1 }; }

	void clickPress(MainEditor* editor, XY pos) override;
	//void clickDrag(MainEditor* editor, XY from, XY to) override;
	//void clickRelease(MainEditor* editor, XY pos) override {}
	void renderOnCanvas(MainEditor* editor, int scale);
};

