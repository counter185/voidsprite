#include "UIColorSlider.h"

void UIColorSlider::render(XY pos)
{
	SDL_Rect drawrect = { pos.x, pos.y, wxWidth, wxHeight };

	if (colors.size() > 1) {
		int splits = ixmax(colors.size(), 2) - 1;
		double wSplit = wxWidth / splits;

		for (int x = 0; x < splits; x++) {
			SDL_Rect subRect = drawrect;
			subRect.w = (x == splits - 1) ? wxWidth - ((splits-1)*wSplit) : wSplit;
			subRect.x = pos.x + x * wSplit;
			u32 color1 = colors[x] | 0xFF000000;
			u32 color2 = colors[x + 1] | 0xFF000000;
			renderGradient(subRect, color1, color2, color1, color2);

			//debug subgradients
			//SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 120);
			//SDL_RenderDrawRect(g_rd, &subRect);
		}
	}
	else if (colors.size() == 1) {
		SDL_SetRenderDrawColor(g_rd, (colors[0] >> 16) & 0xff, (colors[0] >> 8) & 0xff, colors[0] & 0xff, 0xff);
		SDL_RenderFillRect(g_rd, &drawrect);
	}
	else {
		SDL_SetRenderDrawColor(g_rd, 0x0, 0x0, 0x0, 0x80);
		SDL_RenderFillRect(g_rd, &drawrect);
	}

	/* SDL_Vertex gradientVerts[4];
	gradientVerts[0].position = { (float)pos.x, (float)pos.y };
	gradientVerts[0].color = toFColor(SDL_Color{ (uint8_t)((colorMin >> 16) & 0xff), (uint8_t)((colorMin >> 8) & 0xff), (uint8_t)((colorMin) & 0xff), 0xff });

	gradientVerts[1].position = { (float)pos.x + wxWidth, (float)pos.y };
	gradientVerts[1].color = toFColor(SDL_Color{ (uint8_t)((colorMax >> 16) & 0xff), (uint8_t)((colorMax >> 8) & 0xff), (uint8_t)((colorMax) & 0xff), 0xff });

	gradientVerts[2].position = { (float)pos.x + wxWidth, (float)pos.y + wxHeight};
	gradientVerts[2].color = toFColor(SDL_Color{ (uint8_t)((colorMax >> 16) & 0xff), (uint8_t)((colorMax >> 8) & 0xff), (uint8_t)((colorMax) & 0xff), 0xff });

	gradientVerts[3].position = { (float)pos.x, (float)pos.y + wxHeight };
	gradientVerts[3].color = toFColor(SDL_Color{ (uint8_t)((colorMin >> 16) & 0xff), (uint8_t)((colorMin >> 8) & 0xff), (uint8_t)((colorMin) & 0xff), 0xff });

	int indices[6] = { 0, 1, 2, 0, 2, 3 };
	SDL_RenderGeometry(g_rd, NULL, gradientVerts, 4, indices, 6);*/

	//SDL_SetRenderDrawColor(g_rd, 0x0, 0x0, 0x0, focused ? 0xff : 0x80);
	//SDL_RenderFillRect(g_rd, &drawrect);
	SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
	SDL_RenderDrawRect(g_rd, &drawrect);
	drawPosIndicator(pos);
}
