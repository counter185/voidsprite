#pragma once
#include "BaseBrush.h"
class Brush1pxLine :
    public BaseBrush
{

	XY startPos = XY{ 0,0 };

	void resetState() {
		startPos = XY{ 0,0 };
	}
	std::string getName() { return "1px Line"; }
	void clickPress(MainEditor* editor, XY pos) override;
	void clickDrag(MainEditor* editor, XY from, XY to) override {}
	void clickRelease(MainEditor* editor, XY pos) override;
};

