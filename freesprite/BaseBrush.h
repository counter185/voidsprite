#pragma once
#include "globals.h"

class BaseBrush
{
public:
	SDL_Texture* cachedIcon = NULL;

	virtual void resetState() {}
	virtual bool isReadOnly() { return false; }
	virtual std::string getIconPath() { return "assets/brush_default.png"; }
	virtual std::string getName() { return "Base brush"; }
	virtual void clickPress(MainEditor* editor, XY pos) {}
	virtual void clickDrag(MainEditor* editor, XY from, XY to) {}
	virtual void clickRelease(MainEditor* editor, XY pos) {}
};

