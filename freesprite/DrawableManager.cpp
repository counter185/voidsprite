#include "DrawableManager.h"
#include "mathops.h"
#include "Panel.h"

void DrawableManager::processHoverEventInMultiple(std::vector<std::reference_wrapper<DrawableManager>> wxss, SDL_Event evt, XY parentOffset)
{
	if (evt.type != SDL_MOUSEMOTION) {
		return;
	}
	bool found = false;
	for (auto& wxsw : wxss)
	{
		auto& wxs = wxsw.get();
		if (found) {
			wxs.forceUnhover();
		}
		else {
			found = wxs.processHoverEvent(parentOffset, { evt.motion.x, evt.motion.y });
		}
	}
}

bool DrawableManager::processInputEventInMultiple(std::vector<std::reference_wrapper<DrawableManager>> wxss, SDL_Event evt, XY parentOffset)
{
	if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.state) {
		for (auto& wxsw : wxss) {
			auto& wxs = wxsw.get();
			if (wxs.tryFocusOnPoint(XY{ evt.button.x, evt.button.y }, parentOffset)) {
				break;
			}
		}
	}
	else if (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_TAB) {
		for (auto& wxsw : wxss) {
			auto& wxs = wxsw.get();
			if (wxs.anyFocused()) {
				wxs.tryFocusOnNextTabbable();
				break;
			}
		}
	}

	for (auto& wxsw : wxss) {
		auto& wxs = wxsw.get();
		if (wxs.anyFocused()) {
			wxs.passInputToFocused(evt, parentOffset);
			return true;
		}
	}
	return false;
}

void DrawableManager::addDrawable(Drawable* d) {
	drawablesList.push_back(d);
}

void DrawableManager::removeDrawable(Drawable* d) {
	if (focused == d) {
		forceUnfocus();
	}
	for (int x = 0; x < drawablesList.size(); x++) {
		if (drawablesList[x] == d) {
			delete drawablesList[x];
			drawablesList.erase(drawablesList.begin() + x);
			return;
		}
	}
}

void DrawableManager::renderAll(XY offset) {
	//tickAnchors();

	for (Drawable*& a : drawablesList) {
		//XY position = xyAdd(a->position, offset);
		//TODO: DON'T JUST FUCKING XY{g_windowW, g_windowH}
		//XY position = xyAdd(a->position, a->anchorPos(offset, XY{ g_windowW, g_windowH }, a->position, a->getDimensions(), a->anchor));
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
		//XY anchorPosition = a->anchorPos(parentOffset, XY{ g_windowW, g_windowH }, a->position, a->getDimensions(), a->anchor);
	//for (Drawable*& a : drawablesList) {
		if (a->focusable() && a->isMouseIn(xyAdd(a->position, parentOffset), screenPoint)) {
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

bool DrawableManager::tryFocusOnNextTabbable()
{
	if (focused != NULL && focused->isPanel()) {
		return ((Panel*)focused)->subWidgets.tryFocusOnNextTabbable();
	}
	int currentIndex = focused == NULL ? 0 : std::find(drawablesList.begin(), drawablesList.end(), focused) - drawablesList.begin();
	for (int xx = 0; xx < drawablesList.size(); xx++) {
		int x = (currentIndex + xx) % drawablesList.size();
		Drawable* a = drawablesList[x];
		if (a == focused) {
			continue;
		}
		if (a->focusable() && a->focusableWithTab()) {
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

void DrawableManager::forceUnhover()
{
	if (hoverTarget != NULL) {
		hoverTarget->mouseHoverOut();
		hoverTarget = NULL;
	}
}

bool DrawableManager::mouseInAny(XY origin, XY mousePos)
{
	for (Drawable*& d : drawablesList) {
		//XY anchorPosition = d->anchorPos(origin, XY{ g_windowW, g_windowH }, d->position, d->getDimensions(), d->anchor);
		XY renderPoint = xyAdd(d->position, origin);
		if (d->isMouseIn(renderPoint, mousePos)) {
			return true;
		}
	}
	return false;
}

bool DrawableManager::processHoverEvent(XY thisPositionOnScreen, XY mousePos)
{
	Drawable* newHoverTarget = NULL;
	for (auto dd = drawablesList.rbegin(); dd != drawablesList.rend(); dd++) {
		Drawable* d = *dd;
	//for (Drawable*& d : drawablesList) {
		//XY anchorPosition = d->anchorPos(thisPositionOnScreen, XY{ g_windowW, g_windowH }, d->position, d->getDimensions(), d->anchor);
		XY renderPoint = xyAdd(d->position, thisPositionOnScreen);
		if (d->isMouseIn(renderPoint, mousePos)) {
			newHoverTarget = d;
			break;
		}
	}
	if (hoverTarget != newHoverTarget) {
		if (hoverTarget != NULL) {
			hoverTarget->mouseHoverOut();
		}
		if (newHoverTarget != NULL) {
			newHoverTarget->mouseHoverIn();
		}
		hoverTarget = newHoverTarget;
	}
	if (hoverTarget != NULL) {
		hoverTarget->mouseHoverMotion(mousePos, thisPositionOnScreen);
	}
	return hoverTarget != NULL;
}

/*void DrawableManager::tickAnchors()
{
	for (Drawable*& d : drawablesList) {
		if (d->anchor.x == 1) {
			d->position.x = g_windowW - d->getDimensions().x;
		}
		if (d->anchor.y == 1) {
			d->position.y = g_windowH - d->getDimensions().y;
		}
	}
}*/

void DrawableManager::freeAllDrawables()
{
	forceUnfocus();
	forceUnhover();
	for (int x = 0; x < drawablesList.size(); x++) {
		delete drawablesList[x];
	}
	drawablesList.clear();
}
