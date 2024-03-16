#include "StartScreen.h"
#include "FontRenderer.h"

void StartScreen::tick() {

}

void StartScreen::render()
{
	SDL_Rect bgr = SDL_Rect{ 0, 35, 300, 300 };
	SDL_SetRenderDrawColor(g_rd, 0x20, 0x20, 0x20, 0xff);
	SDL_RenderFillRect(g_rd, &bgr);

	g_fnt->RenderString("voidsprite", 10, 40);

	g_fnt->RenderString("New image", 10, 80);
	g_fnt->RenderString("Width", 10, 120);
	g_fnt->RenderString("Height", 10, 155);

	wxsManager.renderAll();
}

void StartScreen::takeInput(SDL_Event evt)
{

	if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == 1 && evt.button.state) {
		wxsManager.tryFocusOnPoint(XY{ evt.button.x, evt.button.y });
	}

	if (!wxsManager.anyFocused()) {
		switch (evt.type) {
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				break;
			case SDL_MOUSEMOTION:
				break;
			case SDL_MOUSEWHEEL:
				break;
			case SDL_KEYDOWN:
				break;
		}
	}
	else {
		wxsManager.passInputToFocused(evt);
	}
}
