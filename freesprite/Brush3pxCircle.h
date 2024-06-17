#pragma once
#include "BaseBrush.h"

class Brush3pxCircle : public BaseBrush
{
	std::string getName() override { return "3px Circle"; }
	std::string getIconPath() override { return "assets/brush_3pxcircle.png"; }
	void clickPress(MainEditor* editor, XY pos) override;
	void clickDrag(MainEditor* editor, XY from, XY to) override;
	void clickRelease(MainEditor* editor, XY pos) override {}
	void renderOnCanvas(XY canvasDrawPoint, int scale);
};

