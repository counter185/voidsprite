#pragma once
#include "drawable.h"
class UILabel :
    public Drawable
{
public:
    std::string text = "";
    int fontsize = 18;
    SDL_Color color = { 255, 255, 255, 255 };
    
    UILabel() {}
    UILabel(std::string t) : text(t) {}

    XY statSize();

    bool focusable() override { return false; }
    void render(XY pos) override;
};

