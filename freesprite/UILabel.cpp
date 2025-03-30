#include "UILabel.h"
#include "FontRenderer.h"

XY UILabel::statSize() { 
    return g_fnt->StatStringDimensions(text, fontsize); 
}

void UILabel::render(XY pos)
{
    g_fnt->RenderString(text, pos.x, pos.y, color, fontsize);
}
