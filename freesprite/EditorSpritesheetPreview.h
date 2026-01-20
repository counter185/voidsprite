#pragma once
#include "drawable.h"
#include "globals.h"
#include "PanelUserInteractable.h"

class EditorSpritesheetPreview : public PanelUserInteractable
{
private:
	SpritesheetPreviewScreen* caller;
public:

	EditorSpritesheetPreview(SpritesheetPreviewScreen* parent) {
		caller = parent;

		position = { 430, 60 };
		setupDraggable();
		setupCollapsible();
		addTitleText("PREVIEW");
	}

	void renderAfterBG(XY at) override;
};

