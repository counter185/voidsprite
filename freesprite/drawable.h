#pragma once
#include "globals.h"
#include "EventCallbackListener.h"

class Drawable
{
public:
	bool focused = false;
	XY position = XY{ 50,50 };

	int callback_id = -1;
	EventCallbackListener* callback = NULL;

	virtual bool isMouseIn(XY thisPositionOnScreen, XY mousePos) { return false; }
	virtual bool clickable() { return true; }
	virtual void render(XY position) {}
	virtual void mouseHoverIn() {}
	virtual void mouseHoverOut() {}
	virtual void focusIn() { focused = true; }
	virtual void focusOut() { focused = false; }
	virtual bool focusable() { return true; }
	virtual void handleInput(SDL_Event evt, XY gPosOffset = {0,0}) {}
	virtual void setCallbackListener(int cb_id,  EventCallbackListener* callback) { callback_id = cb_id; this->callback = callback; }
};

