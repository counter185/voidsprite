#include "UILabel.h"
#include "FontRenderer.h"

void UILabel::render(XY pos)
{
	g_fnt->RenderString(text, pos.x, pos.y);
}
