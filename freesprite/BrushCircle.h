#pragma once
#include "BaseBrush.h"
class BrushCircle :
    public BaseBrush
{
public:
	XY startPos = XY{ 0,0 };
	bool heldDown = false;
	XY lastMousePos = XY{ 0,0 };

	void resetState() {
		startPos = XY{ 0,0 };
	}
	std::string getName() override { return "1px Circle"; };
	std::string getIconPath() override { return VOIDSPRITE_ASSETS_PATH "assets/brush_1pxcircle.png"; }
	void clickPress(MainEditor* editor, XY pos) override;
	void clickDrag(MainEditor* editor, XY from, XY to) override { lastMousePos = to; }
	void clickRelease(MainEditor* editor, XY pos) override;
	void renderOnCanvas(XY canvasDrawPoint, int scale) override;
};

