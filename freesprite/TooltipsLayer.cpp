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
		renderGradient(rect, 0xFF000000, 0xFF000000, 0xD0000000, 0xD0000000);
		SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x90);
		drawLine(t.position, xyAdd(t.position, { rect.w, 0 }), timer);
		drawLine(t.position, xyAdd(t.position, { 0, rect.h }), timer);

		SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x40);
		drawLine(xyAdd(t.position, {rect.w, rect.h}), xyAdd(t.position, { rect.w, 0 }), timer);
		drawLine(xyAdd(t.position, { rect.w, rect.h }), xyAdd(t.position, { 0, rect.h }), timer);

		SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x10);
		drawLine(t.position, xyAdd(t.position, { rect.w, rect.h }), timer);

		g_fnt->RenderString(t.text, t.position.x + 4, t.position.y + 2, t.textColor);
	}
	clearTooltips();
}
