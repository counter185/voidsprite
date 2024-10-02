#pragma once
#include "drawable.h"
#include "DrawableManager.h"
#include "DraggablePanel.h"
#include "BaseBrush.h"
#include "UIButton.h"
#include "EventCallbackListener.h"
#include "Pattern.h"
#include "UILabel.h"
#include "Panel.h"

class EditorBrushPicker : public DraggablePanel, public EventCallbackListener
{
public:
	MainEditor* caller;

	Timer64 patternMenuTimer;
	UIButton* patternPanelBtn;
	UIButton* editorReplaceBtn;
	Panel* patternMenu;
	std::vector<UIButton*> brushButtons;
	std::vector<UIButton*> patternButtons;

	EditorBrushPicker(MainEditor* caller) {
		this->caller = caller;

		wxWidth = 190;
		wxHeight = 165;

		patternPanelBtn = new UIButton();
		patternPanelBtn->position = { 141, 10 };
		patternPanelBtn->text = ">";
		patternPanelBtn->wxWidth = 40;
		patternPanelBtn->wxHeight = 24;
		patternPanelBtn->tooltip = "Open pattern menu";
		patternPanelBtn->icon = g_patterns[0]->cachedIcon;
		patternPanelBtn->setCallbackListener(EVENT_BRUSHPICKER_TOGGLE_PATTERN_MENU, this);
		subWidgets.addDrawable(patternPanelBtn);

		editorReplaceBtn = new UIButton();
		editorReplaceBtn->position = { 110, 10 };
		editorReplaceBtn->text = "R";
		editorReplaceBtn->wxWidth = 24;
		editorReplaceBtn->wxHeight = 24;
		editorReplaceBtn->tooltip = "Replace mode\nPixels will only be drawn on opaque areas";
		editorReplaceBtn->setCallbackListener(EVENT_MAINEDITOR_TOGGLEREPLACE, this);
		subWidgets.addDrawable(editorReplaceBtn);

		patternMenu = new Panel();
		patternMenu->enabled = false;
		patternMenu->position = { 180, 0 };
		subWidgets.addDrawable(patternMenu);

		UILabel* lbl = new UILabel();
		lbl->position = { 0, 5 };
		lbl->text = "PATTERNS";
		patternMenu->subWidgets.addDrawable(lbl);

		int px = 5;
		int py = 40;
		int i = 0;
		for (BaseBrush*& brush : g_brushes) {
			UIButton* newBtn = new UIButton();
			if (px + 26 > wxWidth) {
				py += 30;
				px = 5;
			}
			newBtn->position = XY{ px, py };
			px += 30;
			newBtn->icon = brush->cachedIcon;
			newBtn->tooltip = brush->getName() + (brush->getTooltip() != "" ? ("\n" + brush->getTooltip()) : "");
			//newBtn->text = brush->getName();
			newBtn->wxWidth = 26;
			newBtn->wxHeight = 26;
			newBtn->setCallbackListener(20 + i++, this);
			brushButtons.push_back(newBtn);
			subWidgets.addDrawable(newBtn);
		}
		
		px = 0;
		py = 30;
		i = 0;
		for (Pattern*& pattern : g_patterns) {
			UIButton* newBtn = new UIButton();
			if (px + 26 > wxWidth) {
				py += 26;
				px = 0;
			}
			newBtn->position = XY{ px, py };
			px += 26;
			newBtn->icon = pattern->cachedIcon;
			newBtn->tooltip = pattern->getName();
			//newBtn->text = brush->getName();
			newBtn->wxWidth = 24;
			newBtn->wxHeight = 24;
			newBtn->setCallbackListener(60 + i++, this);
			patternButtons.push_back(newBtn);
			patternMenu->subWidgets.addDrawable(newBtn);
		}
	}

	void render(XY position) override;

	void eventButtonPressed(int evt_id) override;

	void updateActiveBrushButton(int id);
	void updateActivePatternButton(Pattern* p);
};