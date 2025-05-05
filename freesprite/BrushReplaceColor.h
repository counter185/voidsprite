#pragma once
#include "BaseBrush.h"
class BrushReplaceColor : public BaseBrush
{
public:
	std::string getName() override { return TL("vsp.brush.swapcolor"); }
	std::string getTooltip() override { return TL("vsp.brush.swapcolor.desc"); }
	std::string getIconPath() override { return "brush_replace.png"; }
	XY getSection() override { return XY{ 0,1 }; }

	void clickPress(MainEditor* editor, XY pos) override;
	//void clickDrag(MainEditor* editor, XY from, XY to) override;
	//void clickRelease(MainEditor* editor, XY pos) override {}
	void renderOnCanvas(MainEditor* editor, int scale) override;
};

