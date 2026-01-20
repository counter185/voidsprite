#include "EditorSpritesheetPreview.h"
#include "FontRenderer.h"
#include "maineditor.h"
#include "SpritesheetPreviewScreen.h"

void EditorSpritesheetPreview::renderAfterBG(XY at)
{
	XY tileSize = caller->caller->getPaddedTileDimensions();
	wxWidth = ixmax(80, tileSize.x * caller->canvas.scale) + 8;
	wxHeight = ixmax(30, tileSize.y * caller->canvas.scale) + 20 + 8;

	//XY origin = { g_windowW - wxWidth - 4, g_windowH - wxHeight - 4 - 40 };
	XY origin = { at.x,at.y };
	SDL_Rect drawRect = {
		origin.x,
		origin.y,
		wxWidth,
		wxHeight
	};
	//SDL_RenderFillRect(g_rd, &drawRect);
	caller->drawPreview(xyAdd(origin, XY{ 4, 24 }));
}
