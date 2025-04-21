#include "UITextField.h"
#include "FontRenderer.h"
#include "EventCallbackListener.h"
#include "TooltipsLayer.h"

void UITextField::render(XY pos)
{
	SDL_Rect drawrect = { pos.x, pos.y, wxWidth, wxHeight };
	SDL_SetRenderDrawColor(g_rd, bgColor.r, bgColor.g, bgColor.b, focused ? 0xff : 0x80);
	SDL_RenderFillRect(g_rd, &drawrect);
	SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
	SDL_RenderDrawRect(g_rd, &drawrect);

	if (focused) {
		double lineAnimPercent = XM1PW3P1(focusTimer.percentElapsedTime(500));
		SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
		drawLine(XY{ drawrect.x, drawrect.y }, XY{ drawrect.x + drawrect.w, drawrect.y }, lineAnimPercent);
		drawLine(XY{ drawrect.x, drawrect.y }, XY{ drawrect.x, drawrect.y + drawrect.h }, lineAnimPercent);
		drawLine(XY{ drawrect.x + drawrect.w, drawrect.y + drawrect.h }, XY{ drawrect.x, drawrect.y + drawrect.h }, lineAnimPercent);
		drawLine(XY{ drawrect.x + drawrect.w, drawrect.y + drawrect.h }, XY{ drawrect.x + drawrect.w, drawrect.y }, lineAnimPercent);

		
		SDL_SetTextInputArea(g_wd, &drawrect, 0);

		if (imeCandidates.size() > 0) {
			XY imeCandsOrigin = xyAdd(pos, { 0, wxHeight });
			for (int i = 0; i < imeCandidates.size(); i++) {
				g_ttp->addTooltip(Tooltip{ imeCandsOrigin, imeCandidates[i], i == imeCandidateIndex ? SDL_Color{0,255,0,255} : SDL_Color{ 255,255,255,255 }, XM1PW3P1(imeCandidatesTimer.percentElapsedTime(400))});
				imeCandsOrigin.y += 30;
			}
			
		}
	}

	if (hovered) {
		renderGradient(drawrect, 0x08FFFFFF, 0x08FFFFFF, 0x20D3F4FF, 0x20D3F4FF);
		SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x70);
		SDL_RenderDrawRect(g_rd, &drawrect);

		if (!tooltip.empty() && hoverTimer.percentElapsedTime(1000) == 1.0f) {
			g_ttp->addTooltip(Tooltip{ xyAdd(pos, {0, wxHeight}), tooltip, {255,255,255,255}, hoverTimer.percentElapsedTime(300, 1000) });
		}
	}
	
	if (!isColorField || !isValidOrPartialColor() || text.empty()) {
		g_fnt->RenderString(text + (focused ? "_" : ""), pos.x + 2, pos.y + 2, SDL_Color{ textColor.r,textColor.g,textColor.b,(unsigned char)(focused ? 0xff : 0xa0) });
	}
	else {
		int textPtr = 0;
		XY origin = xyAdd(pos, { 2,2 });
		if (text[0] == '#') {
			origin = g_fnt->RenderString("#", origin.x, origin.y, SDL_Color{0x80,0x80,0x80,255});
			textPtr++;
		}
		origin = g_fnt->RenderString(text.substr(textPtr, ixmin(2, text.size() - textPtr)), origin.x, origin.y, SDL_Color{255,0x32,0x32,255});
		textPtr += 2;
		if (textPtr < text.size()) {
			origin = g_fnt->RenderString(text.substr(textPtr, ixmin(2, text.size() - textPtr)), origin.x, origin.y, SDL_Color{0x50,255,0x50,255});
			textPtr += 2;
		}
		if (textPtr < text.size()) {
			origin = g_fnt->RenderString(text.substr(textPtr, ixmin(2, text.size() - textPtr)), origin.x, origin.y, SDL_Color{0x18,0x9A,255,255});
			textPtr += 2;
		}
		if (textPtr < text.size()) {
			origin = g_fnt->RenderString(text.substr(textPtr), origin.x, origin.y);
		}
		if (focused) {
			g_fnt->RenderString("_", origin.x, origin.y);
		
		}
	}
}

void UITextField::handleInput(SDL_Event evt, XY gPosOffset)
{
	if (evt.type == SDL_KEYDOWN) {
		switch (KEYCODE(evt)) {
			case SDL_SCANCODE_TAB:
				break;
			case SDL_SCANCODE_RETURN:
				if (callback != NULL) {
					callback->eventTextInputConfirm(callback_id, text);
				}
				break;
			case SDL_SCANCODE_BACKSPACE:
				if (!text.empty()) {
					text = text.substr(0, text.size() - 1);
					if (callback != NULL) {
						callback->eventTextInput(callback_id, text);
					}
				}
				break;
			case SDL_SCANCODE_DELETE:
				text = "";
				break;
		}
	}
	else if (evt.type == SDL_TEXTINPUT) {
		bool textAdded = false;
		char* nextc = (char*)evt.text.text;
		while (*nextc != '\0') {
			char c = *nextc;
			if ((isNumericField && c >= '0' && c <= '9')
				|| !isNumericField) {
				text += c;
				textAdded = true;
			}
			nextc++;
		}
		if (textAdded && callback != NULL) {
			callback->eventTextInput(callback_id, text);
		}
	}
	else if (evt.type == SDL_EVENT_TEXT_EDITING_CANDIDATES) {
		if (imeCandidates.size() == 0) {
            imeCandidatesTimer.start();
		}
		imeCandidates.clear();
		for (int x = 0; x < evt.edit_candidates.num_candidates; x++) {
            imeCandidates.push_back(evt.edit_candidates.candidates[x]);
		}
		imeCandidateIndex = evt.edit_candidates.selected_candidate;
	}
}

bool UITextField::isValidOrPartialColor()
{
	for (int x = 0; x < text.size(); x++) {
		char c = tolower(text[x]);
		if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c == '#' && x == 0))) {
			return false;
		}
	}
	return true;
}
