#include "globals.h"
#include "FontRenderer.h"
#include "PopupSetColorKey.h"
#include "maineditor.h"

void PopupSetColorKey::render()
{
	renderDefaultBackground();

	XY titlePos = getDefaultTitlePosition();
	XY contentPos = getDefaultContentPosition();

	g_fnt->RenderString(title, titlePos.x, titlePos.y);
	g_fnt->RenderString(text, contentPos.x, contentPos.y);

	renderDrawables();
}

void PopupSetColorKey::eventButtonPressed(int evt_id)
{
	if (evt_id == 0) {
		target->colorKey = colorInput->pickedColor;
		target->colorKeySet = true;
		target->layerDirty = true;

	}
	g_closePopup(this);
}
