#pragma once
#include "globals.h"
#include "maineditor.h"

class BaseTemplate
{
public:
	virtual std::string getName() { return "Base template"; }
	virtual std::string getTooltip() { return ""; }
	virtual Layer* generate() { return NULL; }
	virtual XY tileSize() { return XY{ 0,0 }; }
	void drawPattern(Layer* layer, uint8_t* pattern, XY patternDimensions, XY position, uint32_t color);
	virtual std::vector<CommentData> placeComments() { return {}; }
};

