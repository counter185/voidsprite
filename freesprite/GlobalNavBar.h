#pragma once
#include "globals.h"
#include "drawable.h"
#include "mathops.h"
#include "maineditor.h"
#include "FileIO.h"
#include "PopupMessageBox.h"
#include "PopupSetEditorPixelGrid.h"

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
#if _DEBUG
					{SDLK_a, { "DebugSaveTest",
							[](MainEditor* editor) {
								Layer* flat = editor->flattenImage();
								writePNG(L"a.png", flat);
								delete flat;
								g_addPopup(new PopupMessageBox("Save", "saved :)"));
							}
						}
					},
#endif
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
								Layer* lr = editor->getCurrentLayer();
								lr->flipHorizontally();
							}
						}
					},
					{SDLK_b, { "Swap channels RGB->BGR",
							[](MainEditor* editor) {
								Layer* clayer = editor->getCurrentLayer();
								uint8_t* convData = (uint8_t*)malloc(clayer->w * clayer->h * 4);
								SDL_ConvertPixels(clayer->w, clayer->h, SDL_PIXELFORMAT_ARGB8888, clayer->pixelData, clayer->w * 4, SDL_PIXELFORMAT_ABGR8888, convData, clayer->w * 4);
								free(clayer->pixelData);
								clayer->pixelData = convData;
								clayer->layerDirty = true;
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
								editor->backgroundColor.r = ~editor->backgroundColor.r;
								editor->backgroundColor.g = ~editor->backgroundColor.g;
								editor->backgroundColor.b = ~editor->backgroundColor.b;
							}
						}
					},
					{SDLK_g, { "Set pixel grid...",
							[](MainEditor* editor) {
								g_addPopup(new PopupSetEditorPixelGrid(editor, "Set pixel grid", "Enter grid size <w>x<h>:"));
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

