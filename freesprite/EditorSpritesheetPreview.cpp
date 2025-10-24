#include "EditorSpritesheetPreview.h"
#include "FontRenderer.h"
#include "maineditor.h"
#include "SpritesheetPreviewScreen.h"

void EditorSpritesheetPreview::render(XY at)
{
	XY tileSize = caller->caller->tileDimensions;
	wxWidth = ixmax(80, tileSize.x * caller->canvas.scale) + 8;
	wxHeight = ixmax(30, tileSize.y * caller->canvas.scale) + 20 + 8;
	borderColor = visualConfigHexU32("ui/panel/border");

	//XY origin = { g_windowW - wxWidth - 4, g_windowH - wxHeight - 4 - 40 };
	XY origin = { at.x,at.y };
	SDL_Rect drawRect = {
		origin.x,
		origin.y,
		wxWidth,
		wxHeight
	};
	SDL_SetRenderDrawColor(g_rd, 0xd, 0xd, 0xd, 0xd0);
	renderGradient(drawRect, 0x200d0d0d, 0x800d0d0d, 0x800d0d0d, 0xe0000000);
	//SDL_RenderFillRect(g_rd, &drawRect);
	g_fnt->RenderString("PREVIEW", origin.x+5, origin.y+1);
	caller->drawPreview(xyAdd(origin, XY{ 4, 24 }));
}
