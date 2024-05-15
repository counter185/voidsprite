#include "EditorSpritesheetPreview.h"
#include "FontRenderer.h"
#include "maineditor.h"

void EditorSpritesheetPreview::render(XY at)
{
	XY tileSize = caller->caller->tileDimensions;
	int wxWidth = ixmax(100, tileSize.x * caller->canvasZoom) + 8;
	int wxHeight = ixmax(30, tileSize.y * caller->canvasZoom) + 20 + 8;
	XY origin = { g_windowW - wxWidth - 4, g_windowH - wxHeight - 4 - 40 };
	SDL_Rect drawRect = {
		origin.x,
		origin.y,
		wxWidth,
		wxHeight
	};
	SDL_SetRenderDrawColor(g_rd, 0xd, 0xd, 0xd, 0xd0);
	SDL_RenderFillRect(g_rd, &drawRect);
	g_fnt->RenderString("Preview", origin.x+2, origin.y);
	caller->drawPreview(xyAdd(origin, XY{ 4, 24 }));
}
