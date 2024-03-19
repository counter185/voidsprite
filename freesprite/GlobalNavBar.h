#pragma once
#include "globals.h"
#include "drawable.h"
#include "mathops.h"
#include "maineditor.h"

class GlobalNavBar : public Drawable
{
public:
	MainEditor* parent;
	int wxHeight = 30;

	std::map<SDL_Keycode, NamedEditorOperation> keyBinds = {
		{SDLK_s, { "Save",
				[](MainEditor* editor) {
					editor->trySaveImage();
				}
			}
		},
		{SDLK_f, { "Flip current layer",
				[](MainEditor* editor) {
					Layer* lr = editor->imgLayer;
					lr->flipHorizontally();
				}
			}
		},
		{SDLK_r, { "Recenter canvas",
				[](MainEditor* editor) {
					editor->recenterCanvas();
				}
			}
		},
	};

	GlobalNavBar(MainEditor* caller) {
		parent = caller;
	}

	bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override
	{ 
		//todo: mousePos.y <= wxHeight OR `isMouseIn` for any of the submenus
		return mousePos.y <= wxHeight;
	}
	void render(XY position) override;
	void handleInput(SDL_Event evt, XY gPosOffset) override;
	void focusOut() override {
		Drawable::focusOut();
	}
};

