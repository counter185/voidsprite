#include "DrawableManager.h"
#include "mathops.h"

void DrawableManager::addDrawable(Drawable* d) {
	drawablesList.push_back(d);
}

void DrawableManager::removeDrawable(Drawable* d) {
	//todo
}

void DrawableManager::renderAll(XY offset) {
	for (Drawable*& a : drawablesList) {
		a->render(xyAdd(a->position, offset));
	}
}

void DrawableManager::moveToFront(Drawable* d) {
	removeDrawable(d);
	addDrawable(d);
}

bool DrawableManager::tryFocusOnPoint(XY screenPoint, XY parentOffset) {
	for (int x = (int)drawablesList.size()-1; x >= 0; x--) {
		Drawable* a = drawablesList[x];
	//for (Drawable*& a : drawablesList) {
		if (a->isMouseIn(xyAdd(a->position, parentOffset), screenPoint)) {
			if (focused != a) {
				if (focused != NULL) {
					focused->focusOut();
				}
				a->focusIn();
			}
			focused = a;
			return true;
		}
	}
	if (focused != NULL) {
		focused->focusOut();
	}
	focused = NULL;
	return false;
}

void DrawableManager::forceFocusOn(Drawable* d)
{
	if (focused != d && focused != NULL) {
		focused->focusOut();
	}
	if (d != NULL) {
		d->focusIn();
	}
	focused = d;
}

void DrawableManager::forceUnfocus() {
	if (focused != NULL) {
		focused->focusOut();
		focused = NULL;
	}
}

bool DrawableManager::mouseInAny(XY origin, XY mousePos)
{
	for (Drawable*& d : drawablesList) {
		XY renderPoint = xyAdd(d->position, origin);
		if (d->isMouseIn(renderPoint, mousePos)) {
			return true;
		}
	}
	return false;
}

void DrawableManager::freeAllDrawables()
{
	for (int x = 0; x < drawablesList.size(); x++) {
		delete drawablesList[x];
	}
	drawablesList.clear();
}
