#pragma once
#include "BaseBrush.h"
class Brush1pxLinePathfind : public BaseBrush
{
public:
	XY startPos = XY{ 0,0 };
	bool dragging = false;

	void resetState() {
		startPos = XY{ 0,0 };
	}
	std::string getName() override { return "1px Pathfind Line"; };
	std::string getIconPath() override { return "assets/brush_1pxlinepathfind.png"; }
	void clickPress(MainEditor* editor, XY pos) override;
	void clickDrag(MainEditor* editor, XY from, XY to) override {}
	void clickRelease(MainEditor* editor, XY pos) override;
	void renderOnCanvas(XY canvasDrawPoint, int scale);
};

