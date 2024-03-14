#include "globals.h"
#include "FontRenderer.h"
#include "maineditor.h"
#include "EditorColorPicker.h"
#include "mathops.h"

bool EditorColorPicker::isMouseIn(XY thisPositionOnScreen, XY mousePos)
{
    return pointInBox(mousePos, SDL_Rect{ thisPositionOnScreen.x, thisPositionOnScreen.y, wxWidth, wxHeight });
}

void EditorColorPicker::render(XY position)
{
    SDL_Rect r = SDL_Rect{ position.x, position.y, wxWidth, wxHeight };
    SDL_SetRenderDrawColor(g_rd, 0x26, 0x26, 0x26, focused ? 0xff : 0x30);
    SDL_RenderFillRect(g_rd, &r);

    g_fnt->RenderString("COLOR PICKER", position.x + 1, position.y + 1);

    subWidgets.renderAll(position);
}

void EditorColorPicker::handleInput(SDL_Event evt, XY gPosOffset)
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

void EditorColorPicker::updateMainEditorColor()
{
    rgb col = hsv2rgb(hsv{ currentH, currentS, currentV });
    SDL_Color col_b = rgb2sdlcolor(col);
    caller->pickedColor = (0xFF << 24) + (col_b.r << 16) + (col_b.g << 8) + col_b.b;   
}
