#include "EditorBrushPicker.h"
#include "maineditor.h"
#include "FontRenderer.h"

bool EditorBrushPicker::isMouseIn(XY thisPositionOnScreen, XY mousePos)
{
	return pointInBox(mousePos, SDL_Rect{ thisPositionOnScreen.x, thisPositionOnScreen.y, wxWidth, wxHeight });
}

void EditorBrushPicker::render(XY position)
{
    //SDL_Color previewCol = rgb2sdlcolor(hsv2rgb(hsv{ currentH, currentS, currentV }));

    SDL_Rect r = SDL_Rect{ position.x, position.y, wxWidth, wxHeight };
    SDL_SetRenderDrawColor(g_rd, 0xcd, 0xcd, 0xcd, focused ? 0xaf : 0x30);
    SDL_RenderFillRect(g_rd, &r);

    /*r = SDL_Rect{position.x + wxWidth - 60, position.y + wxHeight - 40, 55, 35};
    SDL_SetRenderDrawColor(g_rd, previewCol.r, previewCol.g, previewCol.b, focused ? 0xff : 0x30);
    SDL_RenderFillRect(g_rd, &r);*/

    g_fnt->RenderString("BRUSHES", position.x + 1, position.y + 1);

    subWidgets.renderAll(position);
}

void EditorBrushPicker::handleInput(SDL_Event evt, XY gPosOffset)
{
    if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == 1 && evt.button.state) {
        subWidgets.tryFocusOnPoint(XY{ evt.button.x, evt.button.y }, position);
    }
    if (!subWidgets.anyFocused()) {

    }
    else {
        subWidgets.passInputToFocused(evt, gPosOffset);
    }
}

void EditorBrushPicker::eventButtonPressed(int evt_id)
{
	if (evt_id >= 10) {
		caller->currentBrush = g_brushes[evt_id-10];
	}
}
