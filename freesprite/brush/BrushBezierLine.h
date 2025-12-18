#pragma once
#include "BaseBrush.h"
class BrushBezierLine :
    public BaseBrush
{
    std::vector<XY> points;
    int dragging = -1;

    void resetState() override {
        dragging = -1;
        points.clear();
    }
    std::string getName() override { return "Bezier Line"; };
    std::string getTooltip() override { return "Place and move control points with Mouse Left.\nDraw line with Mouse Right."; }
    std::string getIconPath() override { return "brush_bezierline.png"; }
    std::map<std::string, BrushProperty> getProperties() override
    {
        return {
            {"brush.bezierline.round", BRUSH_BOOL_PROPERTY(TL("vsp.brush.param.round"),0)},
            {"brush.bezierline.size", BRUSH_INT_PROPERTY(TL("vsp.brush.param.size"),1,16,1)}
        };
    }
    XY getSection() override { return XY{ 0,2 }; }

    void clickPress(MainEditor* editor, XY pos) override;
    void clickDrag(MainEditor* editor, XY from, XY to) override;
    void clickRelease(MainEditor* editor, XY pos) override;
    void rightClickPress(MainEditor* editor, XY pos) override;
    void renderOnCanvas(MainEditor* editor, int scale) override;
    bool isReadOnly() override { return true; }
    bool overrideRightClick() override { return true; }
};

