#pragma once
#include "BaseBrush.h"
class Brush1pxLine :
    public BaseBrush
{

	XY startPos = XY{ 0,0 };
	bool dragging = false;

	void resetState() {
		startPos = XY{ 0,0 };
	}
	std::string getName() override { return "1px Line"; };
	std::string getTooltip() override { return "Hold Mouse Left at the beginning of the line, then release at the end point"; }
	std::string getIconPath() override { return VOIDSPRITE_ASSETS_PATH "assets/brush_1pxline.png"; }
	void clickPress(MainEditor* editor, XY pos) override;
	void clickDrag(MainEditor* editor, XY from, XY to) override {}
	void clickRelease(MainEditor* editor, XY pos) override;
	void renderOnCanvas(XY canvasDrawPoint, int scale);
};

