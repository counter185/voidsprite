#include "UITextField.h"
#include "FontRenderer.h"

void UITextField::render(XY pos)
{
	SDL_Rect drawrect = { pos.x, pos.y, wxWidth, wxHeight };
	SDL_SetRenderDrawColor(g_rd, 0x0, 0x0, 0x0, focused ? 0xff : 0x80);
	SDL_RenderFillRect(g_rd, &drawrect);
	SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, focused ? 0x80 : 0x30);
	SDL_RenderDrawRect(g_rd, &drawrect);
	
	g_fnt->RenderString(text + (focused ? "_" : ""), pos.x + 2, pos.y + 2, SDL_Color{0xff,0xff,0xff,(unsigned char)(focused ? 0xff : 0xa0)});
}

void UITextField::handleInput(SDL_Event evt, XY gPosOffset)
{
	if (evt.type == SDL_KEYDOWN) {
		switch (evt.key.keysym.sym) {
			case SDLK_TAB:
				break;
			case SDLK_RETURN:
				if (callback != NULL) {
					callback->eventTextInputConfirm(callback_id, text);
				}
				break;
			case SDLK_BACKSPACE:
				if (!text.empty()) {
					text = text.substr(0, text.size() - 1);
				}
				break;
		}
	}
	else if (evt.type == SDL_TEXTINPUT) {
		bool textAdded = false;
		for (char& c : evt.text.text) {
			if (c == '\0') {
				break;
			}
			if ((numeric && c >= '0' && c <= '9')
				|| !numeric) {
				text += c;
				textAdded = true;
			}
		}
		if (textAdded && callback != NULL) {
			callback->eventTextInput(callback_id, text);
		}
	}
}
