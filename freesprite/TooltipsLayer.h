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
	bool border = true;
	uint32_t gradientUL = 0xFF000000,
		gradientUR = 0xFF000000,
		gradientLL = 0xD0000000,
		gradientLR = 0xD0000000;

	void addTooltip(Tooltip t);
	void renderAll();
};

