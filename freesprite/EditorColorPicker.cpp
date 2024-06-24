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
        case 2:
            g_fnt->RenderString(std::format("R {}", currentR), tabOrigin.x, tabOrigin.y + 20);
            g_fnt->RenderString(std::format("G {}", currentG), tabOrigin.x, tabOrigin.y + 70);
            g_fnt->RenderString(std::format("B {}", currentB), tabOrigin.x, tabOrigin.y + 120);
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
        if (data.size() == 6 && colorTextField->isValidOrPartialColor()) {
            data = "#" + data;
        }
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
        if (data == "rand" || data == "random") {
            setMainEditorColorRGB((0xFF<<24) + ((rand() % 256) << 16) + ((rand() % 256) << 8) + (rand() % 256));
        }
        else if (g_colors.contains(data)) {
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
        case EVENT_COLORPICKER_SLIDERR:
            currentR = f * 255;
            updateMainEditorColorFromRGBSliders();
            break;
        case EVENT_COLORPICKER_SLIDERG:
            currentG = f * 255;
            updateMainEditorColorFromRGBSliders();
            break;
        case EVENT_COLORPICKER_SLIDERB:
            currentB = f * 255;
            updateMainEditorColorFromRGBSliders();
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
    setMainEditorColorRGB(col_b, false, true);
}

void EditorColorPicker::setMainEditorColorHSV(double h, double s, double v)
{
    currentH = h;
    currentS = s;
    currentV = v;
    rgb col = hsv2rgb(hsv{ currentH, currentS, currentV });
    SDL_Color col_b = rgb2sdlcolor(col);
    setMainEditorColorRGB(col_b, true, true);
}

void EditorColorPicker::updateMainEditorColorFromRGBSliders()
{
    setMainEditorColorRGB({currentR, currentG, currentB}, true, false);
}

void EditorColorPicker::setMainEditorColorRGB(unsigned int col) {
    setMainEditorColorRGB(SDL_Color{ (uint8_t)((col >> 16) & 0xff), (uint8_t)((col >> 8) & 0xff), (uint8_t)(col & 0xff) });
}
void EditorColorPicker::setMainEditorColorRGB(SDL_Color col, bool updateHSVSliders, bool updateRGBSliders) {
    hsv a = rgb2hsv(rgb{ col.r / 255.0f, col.g / 255.0f, col.b / 255.0f });
    if (tbv->openTab != 1 || updateHSVSliders) {
        sliderH->sliderPos = (float)(a.h / 360.0f);
        sliderS->sliderPos = (float)a.s;
        sliderV->sliderPos = (float)a.v;
    }
    if (tbv->openTab != 0 || updateHSVSliders) {
        hueSlider->sliderPos = (float)(a.h / 360.0);
        satValSlider->sPos = (float)a.s;
        satValSlider->vPos = (float)a.v;
    }
    if (updateHSVSliders) {
        currentH = (float)a.h;
        currentS = (float)a.s;
        currentV = (float)a.v;
    }
    currentR = col.r;
    currentG = col.g;
    currentB = col.b;
    if (updateRGBSliders) {
		sliderR->sliderPos = (float)(col.r / 255.0f);
		sliderG->sliderPos = (float)(col.g / 255.0f);
		sliderB->sliderPos = (float)(col.b / 255.0f);
	}

    rgb colorSMin = hsv2rgb(hsv{ currentH, 0, currentV });
    rgb colorSMax = hsv2rgb(hsv{ currentH, 1, currentV });
    rgb colorVMin = hsv2rgb(hsv{ currentH, currentS, 0 });
    rgb colorVMax = hsv2rgb(hsv{ currentH, currentS, 1 });
    sliderS->colorMin = (0xFF << 24) + ((int)(colorSMin.r*255) << 16) + ((int)(colorSMin.g * 255) << 8) + (int)(colorSMin.b * 255);
    sliderS->colorMax = (0xFF << 24) + ((int)(colorSMax.r*255) << 16) + ((int)(colorSMax.g * 255) << 8) + (int)(colorSMax.b * 255);
    sliderV->colorMin = (0xFF << 24) + ((int)(colorVMin.r*255) << 16) + ((int)(colorVMin.g * 255) << 8) + (int)(colorVMin.b * 255);
    sliderV->colorMax = (0xFF << 24) + ((int)(colorVMax.r*255) << 16) + ((int)(colorVMax.g * 255) << 8) + (int)(colorVMax.b * 255);


    uint32_t rgbColor = (0xFF << 24) + (col.r << 16) + (col.g << 8) + col.b;

    sliderR->colorMin = rgbColor & 0x00FFFF;
    sliderR->colorMax = rgbColor | 0xFF0000;
    sliderG->colorMin = rgbColor & 0xFF00FF;
    sliderG->colorMax = rgbColor | 0x00FF00;
    sliderB->colorMin = rgbColor & 0xFFFF00;
    sliderB->colorMax = rgbColor | 0x0000FF;

    colorTextField->text = std::format("#{:02X}{:02X}{:02X}", col.r, col.g, col.b);
    caller->pickedColor = rgbColor;
}