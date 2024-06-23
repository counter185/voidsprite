#pragma once
#include "globals.h"
#include "drawable.h"
#include "mathops.h"

class DrawableManager
{
public:
	std::vector<Drawable*> drawablesList;

	void addDrawable(Drawable* d);
	void removeDrawable(Drawable* d);
	void renderAll(XY offset = XY{0,0});
	void moveToFront(Drawable* d);
	void passInputToFocused(SDL_Event evt, XY parentOffset = XY{0,0}) { focused->handleInput(evt, xyAdd(parentOffset, focused->position)); }
	bool anyFocused() { return focused != NULL; }
	bool tryFocusOnPoint(XY screenPoint, XY parentOffset = XY{0,0});
	bool tryFocusOnNextTabbable();
	void forceFocusOn(Drawable* d);
	void forceUnfocus();
	bool mouseInAny(XY thisPositionOnScreen, XY mousePos);
	//void tickAnchors();

	void freeAllDrawables();
private:
	Drawable* focused = NULL;
};

