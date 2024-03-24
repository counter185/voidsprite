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

    XY tabOrigin = xyAdd(position, tbv->position);
    tabOrigin.y += tbv->buttonsHeight;
    switch (tbv->openTab) {
        case 1:
            g_fnt->RenderString(std::format("H {}", std::round(currentH * 1000.0f) / 1000.0f), tabOrigin.x, tabOrigin.y + 20);
            g_fnt->RenderString(std::format("S {}", std::round(currentS * 1000.0f) / 1000.0f), tabOrigin.x, tabOrigin.y + 70);
            g_fnt->RenderString(std::format("V {}", std::round(currentV * 1000.0f) / 1000.0f), tabOrigin.x, tabOrigin.y + 120);
            break;
    }

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

void EditorColorPicker::eventTextInputConfirm(int evt_id, std::string data)
{
    if (evt_id == EVENT_COLORPICKER_TEXTFIELD) {
        if (g_colors.contains(data)) {
            setMainEditorColorRGB(g_colors[data]);
        }
    }
}

void EditorColorPicker::eventButtonPressed(int evt_id)
{
    if (evt_id == EVENT_COLORPICKER_TOGGLEERASER) {
        toggleEraser();
    }
}

void EditorColorPicker::eventSliderPosChanged(int evt_id, float f)
{
    switch (evt_id) {
        case EVENT_COLORPICKER_SLIDERH:
            currentH = f * 360.0f;
            updateMainEditorColor();
            break;
        case EVENT_COLORPICKER_SLIDERS:
            currentS = f;
            updateMainEditorColor();
            break;
        case EVENT_COLORPICKER_SLIDERV:
            currentV = f;
            updateMainEditorColor();
            break;
    }
}

void EditorColorPicker::toggleEraser()
{
    caller->eraserMode = !caller->eraserMode;
    eraserButton->colorBGFocused = eraserButton->colorBGUnfocused = caller->eraserMode ? SDL_Color{ 0xff,0xff,0xff, 0x30 } : SDL_Color{ 0,0,0, 0x80 };
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
    hsv a = rgb2hsv(rgb{ col.r / 255.0f, col.g / 255.0f, col.b / 255.0f });
    if (tbv->openTab != 1 || updateHSVSliders) {
        sliderH->sliderPos = a.h / 360.0f;
        sliderS->sliderPos = a.s;
        sliderV->sliderPos = a.v;
    }
    if (tbv->openTab != 0 || updateHSVSliders) {
        hueSlider->sliderPos = a.h / 360.0;
        satValSlider->sPos = a.s;
        satValSlider->vPos = a.v;
    }
    if (updateHSVSliders) {
        currentH = a.h;
        currentS = a.s;
        currentV = a.v;
    }

    colorTextField->text = std::format("#{:02X}{:02X}{:02X}", col.r, col.g, col.b);
    caller->pickedColor = (0xFF << 24) + (col.r << 16) + (col.g << 8) + col.b;
}