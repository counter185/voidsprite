#include "BrushReplaceColor.h"
#include "maineditor.h"

void BrushReplaceColor::clickPress(MainEditor* editor, XY pos)
{
	editor->layer_replaceColor(editor->layer_getPixelAt(pos), editor->eraserMode ? (editor->isPalettized ? -1 : 0x00000000) : editor->getActiveColor());
}

void BrushReplaceColor::renderOnCanvas(MainEditor* editor, int scale)
{
	SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
	drawLocalPoint(editor->canvasCenterPoint, lastMouseMotionPos, scale);
	SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
	drawPointOutline(editor->canvasCenterPoint, lastMouseMotionPos, scale);
}
