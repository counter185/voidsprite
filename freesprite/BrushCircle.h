#pragma once
#include "BaseBrush.h"
class BrushCircle :
    public BaseBrush
{
public:
	XY startPos = XY{ 0,0 };
	bool heldDown = false;

	void resetState() override {
		startPos = XY{ 0,0 };
	}
	std::string getName() override { return TL("vsp.brush.circle"); };
	std::string getIconPath() override { return "brush_1pxcircle.png"; }
	XY getSection() override { return XY{ 1,2 }; }

	void clickPress(MainEditor* editor, XY pos) override;
	void clickRelease(MainEditor* editor, XY pos) override;
	void renderOnCanvas(XY canvasDrawPoint, int scale) override;
};

class BrushCircleArc : public BrushCircle {
protected:
	bool rightClicked = false;
public:

	void resetState() override {
		startPos = XY{ 0,0 };
	}
	std::string getName() override { return TL("vsp.brush.circlearc"); };
	std::string getIconPath() override { return "brush_1pxarccircle.png"; }
	XY getSection() override { return XY{ 1,2 }; }

	void clickPress(MainEditor* editor, XY pos) override;
	void clickRelease(MainEditor* editor, XY pos) override;
	void renderOnCanvas(XY canvasDrawPoint, int scale) override;
	void rightClickPress(MainEditor* editor, XY pos) override;
	void rightClickRelease(MainEditor* editor, XY pos) override;
	bool overrideRightClick() override { return true; }
};