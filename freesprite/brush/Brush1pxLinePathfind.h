#pragma once
#include "BaseBrush.h"
class Brush1pxLinePathfind : public LineSelectBrush
{
public:
    std::string getName() override { return TL("vsp.brush.pathfindline"); };
    std::string getTooltip() override { return TL("vsp.brush.pathfindline.desc"); }
    std::string getIconPath() override { return "brush_1pxlinepathfind.png"; }
    XY getSection() override { return XY{ 0,2 }; }

    void lineSelected(MainEditor* editor, XY from, XY to) override;
};

