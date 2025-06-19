#pragma once
#include "BaseBrush.h"
class BrushRectFill :
    public BaseBrush
{
	XY startPos = XY{ 0,0 };
	bool heldDown = false;
	XY lastMousePos = XY{ 0,0 };

	void resetState() {
		startPos = XY{ 0,0 };
	}
	std::string getName() override { return TL("vsp.brush.rectfill"); };
	std::string getIconPath() override { return "brush_rectfill.png"; }
	XY getSection() override { return XY{ 0,2 }; }

	void clickPress(MainEditor* editor, XY pos) override;
	void clickDrag(MainEditor* editor, XY from, XY to) override { lastMousePos = to; }
	void clickRelease(MainEditor* editor, XY pos) override;
	void renderOnCanvas(XY canvasDrawPoint, int scale) override;
};

