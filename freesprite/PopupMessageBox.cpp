#include "FontRenderer.h"
#include "PopupMessageBox.h"

void PopupMessageBox::render()
{
	renderDefaultBackground();

	XY titlePos = getDefaultTitlePosition();
	XY contentPos = getDefaultContentPosition();

	g_fnt->RenderString(title, titlePos.x, titlePos.y);
	g_fnt->RenderString(text, contentPos.x, contentPos.y);

	renderDrawables();
}
