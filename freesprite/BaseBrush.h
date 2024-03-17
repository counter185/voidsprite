#pragma once
#include "globals.h"

class BaseBrush
{
public:
	virtual void resetState() {}
	virtual std::string getName() { return "Base brush"; }
	virtual void clickPress(MainEditor* editor, XY pos) {}
	virtual void clickDrag(MainEditor* editor, XY from, XY to) {}
	virtual void clickRelease(MainEditor* editor, XY pos) {}
};

