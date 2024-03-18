#include "UIButton.h"
#include "FontRenderer.h"
#include "mathops.h"

void UIButton::render(XY pos)
{
	SDL_Rect drawrect = { pos.x, pos.y, wxWidth, wxHeight };
	SDL_Color bgColor = focused ? colorBGFocused : colorBGUnfocused;
	SDL_Color textColor = focused ? colorTextFocused : colorTextUnfocused;
	SDL_SetRenderDrawColor(g_rd, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
	SDL_RenderFillRect(g_rd, &drawrect);

	g_fnt->RenderString(text + (focused ? "_" : ""), pos.x + 2, pos.y + 2, textColor);
}

void UIButton::focusIn()
{
	
}

void UIButton::handleInput(SDL_Event evt, XY gPosOffset)
{
	if (evt.type == SDL_MOUSEBUTTONDOWN) {
		XY mousePos = xySubtract(XY{ evt.motion.x, evt.motion.y }, gPosOffset);
		if (evt.button.button == 1 && evt.button.state) {
			if (pointInBox(mousePos, SDL_Rect{ 0,0,wxWidth,wxHeight })) {
				if (callback != NULL) {
					callback->eventButtonPressed(callback_id);
				}
			}
		}
	}
}
