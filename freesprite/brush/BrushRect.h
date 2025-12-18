#pragma once
#include "BaseBrush.h"
class BrushRect :
    public BaseBrush
{
    XY startPos = XY{ 0,0 };
    bool heldDown = false;
    XY lastMousePos = XY{ 0,0 };

    void resetState() override {
        startPos = XY{ 0,0 };
    }
    std::string getName() override { return TL("vsp.brush.rect:v2"); };
    std::string getIconPath() override { return "brush_1pxrect.png"; }
    std::map<std::string, BrushProperty> getProperties() override
    {
        return {
            {"brush.pxrect.round", BRUSH_BOOL_PROPERTY(TL("vsp.brush.param.round"),0)},
            {"brush.pxrect.size", BRUSH_INT_PROPERTY(TL("vsp.brush.param.size"),1,16,1)}
        };
    }
    XY getSection() override { return XY{ 0,2 }; }

    void clickPress(MainEditor* editor, XY pos) override;
    void clickDrag(MainEditor* editor, XY from, XY to) override { lastMousePos = to; }
    void clickRelease(MainEditor* editor, XY pos) override;
    void renderOnCanvas(MainEditor* editor, int scale) override;
};

