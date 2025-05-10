#pragma once
#include "globals.h"
#include "drawable.h"
#include "mathops.h"

class DrawableManager
{
private:
	void removeDrawableWithoutUnfocus(Drawable* d, bool free = true);
public:
	std::vector<Drawable*> drawablesList;

	~DrawableManager() {
		if (drawablesList.size() == 0) {
			//logprintf("[DrawableManager] duplicate destructor\n");
		}
		freeAllDrawables(); 
	}

	static void processHoverEventInMultiple(std::vector<std::reference_wrapper<DrawableManager>> wxss, SDL_Event evt, XY parentOffset = XY{ 0,0 });
	static bool processInputEventInMultiple(std::vector<std::reference_wrapper<DrawableManager>> wxss, SDL_Event evt, XY parentOffset = XY{0,0});
	static bool processMouseWheelEventInMultiple(std::vector<std::reference_wrapper<DrawableManager>> wxss, SDL_Event evt, XY parentOffset = XY{0,0});

	void addDrawable(Drawable* d);
	void removeDrawable(Drawable* d, bool free = true);
	void renderAll(XY offset = XY{0,0});
	void moveToFront(Drawable* d);
	void passInputToFocused(SDL_Event evt, XY parentOffset = XY{0,0});
	bool anyFocused() { return focused != NULL; }
	bool tryFocusOnPoint(XY screenPoint, XY parentOffset = XY{0,0});
	bool tryFocusOnNextTabbable();
	void forceFocusOn(Drawable* d);
	void forceUnfocus();
	void forceUnhover();
	bool mouseInAny(XY thisPositionOnScreen, XY mousePos);
	bool processHoverEvent(XY thisPositionOnScreen, XY mousePos);
	bool processMouseWheelEvent(XY thisPositionOnScreen, XY mousePos, XY scrollDirection);
	//void tickAnchors();

	void freeAllDrawables();
private:
	Drawable* focused = NULL;
	Drawable* hoverTarget = NULL;
};

