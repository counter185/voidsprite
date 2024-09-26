#include "globals.h"
#include "mathops.h"
#include "UISVPicker.h"
#include "EditorColorPicker.h"
#include "FontRenderer.h"

void UISVPicker::drawPosIndicator(XY origin)
{
	//g_fnt->RenderString(std::string("vpos:") + std::to_string(vPos), origin.x + 10, origin.y + wxHeight / 4);

	XY centerPoint = xyAdd(origin, XY{ (int)(wxWidth * sPos), (int)(wxHeight * (1.0f - vPos))});
	int xdist = 3;
	SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 255);
	for (int x = 0; x < 2; x++) {
		int fxdist = xdist + x;
		SDL_RenderDrawLine(g_rd, centerPoint.x - fxdist, centerPoint.y - fxdist, centerPoint.x - fxdist, centerPoint.y + fxdist);
		SDL_RenderDrawLine(g_rd, centerPoint.x + fxdist, centerPoint.y - fxdist, centerPoint.x + fxdist, centerPoint.y + fxdist);
	}
}

void UISVPicker::render(XY pos)
{
	SDL_Color vtx1 = rgb2sdlcolor(hsv2rgb(hsv{ parent->currentH, 0.0, 1.0 }));
	SDL_Color vtx2 = rgb2sdlcolor(hsv2rgb(hsv{ parent->currentH, 1.0, 1.0 }));
	SDL_Color vtx3 = rgb2sdlcolor(hsv2rgb(hsv{ parent->currentH, 0.0, 0.0 }));
	SDL_Color vtx4 = rgb2sdlcolor(hsv2rgb(hsv{ parent->currentH, 1.0, 0.0 }));
	vtx1.a = vtx2.a = vtx3.a = vtx4.a = 0xff;// parent->focused ? 0xff : 0x30;

	SDL_Vertex SVpicker[4];
	SVpicker[0].color = (vtx1);
	SVpicker[0].position = SDL_FPoint{ (float)(pos.x), (float)(pos.y) };
	SVpicker[1].color = (vtx2);
	SVpicker[1].position = SDL_FPoint{ (float)(pos.x + wxWidth), (float)(pos.y) };
	SVpicker[2].color = (vtx3);
	SVpicker[2].position = SDL_FPoint{ (float)(pos.x), (float)(pos.y + wxHeight) };
	SVpicker[3].color = (vtx4);
	SVpicker[3].position = SDL_FPoint{ (float)(pos.x + wxWidth), (float)(pos.y + wxHeight) };
	int idx[] = { 0,1,2,1,2,3 };
	SDL_RenderGeometry(g_rd, NULL, SVpicker, 4, idx, 6);
	drawPosIndicator(pos);
}

void UISVPicker::handleInput(SDL_Event evt, XY gPosOffset)
{
	switch (evt.type) {
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		if (evt.button.button == 1) {
			mouseHeld = evt.button.state;
		}
	case SDL_MOUSEMOTION:
		if (mouseHeld) {
			XY mousePos = xySubtract(XY{ evt.motion.x, evt.motion.y }, gPosOffset);
			sPos = fclamp(0.0f, mousePos.x / (float)wxWidth, 1.0f);
			vPos = 1.0f - fclamp(0.0f, mousePos.y / (float)wxHeight, 1.0f);
			this->onSVValueChanged();
		}
		break;
	}
}

void UISVPicker::onSVValueChanged()
{
	parent->editorColorSVPickerChanged(sPos, vPos);
}
