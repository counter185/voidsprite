#pragma once
#include "globals.h"

class BaseTemplate
{
public:
	virtual std::string getName() { return "Base template"; }
	virtual Layer* generate() { return NULL; }
	virtual XY tileSize() { return XY{ 0,0 }; }
	void drawPattern(Layer* layer, uint8_t* pattern, XY patternDimensions, XY position, uint32_t color);
};

