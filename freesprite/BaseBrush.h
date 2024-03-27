#pragma once
#include "globals.h"

class BaseBrush
{
public:
	SDL_Texture* cachedIcon = NULL;
	XY lastMouseMotionPos = XY{ 0,0 };

	virtual void resetState() {}
	virtual bool isReadOnly() { return false; }
	virtual bool overrideRightClick() { return false; }
	virtual std::string getIconPath() { return "assets/brush_default.png"; }
	virtual std::string getName() { return "Base brush"; }
	virtual void mouseMotion(MainEditor* editor, XY pos) {
		lastMouseMotionPos = pos;
	}
	virtual void clickPress(MainEditor* editor, XY pos) {}
	virtual void clickDrag(MainEditor* editor, XY from, XY to) {}
	virtual void clickRelease(MainEditor* editor, XY pos) {}
	virtual void rightClickPress(MainEditor* editor, XY pos) {}
	virtual void renderOnCanvas(XY canvasDrawPoint, int scale) {}

	void drawLocalPoint(XY canvasOrigin, XY point, int scale) {
		SDL_Rect r = { canvasOrigin.x + point.x * scale, canvasOrigin.y + point.y * scale, scale, scale };
		SDL_RenderFillRect(g_rd, &r);
	}
};

