#pragma once
#include "globals.h"

struct Tooltip {
	XY position = {0,0};
	std::string text = "Default tooltip text";
	SDL_Color textColor = { 0xff,0xff,0xff,0xff };
	double timer;
};

class TooltipsLayer
{
private:
	std::vector<Tooltip> tooltips;
	void clearTooltips();
public:
	void addTooltip(Tooltip t);
	void renderAll();
};

