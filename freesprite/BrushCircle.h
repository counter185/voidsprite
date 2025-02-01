#pragma once
#include "BaseBrush.h"
class BrushCircle :
    public BaseBrush
{
public:
	XY startPos = XY{ 0,0 };
	bool heldDown = false;

	void resetState() {
		startPos = XY{ 0,0 };
	}
	std::string getName() override { return "1px Circle"; };
	std::string getIconPath() override { return VOIDSPRITE_ASSETS_PATH "assets/brush_1pxcircle.png"; }
	XY getSection() override { return XY{ 1,2 }; }

	void clickPress(MainEditor* editor, XY pos) override;
	void clickRelease(MainEditor* editor, XY pos) override;
	void renderOnCanvas(XY canvasDrawPoint, int scale) override;
};

class BrushCircleArc : public BrushCircle {
protected:
	bool rightClicked = false;
public:

	void resetState() {
		startPos = XY{ 0,0 };
	}
	std::string getName() override { return "1px Circle (arc)"; };
	std::string getIconPath() override { return VOIDSPRITE_ASSETS_PATH "assets/brush_1pxarccircle.png"; }
	XY getSection() override { return XY{ 1,2 }; }

	void clickPress(MainEditor* editor, XY pos) override;
	void clickRelease(MainEditor* editor, XY pos) override;
	void renderOnCanvas(XY canvasDrawPoint, int scale) override;
	virtual void rightClickPress(MainEditor* editor, XY pos) override;
	virtual void rightClickRelease(MainEditor* editor, XY pos) override;
	virtual bool overrideRightClick() { return true; }
};