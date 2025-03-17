#include "PanelReference.h"
#include "UIButton.h"

PanelReference::PanelReference(Layer* t)
{
	previewTex = t;

	wxWidth = 400;
	wxHeight = 300;

	c.dimensions = { previewTex->w, previewTex->h };
	c.recenter({wxWidth, wxHeight});

	UIButton* closeBtn = new UIButton();
	closeBtn->wxWidth = 30;
	closeBtn->wxHeight = 20;
	closeBtn->position = { wxWidth - 5 - closeBtn->wxWidth, 5 };
	closeBtn->text = "X";
	closeBtn->onClickCallback = [&](UIButton* caller) {
		DrawableManager* wxs = getTopmostParent()->parentManager;
		wxs->removeDrawable(getTopmostParent());
	};
	subWidgets.addDrawable(closeBtn);
}

PanelReference::~PanelReference()
{
	if (previewTex != NULL) {
		delete previewTex;
	}
}

bool PanelReference::isMouseIn(XY thisPositionOnScreen, XY mousePos)
{
	XY origin = xyAdd(thisPositionOnScreen, { 5,30 });
	SDL_Rect canvasDraw = { origin.x, origin.y, wxWidth - 10, wxHeight - 40 };
	return Panel::isMouseIn(thisPositionOnScreen, mousePos) || (enabled && pointInBox(mousePos, canvasDraw));
}

void PanelReference::handleInput(SDL_Event evt, XY gPosOffset)
{
	XY origin = xyAdd(gPosOffset, { 5,30 });
	SDL_Rect canvasDraw = { origin.x, origin.y, wxWidth - 10, wxHeight - 40 };
	if (evt.type == SDL_EVENT_MOUSE_BUTTON_DOWN && pointInBox({(int)evt.button.x, (int)evt.button.y}, canvasDraw)) {
		dragging++;
	}
	else if (dragging > 0 && evt.type == SDL_EVENT_MOUSE_BUTTON_UP) {
		dragging = 0;
	}
	else if (dragging && evt.type == SDL_EVENT_MOUSE_MOTION) {
		c.panCanvas({ (int)evt.motion.xrel, (int)evt.motion.yrel });
	}
	else {
		Panel::handleInput(evt, gPosOffset);
	}
}

void PanelReference::render(XY at)
{
	if (!enabled) {
		return;
	}
	
	SDL_Rect panelRect = { at.x, at.y, wxWidth, wxHeight };
	XY origin = xyAdd(at, { 5, 30 });

	renderGradient(panelRect, 0x80000000, 0x80000000, 0x80000000, 0x80404040);
	
	if (thisOrParentFocused()) {
		SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 255);
		drawLine({ at.x, at.y }, { at.x, at.y + wxHeight }, XM1PW3P1(thisOrParentFocusTimer().percentElapsedTime(300)));
		drawLine({ at.x, at.y }, { at.x + wxWidth, at.y }, XM1PW3P1(thisOrParentFocusTimer().percentElapsedTime(300)));
	}

	SDL_Rect canvasDraw = { origin.x, origin.y, wxWidth - 10, wxHeight - 40 };
	g_pushClip(canvasDraw);
	c.lockToScreenBounds(0, 0, 0, 0, { canvasDraw.w, canvasDraw.h });
	SDL_Rect texDraw = c.getCanvasOnScreenRect();
	texDraw.x += at.x;
	texDraw.y += at.y;
	previewTex->render(texDraw);
	g_popClip();

	Panel::render(at);

}
