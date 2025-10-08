#pragma once
#include "globals.h"
#include "Panel.h"

class UIColorInputField :
    public Panel
{
protected:
	UIButton* button = NULL;
	u32 pickedColor;
	bool allowAlpha = false;
public:
	std::function<void(UIColorInputField*, u32)> onColorChangedCallback = NULL;

	UIColorInputField(bool alpha = false);

	u32 getColor() { return pickedColor; }
	void setColor(u32 c);
};

