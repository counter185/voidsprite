#pragma once
#include "globals.h"

class BaseScreen
{
public:
	virtual ~BaseScreen() {}

	virtual void render() {}
	virtual void takeInput(SDL_Event evt) {}
	virtual void tick() {}
};

