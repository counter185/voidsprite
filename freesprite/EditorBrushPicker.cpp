#include "EditorBrushPicker.h"
#include "maineditor.h"
#include "FontRenderer.h"

bool EditorBrushPicker::isMouseIn(XY thisPositionOnScreen, XY mousePos)
{
	return pointInBox(mousePos, SDL_Rect{ thisPositionOnScreen.x, thisPositionOnScreen.y, wxWidth, wxHeight })
        || (patternsMenuOpen && patternMenuWidgets.mouseInAny(thisPositionOnScreen, mousePos));
}

void EditorBrushPicker::render(XY position)
{
    //SDL_Color previewCol = rgb2sdlcolor(hsv2rgb(hsv{ currentH, currentS, currentV }));

    SDL_Rect r = SDL_Rect{ position.x, position.y, wxWidth, wxHeight };
    //SDL_SetRenderDrawColor(g_rd, 0x30, 0x30, 0x30, focused ? 0x80 : 0x30);
    //SDL_RenderFillRect(g_rd, &r);
    SDL_Color colorBG1 = { 0x30, 0x30, 0x30, focused ? 0xa0 : 0x90 };
    SDL_Color colorBG2 = { 0x10, 0x10, 0x10, focused ? 0xa0 : 0x90 };
    renderGradient(r, sdlcolorToUint32(colorBG2), sdlcolorToUint32(colorBG1), sdlcolorToUint32(colorBG1), sdlcolorToUint32(colorBG1));
    if (focused) {
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 255);
        drawLine({ position.x, position.y }, { position.x, position.y + wxHeight }, XM1PW3P1(focusTimer.percentElapsedTime(300)));
        drawLine({ position.x, position.y }, { position.x + wxWidth, position.y }, XM1PW3P1(focusTimer.percentElapsedTime(300)));
    }

    /*r = SDL_Rect{position.x + wxWidth - 60, position.y + wxHeight - 40, 55, 35};
    SDL_SetRenderDrawColor(g_rd, previewCol.r, previewCol.g, previewCol.b, focused ? 0xff : 0x30);
    SDL_RenderFillRect(g_rd, &r);*/

    g_fnt->RenderString("TOOLS", position.x + 3, position.y + 1);

    subWidgets.renderAll(position);

    if (patternsMenuOpen) {
        patternMenuWidgets.renderAll(xySubtract(position, XY{(int)(30 * (1.0- XM1PW3P1(patternMenuTimer.percentElapsedTime(200)))) , 0}));
    }
}

void EditorBrushPicker::handleInput(SDL_Event evt, XY gPosOffset)
{
    if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == 1 && evt.button.state) {
        if (!patternMenuWidgets.tryFocusOnPoint(XY{ evt.button.x, evt.button.y }, position)) {
            subWidgets.tryFocusOnPoint(XY{ evt.button.x, evt.button.y }, position);
        }
    }
    if (patternMenuWidgets.anyFocused()) {
        patternMenuWidgets.passInputToFocused(evt, gPosOffset);
    }
    else if (subWidgets.anyFocused()) {
        subWidgets.passInputToFocused(evt, gPosOffset);
    }
    else {
        
    }
}

void EditorBrushPicker::eventButtonPressed(int evt_id)
{
    if (evt_id == EVENT_BRUSHPICKER_TOGGLE_PATTERN_MENU) {
        patternsMenuOpen = !patternsMenuOpen;
        patternMenuTimer.start();
        patternPanelBtn->text = patternsMenuOpen ? "<" : ">";
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
