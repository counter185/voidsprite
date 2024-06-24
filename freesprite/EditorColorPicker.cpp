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
    SDL_Color devalColor = rgb2sdlcolor(hsv2rgb(hsv{ currentH, currentS, dxmax(currentV / 6, 0.1) }));
    SDL_Color devalColor2 = rgb2sdlcolor(hsv2rgb(hsv{ currentH, currentS, dxmax(currentV / 18, 0.05) }));
    devalColor.a = devalColor2.a = focused ? 0xaf : 0x30;
    renderGradient(r, sdlcolorToUint32(devalColor2), sdlcolorToUint32(devalColor), sdlcolorToUint32(devalColor), sdlcolorToUint32(devalColor));
    //SDL_SetRenderDrawColor(g_rd, previewCol.r/6, previewCol.g / 6, previewCol.b / 6, focused ? 0xaf : 0x30);
    //SDL_RenderFillRect(g_rd, &r);

    SDL_Color valCol = rgb2sdlcolor(hsv2rgb(hsv{ currentH, currentS, dxmin(currentV + 0.4, 1.0) }));
    if (focused) {
        SDL_SetRenderDrawColor(g_rd, valCol.r, valCol.g, valCol.b, 255);
        drawLine({ position.x, position.y }, { position.x, position.y + wxHeight }, XM1PW3P1(focusTimer.percentElapsedTime(300)));
        drawLine({ position.x, position.y }, { position.x + wxWidth, position.y  }, XM1PW3P1(focusTimer.percentElapsedTime(300)));
    }

    XY tabOrigin = xyAdd(position, colorTabs->position);
    tabOrigin.y += colorTabs->buttonsHeight;
    switch (colorTabs->openTab) {
        case 1:
            labelH->text = std::format("H {}", std::round(currentH * 1000.0f) / 1000.0f);
            labelS->text = std::format("S {}", std::round(currentS * 1000.0f) / 1000.0f);
            labelV->text = std::format("V {}", std::round(currentV * 1000.0f) / 1000.0f);
            break;
        case 2:
            labelR->text = std::format("R {}", currentR);
            labelG->text = std::format("G {}", currentG);
            labelB->text = std::format("B {}", currentB);
            break;
    }

    r = SDL_Rect{ position.x + wxWidth - 60, position.y + wxHeight - 40, 55, 35 };
    SDL_SetRenderDrawColor(g_rd, previewCol.r, previewCol.g, previewCol.b, focused ? 0xff : 0x30);
    SDL_RenderFillRect(g_rd, &r);
    SDL_SetRenderDrawColor(g_rd, valCol.r, valCol.g, valCol.b, focused ? 0xff : 0x30);
    SDL_RenderDrawRect(g_rd, &r);

    g_fnt->RenderString("COLOR PICKER", position.x + 5, position.y + 1);

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
	else if (evt_id >= 200) {
		uint32_t col = lastColors[evt_id - 200];
		setMainEditorColorRGB(col);
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
    if (colorTabs->openTab != 1 || updateHSVSliders) {
        sliderH->sliderPos = (float)(a.h / 360.0f);
        sliderS->sliderPos = (float)a.s;
        sliderV->sliderPos = (float)a.v;
    }
    if (colorTabs->openTab != 0 || updateHSVSliders) {
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

void EditorColorPicker::pushLastColor(uint32_t col)
{
    col |= 0xff000000;
    auto fnd = std::find(lastColors.begin(), lastColors.end(), col);

    if (fnd == lastColors.begin() && lastColors.size() > 0) {
        return;
    }

    //printf("pushing new color!\n");
    lastColorsChanged = true;

    if (fnd != lastColors.end()) {
		lastColors.erase(fnd);
	}
    lastColors.insert(lastColors.begin(), col);

    while (lastColors.size() > 256) {
        lastColors.pop_back();
    }
    updateLastColorButtons();
}

void EditorColorPicker::updateLastColorButtons()
{
    if (!lastColorsChanged) {
        return;
    }
    colorModeTabs->tabs[1].wxs.freeAllDrawables();
    int x = 0;
    int xx = 0;
    int y = 0;
    int posX = 0;
    int posY = 5;
    for (uint32_t& col : lastColors) {
        UIButton* colBtn = new UIButton();
        colBtn->colorBGFocused = colBtn->colorBGUnfocused = SDL_Color{(uint8_t)((col >> 16) & 0xff), (uint8_t)((col >> 8) & 0xff), (uint8_t)(col & 0xff), 255};
        colBtn->position = { posX, posY };
        colBtn->wxHeight = 24;
        colBtn->wxWidth = 30;
        colBtn->setCallbackListener(200+(xx++), this);
        colorModeTabs->tabs[1].wxs.addDrawable(colBtn);

        posX += 30;

        if (++x == 12) {
            posX = 0;
            posY += 24;
            x = 0;
            if (y++ == 10) {
                break;
            }
        }
    }
    lastColorsChanged = false;
}