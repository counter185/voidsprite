#include "EditorBrushPicker.h"
#include "maineditor.h"
#include "FontRenderer.h"

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

    patternMenu->position = xyAdd({ 180, 0 }, XY{ (int)(30 * XM1PW3P1(patternMenuTimer.percentElapsedTime(200))) , 0 });

    DraggablePanel::render(position);
}

void EditorBrushPicker::eventButtonPressed(int evt_id)
{
    if (evt_id == EVENT_BRUSHPICKER_TOGGLE_PATTERN_MENU) {
        patternMenu->enabled = !patternMenu->enabled;
        patternMenuTimer.start();
        patternPanelBtn->text = patternMenu->enabled ? "<" : ">";
    }
    else if (evt_id == EVENT_MAINEDITOR_TOGGLEREPLACE) {
        caller->replaceAlphaMode = !caller->replaceAlphaMode;
        editorReplaceBtn->colorBGFocused = editorReplaceBtn->colorBGUnfocused = caller->replaceAlphaMode ? SDL_Color{ 0xff,0xff,0xff,0x40 } : SDL_Color{ 0,0,0,0xd0 };
    }
    else if (evt_id >= 60) {
        caller->currentPattern = g_patterns[evt_id - 60];
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
    brushButtons[id]->colorBGFocused = brushButtons[id]->colorBGUnfocused = SDL_Color{ 0xff,0xff,0xff,0x40 };
}

void EditorBrushPicker::updateActivePatternButton(Pattern* p)
{
    for (int x = 0; x < patternButtons.size(); x++) {
        patternButtons[x]->colorBGFocused = patternButtons[x]->colorBGFocused = p == g_patterns[x] ? SDL_Color{ 0,0,0,0xd0 } : SDL_Color{ 0xff,0xff,0xff,0x40 };
    }
    patternPanelBtn->icon = p->cachedIcon;
}
