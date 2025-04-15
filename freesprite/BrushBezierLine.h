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
    XY getSection() override { return XY{ 0,2 }; }

    void clickPress(MainEditor* editor, XY pos) override;
    void clickDrag(MainEditor* editor, XY from, XY to) override;
    void clickRelease(MainEditor* editor, XY pos) override;
    void rightClickPress(MainEditor* editor, XY pos) override;
    void renderOnCanvas(XY canvasDrawPoint, int scale) override;
    bool isReadOnly() override { return true; }
    bool overrideRightClick() override { return true; }
};

