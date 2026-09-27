#pragma once
#include "BaseBrush.h"

class BrushGradient : public LineSelectBrush
{
protected:
	void lineSelected(MainEditor* editor, XY start, XY end) override;
public:
    std::string getName() override { return TL("vsp.brush.gradient"); };
    std::string getTooltip() override { return TL("vsp.brush.gradient.desc"); }
    std::string getIconPath() override { return "brush_gradient.png"; }
    XY getSection() override { return XY{ 0,1 }; }
};

