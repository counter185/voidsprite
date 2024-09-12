#include "UIDropdown.h"
#include "FontRenderer.h"
#include "UIButton.h"

UIDropdown::UIDropdown(std::vector<std::string> items)
{
	this->items = items;
	genButtons();
}

void UIDropdown::render(XY pos)
{
	if (isOpen) {
		wxs.renderAll(xySubtract(pos, {0, (int)((1.0f - openTimer.percentElapsedTime(100)) * wxHeight)}));
	}

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

	g_fnt->RenderString(text + (focused ? "_" : ""), textX, pos.y + 2, textColor);
}

void UIDropdown::focusIn()
{
}

void UIDropdown::focusOut()
{
	isOpen = false;
	wxs.forceUnfocus();
}

void UIDropdown::handleInput(SDL_Event evt, XY gPosOffset)
{
	if (isOpen && evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == 1 && evt.button.state) {
		wxs.tryFocusOnPoint(XY{ evt.button.x, evt.button.y }, gPosOffset);
	}
	if (!isOpen || !wxs.anyFocused()) {
		if (evt.type == SDL_MOUSEBUTTONDOWN) {
			XY mousePos = xySubtract(XY{ evt.motion.x, evt.motion.y }, gPosOffset);
			if (evt.button.button == 1 && evt.button.state) {
				if (pointInBox(mousePos, SDL_Rect{ 0,0,wxWidth,wxHeight })) {
					click();
				}
			}
		}
	}
	else {
		wxs.passInputToFocused(evt, gPosOffset);
	}
}

void UIDropdown::eventButtonPressed(int evt_id)
{
	if (callback != NULL) {
		callback->eventDropdownItemSelected(callback_id, evt_id, items[evt_id]);
	}
	isOpen = false;
}

void UIDropdown::genButtons()
{
	wxs.freeAllDrawables();

	for (int y = 0; y < items.size(); y++) {
		UIButton* btn = new UIButton();
		btn->text = items[y];
		btn->wxWidth = wxWidth;
		btn->wxHeight = wxHeight;
		btn->colorBGFocused = btn->colorBGUnfocused = colorBGFocused;
		btn->position = { 0, wxHeight + y * btn->wxHeight };
		btn->setCallbackListener(y, this);
		wxs.addDrawable(btn);
	}
}

void UIDropdown::click()
{
	lastClick.start();
	openTimer.start();
	isOpen = true;
}
