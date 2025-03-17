#pragma once
#include "globals.h"
#include "mathops.h"
#include "Timer64.h"

class Drawable
{
protected:
	bool hovered = false;
	Timer64 focusTimer, hoverTimer;
public:
	DrawableManager* parentManager = NULL;
	bool focused = false;
	XY position = XY{ 50,50 };
	XY anchor = XY{ 0,0 };

	int callback_id = -1;
	EventCallbackListener* callback = NULL;

	virtual ~Drawable() {}

	virtual bool isMouseIn(XY thisPositionOnScreen, XY mousePos) { return false; }
	virtual bool clickable() { return true; }
	virtual bool focusableWithTab() { return false; }
	virtual bool takesMouseWheelEvents() { return false; }
	virtual void render(XY position) {}
	virtual void mouseHoverIn() {
		hovered = true;
		hoverTimer.start();
	}
	virtual void mouseHoverOut() {
		hovered = false;
		hoverTimer.start();
	}
	virtual void mouseHoverMotion(XY mousePos, XY gPosOffset = {0,0}) {}
	virtual void mouseWheelEvent(XY mousePos, XY gPosOffset, XY direction) {}
	virtual void focusIn() { 
		focused = true;
		focusTimer.start();
	}
	virtual void focusOut() { 
		focused = false; 
		focusTimer.start();
	}
	virtual bool takesTouchEvents() { return false; }
	virtual bool focusable() { return true; }
	virtual void handleInput(SDL_Event evt, XY gPosOffset = {0,0}) {}
	virtual void setCallbackListener(int cb_id,  EventCallbackListener* callback) { callback_id = cb_id; this->callback = callback; }
	virtual XY getDimensions() { return XY{ 0,0 }; };
	virtual XY getRenderDimensions() { return getDimensions(); };
	virtual bool isPanel() { return false; }
	virtual bool shouldMoveToFrontOnFocus() { return false; }

	XY anchorPos(XY origin, XY originDimensions, XY thisPositionLocal, XY thisDimensions, XY anchor) {
		XY ret;
		XY offset = thisPositionLocal;
		XY endpoint = xyAdd(origin, originDimensions);
		if (anchor.x == 1) {
			ret.x = endpoint.x - offset.x - thisDimensions.x;
		}
		else {
			ret.x = origin.x + thisPositionLocal.x;
		}

		if (anchor.y == 1) {
			ret.y = endpoint.y - offset.y - thisDimensions.y;
		}
		else {
			ret.y = origin.y + thisDimensions.y;
		}
		return ret;
	}
};

