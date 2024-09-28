#include "ToolSetYSymmetry.h"
#include "globals.h"
#include "maineditor.h"
#include "FontRenderer.h"
#include "TooltipsLayer.h"

void ToolSetYSymmetry::clickPress(MainEditor* editor, XY pos)
{
	editor->symmetryEnabled[1] = true;
	editor->symmetryPositions.y = pos.y;
	mouseHeld = true;
	lastEditor = editor;
	clickTimer.start();
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
		std::string tooltipString = std::format("{}{}", symYPos, symYMiddle ? ".5" : "");
		if (lastEditor != NULL && lastEditor->tileDimensions.y != 0) {
			tooltipString += std::format("\n(tile: {}{} / {})", symYPos % lastEditor->tileDimensions.y, symYMiddle ? ".5" : "", lastEditor->tileDimensions.y);
		}
		g_ttp->addTooltip({
			g_mouseX, g_mouseY + 20,
			tooltipString,
			{ 0xff,0xff,0xff,0xff },
			clickTimer.percentElapsedTime(200)
		});
	}
}

void ToolSetYSymmetry::mouseMotion(MainEditor* editor, XY pos)
{
	lastPos = pos;
}
