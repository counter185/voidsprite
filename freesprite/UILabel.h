#pragma once
#include "drawable.h"
class UILabel :
    public Drawable
{
public:
	std::string text = "";

	bool focusable() override { return false; }
	void render(XY pos) override;
};

