#pragma once
#include "BaseBrush.h"
#include "mathops.h"

class BrushFill : public BaseBrush
{
private:
	XY previewLastPosition;
	uint32_t previewSearchingColor;
	std::vector<XY> previewOpenList;
	ScanlineMap previewClosedList;
	IntLineMap previewXBList;
	//std::vector<XY> previewClosedList;
	int previewIterations = 0;
	uint64_t timeStarted;
	uint64_t timeNextIter;

	bool closedListContains(XY a);
public:
	MainEditor* lastEditor = NULL;

	std::string getName() override { return "Fill"; }
	std::string getTooltip() override { return "Mouse Left to fill an area with the current color.\nMouse Right to fill only the currently selected area."; }
	std::string getIconPath() override { return "brush_fill.png"; }
	XY getSection() override { return XY{ 0,1 }; }

	void resetState() override;;
	void clickPress(MainEditor* editor, XY pos) override;
	void clickDrag(MainEditor* editor, XY from, XY to) override;
	void clickRelease(MainEditor* editor, XY pos) override {}
	void rightClickPress(MainEditor* editor, XY pos) override;
	void renderOnCanvas(MainEditor* editor, int scale);
	bool overrideRightClick() { return true; }
};

