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
    SDL_Color previewCol = rgb2sdlcolor(hsv2rgb(hsv{ currentH, currentS, currentV }));

    SDL_Rect r = SDL_Rect{ position.x, position.y, wxWidth, wxHeight };
    SDL_SetRenderDrawColor(g_rd, previewCol.r/6, previewCol.g / 6, previewCol.b / 6, focused ? 0xaf : 0x30);
    SDL_RenderFillRect(g_rd, &r);

    r = SDL_Rect{ position.x + wxWidth - 60, position.y + wxHeight - 40, 55, 35 };
    SDL_SetRenderDrawColor(g_rd, previewCol.r, previewCol.g, previewCol.b, focused ? 0xff : 0x30);
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

void EditorColorPicker::eventTextInput(int evt_id, std::string data)
{
    if (evt_id == EVENT_COLORPICKER_TEXTFIELD) {
        //do something with the text i guess
        if (data.size() == 7) {
            unsigned int col;
            if (tryRgbStringToColor(data.substr(1), &col)) {
                col |= 0xff000000;
                setMainEditorColorRGB(col);
            }
        }
    }
}

void EditorColorPicker::updateMainEditorColor()
{
    rgb col = hsv2rgb(hsv{ currentH, currentS, currentV });
    SDL_Color col_b = rgb2sdlcolor(col);
    setMainEditorColorRGB(col_b, false);
}

void EditorColorPicker::setMainEditorColorRGB(unsigned int col) {
    setMainEditorColorRGB(SDL_Color{ (uint8_t)((col >> 16) & 0xff), (uint8_t)((col >> 8) & 0xff), (uint8_t)(col & 0xff) });
}
void EditorColorPicker::setMainEditorColorRGB(SDL_Color col, bool updateHSVSliders) {
    if (updateHSVSliders) {
        hsv a = rgb2hsv(rgb{ col.r / 255.0f, col.g / 255.0f, col.b / 255.0f });
        hueSlider->sliderPos = a.h / 360.0;
        satValSlider->sPos = a.s;
        satValSlider->vPos = a.v;
        currentH = a.h;
        currentS = a.s;
        currentV = a.v;
        //todo clean this shit up
    }
    colorTextField->text = std::format("#{:02X}{:02X}{:02X}", col.r, col.g, col.b);
    caller->pickedColor = (0xFF << 24) + (col.r << 16) + (col.g << 8) + col.b;
}