#pragma once
#include "globals.h"

class BaseTemplate
{
public:
	virtual std::string getName() { return "Base template"; }
	virtual Layer* generate() { return NULL; }
	virtual XY tileSize() { return XY{ 0,0 }; }
};

