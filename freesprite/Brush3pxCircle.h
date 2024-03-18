#pragma once
#include "BaseBrush.h"

class Brush3pxCircle : public BaseBrush
{
	std::string getName() { return "3px Circle"; }
	void clickPress(MainEditor* editor, XY pos) override;
	void clickDrag(MainEditor* editor, XY from, XY to) override;
	void clickRelease(MainEditor* editor, XY pos) override {}
};

