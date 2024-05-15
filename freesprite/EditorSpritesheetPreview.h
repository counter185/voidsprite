#pragma once
#include "drawable.h"
#include "globals.h"

class EditorSpritesheetPreview : public Drawable
{
private:
	SpritesheetPreviewScreen* caller;
public:

	EditorSpritesheetPreview(SpritesheetPreviewScreen* parent) {
		caller = parent;
	}

	bool focusable() override { return false; }
	void render(XY at) override;
};

