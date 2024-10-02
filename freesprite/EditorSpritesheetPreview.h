#pragma once
#include "drawable.h"
#include "globals.h"
#include "DraggablePanel.h"

class EditorSpritesheetPreview : public DraggablePanel
{
private:
	SpritesheetPreviewScreen* caller;
public:

	EditorSpritesheetPreview(SpritesheetPreviewScreen* parent) {
		caller = parent;

		position = { 430, 60 };
	}

	void render(XY at) override;
};

