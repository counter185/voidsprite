#pragma once
#include "BaseBrush.h"

class Brush1pxLine :
    public LineSelectBrush
{

    std::string getName() override { return TL("vsp.brush.line:v2"); };
    std::string getTooltip() override { return TL("vsp.brush.line.desc"); }
    std::string getIconPath() override { return "brush_1pxline.png"; }
    std::map<std::string, BrushProperty> getProperties() override
    {
        return {
            {"brush.pxline.round", BRUSH_BOOL_PROPERTY(TL("vsp.brush.param.round"),0)},
            {"brush.pxline.gradsize", BRUSH_BOOL_PROPERTY(TL("vsp.brush.param.gradsize"),0)},
            {"brush.pxline.size", BRUSH_INT_PROPERTY(TL("vsp.brush.param.size"),1,32,1)}
        };
    }
    XY getSection() override { return XY{ 0,2 }; }

    bool snapManuallyOnRelease() override { return true; }

    void renderOnCanvas(MainEditor* editor, int scale) override;

    void lineSelected(MainEditor* editor, XY start, XY end) override;
};

