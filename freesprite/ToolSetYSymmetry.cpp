#include "ToolSetYSymmetry.h"
#include "globals.h"
#include "maineditor.h"
#include "FontRenderer.h"

void ToolSetYSymmetry::clickPress(MainEditor* editor, XY pos)
{
	editor->symmetryEnabled[1] = true;
	editor->symmetryPositions.y = pos.y;
	mouseHeld = true;
}

void ToolSetYSymmetry::clickDrag(MainEditor* editor, XY from, XY to)
{
	editor->symmetryEnabled[1] = true;
	editor->symmetryPositions.y = to.y;
}

void ToolSetYSymmetry::clickRelease(MainEditor* editor, XY pos)
{
	mouseHeld = false;
}

void ToolSetYSymmetry::rightClickPress(MainEditor* editor, XY pos)
{
	editor->symmetryEnabled[1] = !editor->symmetryEnabled[1];
}

void ToolSetYSymmetry::renderOnCanvas(XY canvasDrawPoint, int scale)
{
	int symYPos = lastPos.y / 2;
	bool symYMiddle = lastPos.y % 2;
	int lineDrawYPoint = canvasDrawPoint.y + symYPos * scale + (symYMiddle ? scale / 2 : 0);

	SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x20);
	SDL_RenderDrawLine(g_rd, 0, lineDrawYPoint, g_windowW, lineDrawYPoint);
	if (mouseHeld) {
		g_fnt->RenderString(std::format("{}{}", symYPos, symYMiddle ? ".5" : ""), g_mouseX, g_mouseY + 20);
	}
}

void ToolSetYSymmetry::mouseMotion(MainEditor* editor, XY pos)
{
	lastPos = pos;
}
