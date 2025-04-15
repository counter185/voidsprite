#include "UILabel.h"
#include "FontRenderer.h"

XY UILabel::statSize() { 
    return g_fnt->StatStringDimensions(text, fontsize); 
}

XY UILabel::calcEndpoint()
{
    return g_fnt->StatStringEndpoint(text, position.x, position.y, fontsize);
}

void UILabel::render(XY pos)
{
    g_fnt->RenderString(text, pos.x, pos.y, color, fontsize);
}
