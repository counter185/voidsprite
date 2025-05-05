#include "UILabel.h"
#include "FontRenderer.h"

XY UILabel::statSize() { 
    if (!cachedSizeValid) {
        cachedSize = g_fnt->StatStringDimensions(text, fontsize);
        cachedSizeValid = true;
    }
    return cachedSize;
}

XY UILabel::calcEndpoint()
{
    return g_fnt->StatStringEndpoint(text, position.x, position.y, fontsize);
}

void UILabel::setText(std::string newtext)
{
    text = newtext;
    cachedSizeValid = false;
}

void UILabel::render(XY pos)
{
    g_fnt->RenderString(text, pos.x, pos.y, color, fontsize);
}
