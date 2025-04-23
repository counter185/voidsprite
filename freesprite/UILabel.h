#pragma once
#include "drawable.h"
class UILabel :
    public Drawable
{
protected:
    std::string text = "";

    XY cachedSize = {-1,-1};
    bool cachedSizeValid = false;
public:
    int fontsize = 18;
    SDL_Color color = { 255, 255, 255, 255 };
    
    UILabel() {}
    UILabel(std::string t) : text(t) {}

    XY statSize();
    XY calcEndpoint();

    std::string getText() { return text; }
    void setText(std::string newtext);

    XY getDimensions() override { return statSize(); };
    bool focusable() override { return false; }
    void render(XY pos) override;
};

