#pragma once
#include "globals.h"

class Drawable
{
public:
	bool focused = false;
	XY position = XY{ 50,50 };

	virtual bool isMouseIn(XY thisPositionOnScreen, XY mousePos) { return false; }
	virtual bool clickable() { return true; }
	virtual void render(XY position) {}
	virtual void mouseHoverIn() {}
	virtual void mouseHoverOut() {}
	virtual void focusIn() { focused = true; }
	virtual void focusOut() { focused = false; }
	virtual bool focusable() { return true; }
	virtual void handleInput(SDL_Event evt, XY gPosOffset = {0,0}) {}
};

