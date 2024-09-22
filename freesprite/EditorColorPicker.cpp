#include "globals.h"
#include "FontRenderer.h"
#include "maineditor.h"
#include "EditorColorPicker.h"
#include "mathops.h"

EditorColorPicker::EditorColorPicker(MainEditor* c) {
    caller = c;

    wxWidth = 400;
    wxHeight = 390;

    colorModeTabs = new TabbedView({ { "Colors" },{ "Last" } }, 75);
    colorModeTabs->position = XY{ 20,30 };
    subWidgets.addDrawable(colorModeTabs);

    colorTabs = new TabbedView({ { "Visual", g_iconColorVisual },{ "HSV", g_iconColorHSV },{ "RGB", g_iconColorRGB } }, 90);
    colorTabs->position = XY{ 0,5 };
    colorModeTabs->tabs[0].wxs.addDrawable(colorTabs);

    hueSlider = new UIHueSlider(this);
    hueSlider->position = XY{ 0,10 };
    colorTabs->tabs[0].wxs.addDrawable(hueSlider);

    satValSlider = new UISVPicker(this);
    satValSlider->position = XY{ 0,40 };
    colorTabs->tabs[0].wxs.addDrawable(satValSlider);

    sliderH = new UISlider();
    sliderH->position = XY{ 30, 10 };
    sliderH->wxWidth = 200;
    sliderH->setCallbackListener(EVENT_COLORPICKER_SLIDERH, this);
    colorTabs->tabs[1].wxs.addDrawable(sliderH);

    UILabel* labelH = new UILabel();
    UILabel* labelR = new UILabel();
    labelR->text = "R";
    labelH->text = "H";
    labelH->position = labelR->position = XY{ 0, 20 };
    colorTabs->tabs[1].wxs.addDrawable(labelH);
    colorTabs->tabs[2].wxs.addDrawable(labelR);

    UILabel* labelS = new UILabel();
    UILabel* labelG = new UILabel();
    labelG->text = "G";
    labelS->text = "S";
    labelS->position = labelG->position = XY{ 0, 70 };
    colorTabs->tabs[1].wxs.addDrawable(labelS);
    colorTabs->tabs[2].wxs.addDrawable(labelG);

    UILabel* labelV = new UILabel();
    UILabel* labelB = new UILabel();
    labelB->text = "B";
    labelV->text = "V";
    labelV->position = labelB->position = XY{ 0, 120 };
    colorTabs->tabs[1].wxs.addDrawable(labelV);
    colorTabs->tabs[2].wxs.addDrawable(labelB);

    txtR = new UITextField();
    txtG = new UITextField();
    txtB = new UITextField();
    txtH = new UITextField();
    txtS = new UITextField();
    txtV = new UITextField();
    txtR->position = txtH->position = XY{ 240, 15 };
    txtR->textColor = { 0xff,0,0,0xff };
    txtG->position = txtS->position = XY{ 240, 65 };
    txtG->textColor = { 0,0xff,0,0xff };
    txtB->position = txtV->position = XY{ 240, 115 };
    txtB->textColor = { 0,0,0xff,0xff };
    txtR->wxWidth = txtG->wxWidth = txtB->wxWidth = 60;
    txtH->wxWidth = txtS->wxWidth = txtV->wxWidth = 120;
    txtR->setCallbackListener(EVENT_COLORPICKER_TBOXR, this);
    txtG->setCallbackListener(EVENT_COLORPICKER_TBOXG, this);
    txtB->setCallbackListener(EVENT_COLORPICKER_TBOXB, this);
    txtH->setCallbackListener(EVENT_COLORPICKER_TBOXH, this);
    txtS->setCallbackListener(EVENT_COLORPICKER_TBOXS, this);
    txtV->setCallbackListener(EVENT_COLORPICKER_TBOXV, this);
    colorTabs->tabs[2].wxs.addDrawable(txtR);
    colorTabs->tabs[2].wxs.addDrawable(txtG);
    colorTabs->tabs[2].wxs.addDrawable(txtB);
    colorTabs->tabs[1].wxs.addDrawable(txtH);
    colorTabs->tabs[1].wxs.addDrawable(txtS);
    colorTabs->tabs[1].wxs.addDrawable(txtV);

    sliderS = new UIColorSlider();
    sliderS->position = XY{ 30, 60 };
    sliderS->wxWidth = 200;
    sliderS->setCallbackListener(EVENT_COLORPICKER_SLIDERS, this);
    colorTabs->tabs[1].wxs.addDrawable(sliderS);

    sliderV = new UIColorSlider();
    sliderV->position = XY{ 30, 110 };
    sliderV->wxWidth = 200;
    sliderV->setCallbackListener(EVENT_COLORPICKER_SLIDERV, this);
    colorTabs->tabs[1].wxs.addDrawable(sliderV);

    sliderR = new UIColorSlider();
    sliderR->position = XY{ 30, 10 };
    sliderR->wxWidth = 200;
    sliderR->setCallbackListener(EVENT_COLORPICKER_SLIDERR, this);
    colorTabs->tabs[2].wxs.addDrawable(sliderR);

    sliderG = new UIColorSlider();
    sliderG->position = XY{ 30, 60 };
    sliderG->wxWidth = 200;
    sliderG->setCallbackListener(EVENT_COLORPICKER_SLIDERG, this);
    colorTabs->tabs[2].wxs.addDrawable(sliderG);

    sliderB = new UIColorSlider();
    sliderB->position = XY{ 30, 110 };
    sliderB->wxWidth = 200;
    sliderB->setCallbackListener(EVENT_COLORPICKER_SLIDERB, this);
    colorTabs->tabs[2].wxs.addDrawable(sliderB);

    colorTextField = new UITextField();
    colorTextField->isColorField = true;
    colorTextField->position = XY{ 60, 350 };
    colorTextField->wxWidth = 140;
    colorTextField->setCallbackListener(EVENT_COLORPICKER_TEXTFIELD, this);
    subWidgets.addDrawable(colorTextField);

    eraserButton = new UIButton();
    eraserButton->position = { 20, 350 };
    //eraserButton->text = "E";
    eraserButton->icon = g_iconEraser;
    eraserButton->wxWidth = 30;
    eraserButton->setCallbackListener(EVENT_COLORPICKER_TOGGLEERASER, this);
    subWidgets.addDrawable(eraserButton);

    blendModeButton = new UIButton();
    blendModeButton->position = { 215, 350 };
    blendModeButton->icon = g_iconBlendMode;
    blendModeButton->wxWidth = 30;
    blendModeButton->setCallbackListener(EVENT_COLORPICKER_TOGGLEBLENDMODE, this);
    blendModeButton->colorBGFocused = blendModeButton->colorBGUnfocused = caller->blendAlphaMode ? SDL_Color{ 0xff,0xff,0xff, 0x30 } : SDL_Color{ 0,0,0, 0x80 };
    subWidgets.addDrawable(blendModeButton);
}

