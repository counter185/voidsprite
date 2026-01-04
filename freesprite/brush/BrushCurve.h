#pragma once
#include "BaseBrush.h"

enum BrushCurveState : int {
    BRUSHCURVE_START = 0,
    BRUSHCURVE_PLACEP1 = 1,
    BRUSHCURVE_PLACEP2 = 2
};

class BrushCurve :
    public BaseBrush
{

    BrushCurveState state = BRUSHCURVE_START;

    XY startPos = XY{ 0,0 };
    XY endPos = XY{ 0,0 };
    XY p1Pos = XY{ 0,0 };
    XY p2Pos = XY{ 0,0 };
    bool dragging = false;

    void resetState() override {
        state = BRUSHCURVE_START;
        startPos = endPos = p1Pos = p2Pos = XY{ 0,0 };
    }
    std::string getName() override { return TL("vsp.brush.curve"); };
    std::string getTooltip() override { return TL("vsp.brush.curve.desc"); }
    std::string getIconPath() override { return "brush_curve.png"; }
    std::map<std::string, BrushProperty> getProperties() override
    {
        return {
            {"brush.curve.round", BRUSH_BOOL_PROPERTY(TL("vsp.brush.param.round"),0)},
            {"brush.curve.gradsize", BRUSH_BOOL_PROPERTY(TL("vsp.brush.param.gradsize"),0)},
            {"brush.curve.size", BRUSH_INT_PROPERTY(TL("vsp.brush.param.size"),1,16,1)}
        };
    }
    XY getSection() override { return XY{ 0,2 }; }
    bool isReadOnly() override { return state != BRUSHCURVE_PLACEP2; } 

    void clickPress(MainEditor* editor, XY pos) override;
    void clickRelease(MainEditor* editor, XY pos) override;
    void renderOnCanvas(MainEditor* editor, int scale) override;

    void putLine(MainEditor* editor);
};