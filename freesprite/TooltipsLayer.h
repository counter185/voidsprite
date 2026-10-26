#pragma once
#include "globals.h"

struct Tooltip {
    XY position = {0,0};
    std::string text = "Default tooltip text";
    SDL_Color textColor = { 0xff,0xff,0xff,0xff };
    double timer = 1.0;
};

class TooltipsLayer
{
private:
    std::vector<Tooltip> tooltips;
    void clearTooltips();
public:
    bool border = true;
    bool takeTooltips = true;
    Fill tooltipFill = Fill::None();

    TooltipsLayer() {
        static Fill ttpFill = visualConfigFill("tooltips/fill");
        tooltipFill = ttpFill;
    }

    void addTooltip(Tooltip t);
    void renderAll();
};

