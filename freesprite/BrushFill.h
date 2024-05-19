#pragma once
#include "BaseBrush.h"
class BrushFill : public BaseBrush
{
private:
	XY previewLastPosition;
	uint32_t previewSearchingColor;
	std::vector<XY> previewOpenList;
	std::vector<XY> previewClosedList;
	int previewIterations = 0;
	uint64_t timeStarted;
	uint64_t timeNextIter;

	bool closedListContains(XY a);
public:
	MainEditor* lastEditor = NULL;

	std::string getName() override { return "Fill"; }
	std::string getIconPath() override { return "assets/brush_fill.png"; }
	void clickPress(MainEditor* editor, XY pos) override;
	void clickDrag(MainEditor* editor, XY from, XY to) override;
	void clickRelease(MainEditor* editor, XY pos) override {}
	void renderOnCanvas(MainEditor* editor, int scale);
};

