#pragma once
#include "BaseBrush.h"

class Brush1pxLine :
    public BaseBrush
{

    XY startPos = XY{ 0,0 };
    bool dragging = false;

    void resetState() override {
        startPos = XY{ 0,0 };
    }
    std::string getName() override { return TL("vsp.brush.line:v2"); };
    std::string getTooltip() override { return TL("vsp.brush.line.desc"); }
    std::string getIconPath() override { return "brush_1pxline.png"; }
    std::map<std::string, BrushProperty> getProperties() override
    {
        return {
            {"brush.pxline.round", BRUSH_BOOL_PROPERTY(TL("vsp.brush.param.round"),0)},
            {"brush.pxline.gradsize", BRUSH_BOOL_PROPERTY(TL("vsp.brush.param.gradsize"),0)},
            {"brush.pxline.size", BRUSH_INT_PROPERTY(TL("vsp.brush.param.size"),1,16,1)}
        };
    }
    XY getSection() override { return XY{ 0,2 }; }

    void clickPress(MainEditor* editor, XY pos) override;
    void clickDrag(MainEditor* editor, XY from, XY to) override {}
    void clickRelease(MainEditor* editor, XY pos) override;
    void renderOnCanvas(MainEditor* editor, int scale) override;
};

