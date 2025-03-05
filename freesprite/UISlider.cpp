#include "FontRenderer.h"
#include "UISlider.h"
#include "mathops.h"

void UISlider::drawPosIndicator(XY origin) {
	//g_fnt->RenderString(std::string("pos:") + std::to_string(sliderPos), origin.x + 10, origin.y + wxHeight / 4);

	XY centerPoint = xyAdd(origin, XY{ (int)(wxWidth * sliderPos), 0});
	int xdist = 3;
	SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 255);
	
	for (int x = 0; x < 2; x++) {
		int fxdist = xdist + x;	
		SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y - fxdist, centerPoint.x - fxdist, centerPoint.y + wxHeight + fxdist);
		SDL_RenderDrawLine(g_rd, centerPoint.x + fxdist, centerPoint.y - fxdist, centerPoint.x + fxdist, centerPoint.y + wxHeight + fxdist);
		//draws first triangle
		SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y - fxdist, centerPoint.x, centerPoint.y);
		SDL_RenderDrawLine(g_rd, centerPoint.x + fxdist, centerPoint.y - fxdist, centerPoint.x, centerPoint.y);
		//draws top/bottom bars
		SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y + wxHeight + fxdist, centerPoint.x + fxdist, centerPoint.y + wxHeight + fxdist);
		SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y - fxdist, centerPoint.x + fxdist, centerPoint.y - fxdist);
		//draws second triangle
		SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y + wxHeight + fxdist, centerPoint.x, centerPoint.y + wxHeight);
		SDL_RenderDrawLine(g_rd, centerPoint.x + fxdist, centerPoint.y + wxHeight + fxdist, centerPoint.x, centerPoint.y + wxHeight);
		SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 255);
	}
}

void UISlider::render(XY pos)
{
	SDL_Rect drawrect = { pos.x, pos.y, wxWidth, wxHeight };
	SDL_SetRenderDrawColor(g_rd, 0x0, 0x0, 0x0, focused ? 0xff : 0x80);
	SDL_RenderFillRect(g_rd, &drawrect);
	SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, focused ? 0x80 : 0x30);
	SDL_RenderDrawRect(g_rd, &drawrect);
	drawPosIndicator(pos);
}

void UISlider::handleInput(SDL_Event evt, XY gPosOffset)
{
	switch (evt.type) {
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			if (evt.button.button == 1) {
				if (evt.button.down) {
					XY mousePos = xySubtract(XY{ (int)evt.button.x, (int)evt.button.y }, gPosOffset);
					if (mousePos.y >= 0 && mousePos.y <= wxHeight) {
						sliderPos = fclamp(0.0f, mousePos.x / (float)wxWidth, 1.0f);
						this->onSliderPosChanged();
					}
				}
				else {
					if (mouseHeld) {
						this->onSliderPosFinishedChanging();
					}
				}
				mouseHeld = evt.button.down;
			}
			break;
		case SDL_MOUSEMOTION:
			if (mouseHeld) {
				XY mousePos = xySubtract(XY{ (int)(evt.motion.x), (int)(evt.motion.y) }, gPosOffset);
				if (mousePos.y >= 0 && mousePos.y <= wxHeight) {
					sliderPos = fclamp(0.0f, mousePos.x / (float)wxWidth, 1.0f);
					this->onSliderPosChanged();
				}
			}
			break;
	}
}
