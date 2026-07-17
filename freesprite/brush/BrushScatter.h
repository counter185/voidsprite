#pragma once
#include "BaseBrush.h"

class BrushScatter :
    public BaseBrush
{
    std::string getName() override { return TL("vsp.brush.scatter"); }
    std::string getTooltip() override { return TL("vsp.brush.scatter.desc"); }
    std::string getIconPath() override { return "brush_scatter.png"; }
    std::map<std::string, BrushProperty> getProperties() override
    {
        return {
            {"brush.scatter.round", BRUSH_BOOL_PROPERTY(TL("vsp.brush.param.round"),0)},
            {"brush.scatter.pressuresens", BRUSH_BOOL_PROPERTY(TL("vsp.brush.param.pressuresize"),0)},
            {"brush.scatter.size", BRUSH_INT_PROPERTY(TL("vsp.brush.param.size"),2,20,4)},
            {"brush.scatter.move1px", BRUSH_BOOL_PROPERTY("Move by 1 pixel",0)},
        };
    }
    void clickPress(MainEditor* editor, XY pos) override;
    void clickDrag(MainEditor* editor, XY from, XY to) override;
    void clickRelease(MainEditor* editor, XY pos) override {}
    void renderOnCanvas(MainEditor* editor, int scale) override;

    void doScatter(MainEditor* editor, XY pos, int size, bool round);
};

