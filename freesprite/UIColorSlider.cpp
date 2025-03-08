#include "UIColorSlider.h"

void UIColorSlider::render(XY pos)
{
	SDL_Rect drawrect = { pos.x, pos.y, wxWidth, wxHeight };

	SDL_Vertex gradientVerts[4];
	gradientVerts[0].position = { (float)pos.x, (float)pos.y };
	gradientVerts[0].color = toFColor(SDL_Color{ (uint8_t)((colorMin >> 16) & 0xff), (uint8_t)((colorMin >> 8) & 0xff), (uint8_t)((colorMin) & 0xff), 0xff });

	gradientVerts[1].position = { (float)pos.x + wxWidth, (float)pos.y };
	gradientVerts[1].color = toFColor(SDL_Color{ (uint8_t)((colorMax >> 16) & 0xff), (uint8_t)((colorMax >> 8) & 0xff), (uint8_t)((colorMax) & 0xff), 0xff });

	gradientVerts[2].position = { (float)pos.x + wxWidth, (float)pos.y + wxHeight};
	gradientVerts[2].color = toFColor(SDL_Color{ (uint8_t)((colorMax >> 16) & 0xff), (uint8_t)((colorMax >> 8) & 0xff), (uint8_t)((colorMax) & 0xff), 0xff });

	gradientVerts[3].position = { (float)pos.x, (float)pos.y + wxHeight };
	gradientVerts[3].color = toFColor(SDL_Color{ (uint8_t)((colorMin >> 16) & 0xff), (uint8_t)((colorMin >> 8) & 0xff), (uint8_t)((colorMin) & 0xff), 0xff });

	int indices[6] = { 0, 1, 2, 0, 2, 3 };
	SDL_RenderGeometry(g_rd, NULL, gradientVerts, 4, indices, 6);

	//SDL_SetRenderDrawColor(g_rd, 0x0, 0x0, 0x0, focused ? 0xff : 0x80);
	//SDL_RenderFillRect(g_rd, &drawrect);
	SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, focused ? 0x80 : 0x30);
	SDL_RenderDrawRect(g_rd, &drawrect);
	drawPosIndicator(pos);
}
