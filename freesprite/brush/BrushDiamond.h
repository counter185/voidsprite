#pragma once
#include "BaseBrush.h"
class BrushDiamond :
    public BaseBrush
{
    XY startPos = XY{ 0,0 };
    bool heldDown = false;

    void resetState() override {
        startPos = XY{ 0,0 };
    }
    std::string getName() override { return TL("vsp.brush.diamond"); };
    std::string getIconPath() override { return "brush_1pxdiamond.png"; }
    std::map<std::string, BrushProperty> getProperties() override
    {
        return {
            {"brush.diamond.round", BRUSH_BOOL_PROPERTY(TL("vsp.brush.param.round"),0)},
            {"brush.diamond.size", BRUSH_INT_PROPERTY(TL("vsp.brush.param.size"),1,16,1)}
        };
    }
    XY getSection() override { return XY{ 1,2 }; }

    void clickPress(MainEditor* editor, XY pos) override;
    void clickRelease(MainEditor* editor, XY pos) override;
    void renderOnCanvas(MainEditor* editor, int scale) override;
};

