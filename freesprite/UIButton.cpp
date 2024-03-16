#include "UIButton.h"
#include "FontRenderer.h"

void UIButton::render(XY pos)
{
	SDL_Rect drawrect = { pos.x, pos.y, wxWidth, wxHeight };
	SDL_SetRenderDrawColor(g_rd, 0x0, 0x0, 0x0, focused ? 0xff : 0x30);
	SDL_RenderFillRect(g_rd, &drawrect);

	g_fnt->RenderString(text + (focused ? "_" : ""), pos.x + 2, pos.y + 2, SDL_Color{ 0xff,0xff,0xff,(unsigned char)(focused ? 0xff : 0xd0) });
}

void UIButton::focusIn()
{
	if (callback != NULL) {
		callback->eventButtonPressed(callback_id);
	}
}
