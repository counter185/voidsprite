#pragma once
#include "BaseBrush.h"
class BrushDiamond :
    public BaseBrush
{
	XY startPos = XY{ 0,0 };
	bool heldDown = false;

	void resetState() {
		startPos = XY{ 0,0 };
	}
	std::string getName() override { return "1px Diamond"; };
	std::string getIconPath() override { return VOIDSPRITE_ASSETS_PATH "assets/brush_1pxdiamond.png"; }
	XY getSection() override { return XY{ 1,2 }; }

	void clickPress(MainEditor* editor, XY pos) override;
	void clickRelease(MainEditor* editor, XY pos) override;
	void renderOnCanvas(XY canvasDrawPoint, int scale) override;
};

