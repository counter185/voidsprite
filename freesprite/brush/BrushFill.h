#pragma once
#include "BaseBrush.h"

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

    std::string getName() override { return TL("vsp.brush.fill"); }
    std::string getTooltip() override { return TL("vsp.brush.fill.desc"); }
    std::string getIconPath() override { return "brush_fill.png"; }
    XY getSection() override { return XY{ 0,1 }; }

    void resetState() override;;
    void clickPress(MainEditor* editor, XY pos) override;
    void clickDrag(MainEditor* editor, XY from, XY to) override;
    void clickRelease(MainEditor* editor, XY pos) override {}
    void rightClickPress(MainEditor* editor, XY pos) override;
    void renderOnCanvas(MainEditor* editor, int scale) override;
    bool overrideRightClick() override { return true; }
};

class BrushRaycastFill : public BrushFill 
{
public:
    std::string getName() override { return TL("vsp.brush.raycastfill"); }
    std::string getTooltip() override { return TL("vsp.brush.raycastfill.desc"); }
    std::string getIconPath() override { return "brush_raycastfill.png"; }
    bool overrideRightClick() override { return false; }
    std::map<std::string, BrushProperty> getProperties() override
    {
        return {
            {"brush.rtfill.precision", BRUSH_INT_PROPERTY(TL("vsp.brush.param.precision"),1,30,8)}
        };
    }

    void clickPress(MainEditor* editor, XY pos) override;
};