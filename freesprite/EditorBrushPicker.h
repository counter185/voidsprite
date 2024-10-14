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
#include "ScrollingPanel.h"

class EditorBrushPicker : public DraggablePanel, public EventCallbackListener
{
public:
	MainEditor* caller;

	Timer64 patternMenuTimer;
	UIButton* patternPanelBtn;
	UIButton* editorReplaceBtn;
	Panel* patternMenuPanel;
	ScrollingPanel* patternMenu;
	std::vector<UIButton*> brushButtons;
	std::vector<UIButton*> patternButtons;

	EditorBrushPicker(MainEditor* caller);

	void render(XY position) override;

	void eventButtonPressed(int evt_id) override;

	void updateActiveBrushButton(int id);
	void updateActiveBrushButton(BaseBrush* id);
	void updateActivePatternButton(Pattern* p);
};