#include "EditorBrushPicker.h"
#include "maineditor.h"
#include "FontRenderer.h"

EditorBrushPicker::EditorBrushPicker(MainEditor* caller) {
	this->caller = caller;

	wxWidth = 190;
	wxHeight = 195;

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

	patternMenuPanel = new Panel();
	patternMenuPanel->enabled = false;
	patternMenuPanel->position = { 180, 0 };
	subWidgets.addDrawable(patternMenuPanel);

	UILabel* lbl = new UILabel();
	lbl->position = { 0, 5 };
	lbl->text = "PATTERNS";
	patternMenuPanel->subWidgets.addDrawable(lbl);

	patternMenu = new ScrollingPanel();
	patternMenu->scrollHorizontally = false;
	patternMenu->scrollVertically = true;
	patternMenu->wxWidth = 30 * 6 + 20;
	patternMenu->wxHeight = 130;
	patternMenu->position = { 0,30 };
	patternMenu->bgColor = { 0,0,0,0x80 };
	patternMenuPanel->subWidgets.addDrawable(patternMenu);

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
		newBtn->colorBorder = brush->overrideRightClick() ? SDL_Color{ 0x00,0xae,0xff,0x80 } : SDL_Color{ 0xff, 0xff, 0xff, 0x50 };
		newBtn->setCallbackListener(20 + i++, this);
		brushButtons.push_back(newBtn);
		subWidgets.addDrawable(newBtn);
	}
	updateActiveBrushButton(caller->currentBrush);

	px = 5;
	py = 0;
	i = 0;
	for (Pattern*& pattern : g_patterns) {
		UIButton* newBtn = new UIButton();
		if (px + 26 > wxWidth) {
			py += 26;
			px = 5;
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

void EditorBrushPicker::render(XY position)
{
    SDL_Rect r = SDL_Rect{ position.x, position.y, wxWidth, wxHeight };

    SDL_Color colorBG1 = { 0x30, 0x30, 0x30, focused ? 0xa0 : 0x90 };
    SDL_Color colorBG2 = { 0x10, 0x10, 0x10, focused ? 0xa0 : 0x90 };
    renderGradient(r, sdlcolorToUint32(colorBG2), sdlcolorToUint32(colorBG1), sdlcolorToUint32(colorBG1), sdlcolorToUint32(colorBG1));
    if (focused) {
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 255);
        drawLine({ position.x, position.y }, { position.x, position.y + wxHeight }, XM1PW3P1(focusTimer.percentElapsedTime(300)));
        drawLine({ position.x, position.y }, { position.x + wxWidth, position.y }, XM1PW3P1(focusTimer.percentElapsedTime(300)));
    }

    g_fnt->RenderString("TOOLS", position.x + 3, position.y + 1);

    patternMenuPanel->position = xyAdd({ 180, 0 }, XY{ (int)(30 * XM1PW3P1(patternMenuTimer.percentElapsedTime(200))) , 0 });

    DraggablePanel::render(position);
}

void EditorBrushPicker::eventButtonPressed(int evt_id)
{
    if (evt_id == EVENT_BRUSHPICKER_TOGGLE_PATTERN_MENU) {
        patternMenuPanel->enabled = !patternMenuPanel->enabled;
        patternMenuTimer.start();
        patternPanelBtn->text = patternMenu->enabled ? "<" : ">";
    }
    else if (evt_id == EVENT_MAINEDITOR_TOGGLEREPLACE) {
        caller->replaceAlphaMode = !caller->replaceAlphaMode;
        editorReplaceBtn->colorBGFocused = editorReplaceBtn->colorBGUnfocused = caller->replaceAlphaMode ? SDL_Color{ 0xff,0xff,0xff,0x40 } : SDL_Color{ 0,0,0,0xd0 };
    }
    else if (evt_id >= 60) {
        caller->currentPattern = g_patterns[evt_id - 60];
        patternMenu->subWidgets.forceUnfocus();
        updateActivePatternButton(caller->currentPattern);
    }
	else if (evt_id >= 20) {
		caller->currentBrush = g_brushes[evt_id-20];
        updateActiveBrushButton(evt_id - 20);
	}
}

void EditorBrushPicker::updateActiveBrushButton(int id)
{
    for (UIButton*& bbtn : brushButtons) {
        bbtn->colorBGFocused = bbtn->colorBGUnfocused = SDL_Color{ 0,0,0,0xd0 };
    }
    if (id >= 0) {
        brushButtons[id]->colorBGFocused = brushButtons[id]->colorBGUnfocused = SDL_Color{ 0xff,0xff,0xff,0x40 };
    }
}

void EditorBrushPicker::updateActiveBrushButton(BaseBrush* id)
{
    for (int x = 0; x < g_brushes.size(); x++) {
        if (g_brushes[x] == id) {
            updateActiveBrushButton(x);
            return;
        }
    }
    updateActiveBrushButton(-1);
}

void EditorBrushPicker::updateActivePatternButton(Pattern* p)
{
    for (int x = 0; x < patternButtons.size(); x++) {
        patternButtons[x]->colorBGFocused = patternButtons[x]->colorBGFocused = p == g_patterns[x] ? SDL_Color{ 0,0,0,0xd0 } : SDL_Color{ 0xff,0xff,0xff,0x40 };
    }
    patternPanelBtn->icon = p->cachedIcon;
}
