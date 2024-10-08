#include "BrushFill.h"
#include "maineditor.h"

bool BrushFill::closedListContains(XY a)
{
	for (int x = previewClosedList.size(); x --> 0;) {
		if (xyEqual(a, previewClosedList[x])) {
			return true;
		}
	}
	return false;
}

void BrushFill::clickPress(MainEditor* editor, XY pos)
{
	Layer* currentLayer = editor->getCurrentLayer();
	uint32_t pixel = currentLayer->getPixelAt(pos);
	uint32_t swapTo = editor->eraserMode ? (editor->isPalettized ? -1 : 0x00000000) : editor->getActiveColor();

	if (pixel == swapTo) {
		return;
	}

	std::vector<XY> openList;
	openList.push_back(pos);
	std::vector<XY> nextList;
	while (!openList.empty()) {
		for (XY& openListElement : openList) {
			uint32_t pixelRn = currentLayer->getPixelAt(openListElement);
			if (editor->isInBounds(openListElement) && (pixelRn == pixel || (!editor->isPalettized && pixelRn>>24 == 0 && pixel>>24 == 0))) {
				currentLayer->setPixel(openListElement, swapTo);
				XY p[] = {
					{0,1},
					{0,-1},
					{1,0},
					{-1,0}
				};
				for (XY& pp : p) {
					nextList.push_back(xyAdd(openListElement, pp));
				}
			}
		}
		openList = nextList;
		nextList.clear();
	}
}

void BrushFill::clickDrag(MainEditor* editor, XY from, XY to)
{
}

void BrushFill::renderOnCanvas(MainEditor* editor, int scale) {

	XY canvasDrawPoint = editor->canvasCenterPoint;
	if (editor != lastEditor || !xyEqual(lastMouseMotionPos, previewLastPosition)) {
		lastEditor = editor;
		previewLastPosition = lastMouseMotionPos;
		previewSearchingColor = editor->getCurrentLayer()->getPixelAt(previewLastPosition);

		timeStarted = SDL_GetTicks64();
		timeNextIter = timeStarted;
		previewIterations = 0;
		previewOpenList.clear();
		previewClosedList.clear();
		previewOpenList.push_back(previewLastPosition);
	}
	if (editor != NULL 
		&& editor->isInBounds(previewLastPosition) 
		&& previewClosedList.size() < 18000
		&& SDL_GetTicks64() > timeNextIter + 16) {
		timeNextIter = SDL_GetTicks64();
		std::vector<XY> nextList;
		for (XY& openListElement : previewOpenList) {
			uint32_t pixelRn = editor->getCurrentLayer()->getPixelAt(openListElement);
			if (editor->isInBounds(openListElement)
				&& (pixelRn == previewSearchingColor || (!editor->isPalettized && pixelRn >> 24 == 0 && previewSearchingColor >> 24 == 0))
				&& !closedListContains(openListElement)) {
				XY p[] = {
					{0,1},
					{0,-1},
					{1,0},
					{-1,0}
				};
				for (XY& pp : p) {
					nextList.push_back(xyAdd(openListElement, pp));
				}
				previewClosedList.push_back(openListElement);
			}
		}
		previewOpenList = nextList;
		previewIterations++;
	}
	for (XY& cle : previewClosedList) {
		uint64_t distanceTicks = SDL_GetTicks64() - timeStarted;
		//double alphaModifier = dxmin((distanceTicks / 32.0) / dxmax(xyDistance(cle, previewLastPosition), 1), 1);
		double alphaModifier = xyDistance(cle, previewLastPosition) / dxmax(1, distanceTicks / 16.0);
		uint8_t alpha = 0xd0 * dxmax(0, dxmin(alphaModifier, 1));

		SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, alpha);
		drawLocalPoint(canvasDrawPoint, cle, scale);
		SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x40);
		drawPointStrikethrough(canvasDrawPoint, cle, scale);
	}

	SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
	drawLocalPoint(canvasDrawPoint, lastMouseMotionPos, scale);
	SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
	drawPointOutline(canvasDrawPoint, lastMouseMotionPos, scale);
}
