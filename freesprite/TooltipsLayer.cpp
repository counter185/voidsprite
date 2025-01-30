#include "TooltipsLayer.h"
#include "FontRenderer.h"
#include "mathops.h"

void TooltipsLayer::clearTooltips()
{
	tooltips.clear();
}

void TooltipsLayer::addTooltip(Tooltip t)
{
	tooltips.push_back(t);
}

void TooltipsLayer::renderAll()
{
	for (Tooltip& t : tooltips)
	{
		XY dim = g_fnt->StatStringDimensions(t.text);
		double timer = XM1PW3P1(t.timer);
		SDL_Rect rect = { t.position.x, t.position.y, dim.x + 8, (dim.y + 12) * timer };

		if ((rect.x + rect.w) > g_windowW)
		{
			rect.x = g_windowW - rect.w;
		}
		rect.x = ixmax(rect.x, 0);
		if ((rect.y + rect.h) > g_windowH)
		{
			rect.y = g_windowH - rect.h;
		}
		rect.y = ixmax(rect.y, 0);
		XY rectPos = { rect.x, rect.y };

		renderGradient(rect, gradientUL, gradientUR, gradientLL, gradientLR);
		
		if (border) {
			SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x90);
			drawLine(rectPos, xyAdd(rectPos, { rect.w, 0 }), timer);
			drawLine(rectPos, xyAdd(rectPos, { 0, rect.h }), timer);

			SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x40);
			drawLine(xyAdd(rectPos, { rect.w, rect.h }), xyAdd(rectPos, { rect.w, 0 }), timer);
			drawLine(xyAdd(rectPos, { rect.w, rect.h }), xyAdd(rectPos, { 0, rect.h }), timer);

			SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x10);
			drawLine(rectPos, xyAdd(rectPos, { rect.w, rect.h }), timer);
		}

		uint8_t alpha = (uint8_t)(t.textColor.a * timer);
		SDL_Color textCol = t.textColor;
		textCol.a = alpha;

		g_fnt->RenderString(t.text, rectPos.x + 4, rectPos.y + 2 - (20 * (1.0-timer)), textCol);
	}
	clearTooltips();
}
