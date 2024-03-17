#pragma once
#include "BaseBrush.h"

class Brush1x1 : public BaseBrush
{
	std::string getName() { return "1x1 Pixel"; }
	void clickPress(MainEditor* editor, XY pos) override;
	void clickDrag(MainEditor* editor, XY from, XY to) override;
	void clickRelease(MainEditor* editor, XY pos) override {}
};