void EditorColorPicker::render(XY position)
{
    SDL_Color previewCol = rgb2sdlcolor(hsv2rgb(hsv{ currentH, currentS, currentV }));

    SDL_Rect r = SDL_Rect{ position.x, position.y, wxWidth, wxHeight };
    SDL_Color devalColor = rgb2sdlcolor(hsv2rgb(hsv{ currentH, currentS, dxmax(currentV / 6, 0.1) }));
    SDL_Color devalColor2 = rgb2sdlcolor(hsv2rgb(hsv{ currentH, currentS, dxmax(currentV / 18, 0.05) }));
    devalColor.a = devalColor2.a = focused ? 0xaf : 0x90;
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

    r = SDL_Rect{ position.x + wxWidth - 60, position.y + wxHeight - 40, 55, 35 };
    SDL_SetRenderDrawColor(g_rd, previewCol.r, previewCol.g, previewCol.b, 0xff);
    SDL_RenderFillRect(g_rd, &r);
    SDL_SetRenderDrawColor(g_rd, valCol.r, valCol.g, valCol.b, 0xff);
    SDL_RenderDrawRect(g_rd, &r);

    g_fnt->RenderString("COLOR PICKER", position.x + 5, position.y + 1);

    DraggablePanel::render(position);
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
    else if (evt_id == EVENT_COLORPICKER_TBOXR) {
        updateRGBTextBoxOnInputEvent(data, &currentR);
    }
    else if (evt_id == EVENT_COLORPICKER_TBOXG) {
        updateRGBTextBoxOnInputEvent(data, &currentG);
    }
    else if (evt_id == EVENT_COLORPICKER_TBOXB) {
        updateRGBTextBoxOnInputEvent(data, &currentB);
    }
    else if (evt_id == EVENT_COLORPICKER_TBOXH) {
        updateHSVTextBoxOnInputEvent(data, &currentH);
    }
    else if (evt_id == EVENT_COLORPICKER_TBOXS) {
        updateHSVTextBoxOnInputEvent(data, &currentS);
    }
    else if (evt_id == EVENT_COLORPICKER_TBOXV) {
        updateHSVTextBoxOnInputEvent(data, &currentV);
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
    else if (evt_id == EVENT_COLORPICKER_TOGGLEBLENDMODE) {
        toggleAlphaBlendMode();
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

void EditorColorPicker::toggleAlphaBlendMode()
{
    caller->blendAlphaMode = !caller->blendAlphaMode;
    blendModeButton->colorBGFocused = blendModeButton->colorBGUnfocused = caller->blendAlphaMode ? SDL_Color{ 0xff,0xff,0xff, 0x30 } : SDL_Color{ 0,0,0, 0x80 };
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

void EditorColorPicker::updateMainEditorColorFromRGBTextBoxes()
{
    setMainEditorColorRGB({currentR, currentG, currentB}, true, true);
}

void EditorColorPicker::updateMainEditorColorFromHSVTextBoxes()
{
    rgb col = hsv2rgb(hsv{ currentH, currentS, currentV });
    SDL_Color col_b = rgb2sdlcolor(col);
    setMainEditorColorRGB(col_b, true, true, false);
}

void EditorColorPicker::setMainEditorColorRGB(unsigned int col) {
    setMainEditorColorRGB(SDL_Color{ (uint8_t)((col >> 16) & 0xff), (uint8_t)((col >> 8) & 0xff), (uint8_t)(col & 0xff) });
}
void EditorColorPicker::setMainEditorColorRGB(SDL_Color col, bool updateHSVSliders, bool updateRGBSliders, bool updateHSVTextBoxes) {
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

    txtR->text = std::to_string(currentR);
    txtG->text = std::to_string(currentG);
    txtB->text = std::to_string(currentB);
    if (updateHSVTextBoxes) {
        txtH->text = std::to_string(currentH);
        txtS->text = std::to_string(currentS);
        txtV->text = std::to_string(currentV);
    }

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

void EditorColorPicker::updateRGBTextBoxOnInputEvent(std::string data, uint8_t* value)
{
    try {
        int val;
        if (data.size() == 3 && data[0] == 'x') {
            val = std::stoi(data.substr(1), 0, 16);
        }
        else {
            val = std::stoi(data);
        }
        if (val >= 0 && val <= 255) {
            *value = val;
            updateMainEditorColorFromRGBTextBoxes();
        }
    }
    catch (std::exception) {

    }
}

void EditorColorPicker::updateHSVTextBoxOnInputEvent(std::string data, double* value) {
    try {
        double val = std::stod(data);
        if ((value != &currentH && val >= 0 && val <= 1) || (value == &currentH && val >= 0 && val <= 360)) {
			*value = val;
            updateMainEditorColorFromHSVTextBoxes();
		}
    }
    catch (std::exception) {

    }
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