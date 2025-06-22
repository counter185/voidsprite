#include "UIButton.h"
#include "FontRenderer.h"
#include "mathops.h"
#include "TooltipsLayer.h"
#include "EventCallbackListener.h"

void UIButton::render(XY pos)
{
	lastPositionOnScreen = pos;

	SDL_Rect drawrect = { pos.x, pos.y, wxWidth, wxHeight };

	if (pos.y >= g_windowH || pos.x >= g_windowW
		|| (pos.y+wxHeight) < 0 || (pos.x+wxWidth) < 0) {
		return;
	}

	//SDL_Color bgColor = focused ? colorBGFocused : colorBGUnfocused;
	SDL_Color textColor = focused ? colorTextFocused : colorTextUnfocused;
	//SDL_SetRenderDrawColor(g_rd, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
	//SDL_RenderFillRect(g_rd, &drawrect);
    fill.fill(drawrect);
	SDL_SetRenderDrawColor(g_rd, colorBorder.r, colorBorder.g, colorBorder.b, colorBorder.a);
	SDL_RenderDrawRect(g_rd, &drawrect);

	SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x40);
	int offset = 4;
	XY p1 = { drawrect.x + offset, drawrect.y + offset };
	XY p2 = { drawrect.x + drawrect.w - offset, drawrect.y + drawrect.h - offset };
	double hoverTime = !hoverTimer.started ? 0 
					   : hovered ? XM1PW3P1(hoverTimer.percentElapsedTime(400)) : (1 - XM1PW3P1(hoverTimer.percentElapsedTime(200)));
	//SDL_RenderDrawLine(g_rd, p1.x, p1.y, p1.x + drawrect.w / 2, p1.y);
	//SDL_RenderDrawLine(g_rd, p1.x, p1.y, p1.x, p1.y + drawrect.h / 2);
	drawLine(p2, { p2.x - drawrect.w + offset, p2.y }, 0.25 + 0.65 * hoverTime);
	drawLine({ p2.x, p2.y - 1 }, { p2.x, p2.y - drawrect.h + offset}, 0.33 + 0.56 * hoverTime);

	renderAnimations(pos);

	int textX = pos.x + 5;
	if (icon != NULL) {
		SDL_Texture* icn = icon->get(g_rd);
		SDL_SetTextureAlphaMod(icn, 0xff);
		SDL_Rect iconRect = SDL_Rect{ pos.x + 1, pos.y + 1, fullWidthIcon ? (wxWidth - 2) : (wxHeight - 2), wxHeight - 2 };
		SDL_RenderCopy(g_rd, icn, NULL, &iconRect);
		textX += iconRect.w;
	}

	g_fnt->RenderString(text + (focused ? "_" : ""), textX, pos.y + 2, textColor);

	renderTooltip(pos);
}

void UIButton::focusIn()
{
	
}

void UIButton::focusOut()
{
	touchHoldingDown = false;
}

void UIButton::handleInput(SDL_Event evt, XY gPosOffset)
{
	switch (evt.type) {
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		{
			XY mousePos = xySubtract(XY{ (int)(evt.motion.x), (int)(evt.motion.y) }, gPosOffset);
			if (evt.button.down && pointInBox(mousePos, SDL_Rect{ 0,0,wxWidth,wxHeight })) {
				if (evt.button.button == 1) {
					click();
				}
				else if (evt.button.button == 3) {
					rightClick();
				}
			}
		}
			break;
		case SDL_EVENT_FINGER_DOWN:
		{
			XY touchPosition = { (int)(evt.tfinger.x * g_windowW), (int)(evt.tfinger.y * g_windowH) };
			if (pointInBox(touchPosition, SDL_Rect{ gPosOffset.x, gPosOffset.y, wxWidth, wxHeight })) {
				touchHoldDownPos = touchPosition;
				touchHoldingDown = true;
			}
		}
			break;
		case SDL_EVENT_FINGER_UP:
		{
			if (touchHoldingDown) {
				touchHoldingDown = false;
				XY touchPosition = { (int)(evt.tfinger.x * g_windowW), (int)(evt.tfinger.y * g_windowH) };
				if (pointInBox(touchPosition, SDL_Rect{ gPosOffset.x, gPosOffset.y, wxWidth, wxHeight })) {
					click();
				}
			}
		}
			break;
		case SDL_EVENT_FINGER_MOTION:
			if (touchHoldingDown) {
				XY touchPosition = { (int)(evt.tfinger.x * g_windowW), (int)(evt.tfinger.y * g_windowH) };
				if (xyDistance(touchPosition, touchHoldDownPos) > 20) {
					touchHoldingDown = false;
				}
			}
			break;
	}
}

void UIButton::renderAnimations(XY pos)
{
	SDL_Rect drawrect = { pos.x, pos.y, wxWidth, wxHeight };

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

	if (hovered || touchHoldingDown) {
		renderGradient(drawrect, 0x10FFFFFF, 0x10FFFFFF, 0x40D3F4FF, 0x40D3F4FF);
		SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x70);
		SDL_RenderDrawRect(g_rd, &drawrect);
	}

}

void UIButton::renderTooltip(XY pos)
{
	if (hovered) {
		if (!tooltip.empty() && (instantTooltip || hoverTimer.percentElapsedTime(1000) == 1.0f)) {
			g_ttp->addTooltip(Tooltip{ xyAdd(pos, {0, wxHeight}), tooltip, {255,255,255,255}, hoverTimer.percentElapsedTime(300, instantTooltip ? 0 : 1000) });
		}
	}
}

void UIButton::click()
{
	lastClick.start();
	g_newVFX(VFX_BUTTONPULSE, 700, 0x80FFFFFF, { lastPositionOnScreen.x, lastPositionOnScreen.y, wxWidth, wxHeight });
    if (onClickCallback != NULL) {
        onClickCallback(this);
    } 
	else if (callback != NULL) {
		callback->eventButtonPressed(callback_id);
	}
}

void UIButton::rightClick()
{
	lastClick.start();
	if (onRightClickCallback != NULL) {
		onRightClickCallback(this);
	}
	else if (callback != NULL) {
		callback->eventButtonRightClicked(callback_id);
	}
}
