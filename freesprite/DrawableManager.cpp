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
	for (Drawable*& a : drawablesList) {
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

void DrawableManager::forceUnfocus() {
	if (focused != NULL) {
		focused->focusOut();
		focused = NULL;
	}
}