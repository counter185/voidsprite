#include "globals.h"
#include "ToolSetXSymmetry.h"
#include "maineditor.h"
#include "FontRenderer.h"

void ToolSetXSymmetry::clickPress(MainEditor* editor, XY pos)
{
	editor->symmetryEnabled[0] = true;
	editor->symmetryPositions.x = pos.x;
	mouseHeld = true;
	lastEditor = editor;
}

void ToolSetXSymmetry::clickDrag(MainEditor* editor, XY from, XY to)
{
	editor->symmetryEnabled[0] = true;
	editor->symmetryPositions.x = to.x;
}

void ToolSetXSymmetry::clickRelease(MainEditor* editor, XY pos)
{
	mouseHeld = false;
}

void ToolSetXSymmetry::rightClickPress(MainEditor* editor, XY pos)
{
	editor->symmetryEnabled[0] = !editor->symmetryEnabled[0];
}

void ToolSetXSymmetry::renderOnCanvas(XY canvasDrawPoint, int scale)
{
	int symXPos = lastPos.x / 2;
	bool symXMiddle = lastPos.x % 2;
	int lineDrawXPoint = canvasDrawPoint.x + symXPos * scale + (symXMiddle ? scale / 2 : 0);

	SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x30);
	SDL_RenderDrawLine(g_rd, lineDrawXPoint, 0, lineDrawXPoint, g_windowH);
	if (mouseHeld) {
		g_fnt->RenderString(std::format("{}{}", symXPos, symXMiddle ? ".5" : ""), g_mouseX, g_mouseY + 20);
		if (lastEditor != NULL && lastEditor->tileDimensions.x != 0) {
			g_fnt->RenderString(std::format("(tile: {}{} / {})", symXPos % lastEditor->tileDimensions.x, symXMiddle ? ".5" : "", lastEditor->tileDimensions.x), g_mouseX, g_mouseY + 50);
		}
	}
}

void ToolSetXSymmetry::mouseMotion(MainEditor* editor, XY pos)
{
	lastPos = pos;
}
