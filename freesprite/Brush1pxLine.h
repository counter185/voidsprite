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
	std::string getName() override { return TL("vsp.brush.line"); };
	std::string getTooltip() override { return TL("vsp.brush.line.desc"); }
	std::string getIconPath() override { return "brush_1pxline.png"; }
	XY getSection() override { return XY{ 0,2 }; }

	void clickPress(MainEditor* editor, XY pos) override;
	void clickDrag(MainEditor* editor, XY from, XY to) override {}
	void clickRelease(MainEditor* editor, XY pos) override;
	void renderOnCanvas(XY canvasDrawPoint, int scale);
};

