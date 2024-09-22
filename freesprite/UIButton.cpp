#include "UIButton.h"
#include "FontRenderer.h"
#include "mathops.h"
#include "EventCallbackListener.h"

void UIButton::render(XY pos)
{
	SDL_Rect drawrect = { pos.x, pos.y, wxWidth, wxHeight };
	SDL_Color bgColor = focused ? colorBGFocused : colorBGUnfocused;
	SDL_Color textColor = focused ? colorTextFocused : colorTextUnfocused;
	SDL_SetRenderDrawColor(g_rd, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
	SDL_RenderFillRect(g_rd, &drawrect);
	SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
	SDL_RenderDrawRect(g_rd, &drawrect);

	if (lastClick.started) {
		double lineAnimPercent = 1.0 - XM1PW3P1(lastClick.percentElapsedTime(200));
		if (lineAnimPercent > 0) {
			SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
			drawLine(XY{ drawrect.x, drawrect.y }, XY{ drawrect.x + drawrect.w, drawrect.y }, lineAnimPercent);
			drawLine(XY{ drawrect.x, drawrect.y }, XY{ drawrect.x, drawrect.y + drawrect.h }, lineAnimPercent);
			drawLine(XY{ drawrect.x + drawrect.w, drawrect.y + drawrect.h }, XY{ drawrect.x, drawrect.y + drawrect.h }, lineAnimPercent);
			drawLine(XY{ drawrect.x + drawrect.w, drawrect.y + drawrect.h }, XY{ drawrect.x + drawrect.w, drawrect.y }, lineAnimPercent);
		}
	}

	int textX = pos.x + 2;
	if (icon != NULL) {
		SDL_Rect iconRect = SDL_Rect{ pos.x + 1, pos.y + 1, wxHeight - 2, wxHeight - 2 };
		SDL_RenderCopy(g_rd, icon, NULL, &iconRect);
		textX += iconRect.w;
	}

	if (hovered) {
		SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x70);
		renderGradient(drawrect, 0x10FFFFFF, 0x10FFFFFF, 0x40D3F4FF, 0x40D3F4FF);
		//SDL_RenderFillRect(g_rd, &drawrect);
	}
	g_fnt->RenderString(text + (focused ? "_" : ""), textX, pos.y + 2, textColor);
}

void UIButton::focusIn()
{
	
}

void UIButton::handleInput(SDL_Event evt, XY gPosOffset)
{
	if (evt.type == SDL_MOUSEBUTTONDOWN) {
		XY mousePos = xySubtract(XY{ evt.motion.x, evt.motion.y }, gPosOffset);
		if (evt.button.state && pointInBox(mousePos, SDL_Rect{ 0,0,wxWidth,wxHeight })) {
			if (evt.button.button == 1) {
				click();
			} else if (evt.button.button == 3) {
				rightClick();
			}
		}
	}
}

void UIButton::click()
{
	lastClick.start();
	if (callback != NULL) {
		callback->eventButtonPressed(callback_id);
	}
}

void UIButton::rightClick()
{
	lastClick.start();
	if (callback != NULL) {
		callback->eventButtonRightClicked(callback_id);
	}
}
