#include "GlobalNavBar.h"

void GlobalNavBar::render(XY position)
{
	SDL_Rect r = SDL_Rect{0,0,g_windowW, wxHeight};
	SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0xa0);
	SDL_RenderFillRect(g_rd, &r);

	SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xa0);
	SDL_RenderDrawLine(g_rd, 0, wxHeight, g_windowW, wxHeight);
}

void GlobalNavBar::handleInput(SDL_Event evt, XY gPosOffset)
{
	if (evt.type == SDL_KEYDOWN) {
		if (keyBinds.contains(evt.key.keysym.sym)) {
			keyBinds[evt.key.keysym.sym].function(parent);
		}
	}
}
