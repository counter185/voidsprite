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
	DrawableManager wxs;
	DrawableManager subWxs;
	SDL_Keycode currentSubmenuOpen = -1;

	std::map<SDL_Keycode, NavbarSection> keyBinds = 
	{ 
		{
			SDLK_f,
			{
				"File",
				{
					{SDLK_d, { "Save as",
							[](MainEditor* editor) {
								//todo: implement save as
							}
						}
					},
					{SDLK_s, { "Save",
							[](MainEditor* editor) {
								editor->trySaveImage();
							}
						}
					},
				}
			}
		},
		{
			SDLK_l,
			{
				"Layer",
				{
					{SDLK_f, { "Flip current layer: X axis",
							[](MainEditor* editor) {
								Layer* lr = editor->imgLayer;
								lr->flipHorizontally();
							}
						}
					},
				}
			}
		},
		{
			SDLK_v,
			{
				"View",
				{
					{SDLK_r, { "Recenter canvas",
							[](MainEditor* editor) {
								editor->recenterCanvas();
							}
						}
					},
					{SDLK_b, { "Toggle background color",
							[](MainEditor* editor) {
								//todo: implement this
							}
						}
					},
				}
			}
		}
	};

	GlobalNavBar(MainEditor* caller) {
		parent = caller;
		int x = 10;
		int xDist = 120;
		position = XY{ 0,0 };
		for (auto& editorSection : keyBinds) {
			UIButton* sectionButton = new UIButton();
			sectionButton->position = { x, 1 };
			sectionButton->text = editorSection.second.name + std::format("({})", SDL_GetKeyName(editorSection.first));
			sectionButton->colorBGFocused = sectionButton->colorBGUnfocused = SDL_Color{ 0,0,0,0 };
			sectionButton->colorTextFocused = sectionButton->colorTextUnfocused = SDL_Color{ 255,255,255,0xa0 };
			wxs.addDrawable(sectionButton);
			x += xDist;
		}
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
		currentSubmenuOpen = -1;
		updateCurrentSubmenu();
	}

	void tryPressHotkey(SDL_Keycode k);
	void openSubmenu(SDL_Keycode which);
	void updateCurrentSubmenu();
};

