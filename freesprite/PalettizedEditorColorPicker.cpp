#include "PalettizedEditorColorPicker.h"
#include "FontRenderer.h"
#include "UIDropdown.h"

PalettizedEditorColorPicker::PalettizedEditorColorPicker(MainEditorPalettized* c)
{
    caller = c;
    upcastCaller = c;

    colorPaletteTabs = new TabbedView({ {"Palette"}, {"Options"} }, 75);
    colorPaletteTabs->position = { 20,30 };
    subWidgets.addDrawable(colorPaletteTabs);

    eraserButton = new UIButton();
    eraserButton->position = { 20, 350 };
    //eraserButton->text = "E";
    eraserButton->icon = g_iconEraser;
    eraserButton->wxWidth = 30;
    eraserButton->setCallbackListener(EVENT_COLORPICKER_TOGGLEERASER, this);
    subWidgets.addDrawable(eraserButton);

    std::vector<std::string> palettes;
    for (auto& pal : g_palettes) {
		palettes.push_back(pal.first);
	}
    UIDropdown* defaultpalettePicker = new UIDropdown(palettes);
    defaultpalettePicker->position = XY{ 20, 20 };
    defaultpalettePicker->wxWidth = 300;
    defaultpalettePicker->wxHeight = 30;
    defaultpalettePicker->text = "Default palettes";
    defaultpalettePicker->setCallbackListener(EVENT_PALETTECOLORPICKER_PALETTELIST, this);
    colorPaletteTabs->tabs[1].wxs.addDrawable(defaultpalettePicker);

    updateForcedColorPaletteButtons();

    /*colorTabs = new TabbedView({{"Visual", g_iconColorVisual},{"HSV", g_iconColorHSV},{"RGB", g_iconColorRGB}}, 90);
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


    blendModeButton = new UIButton();
    blendModeButton->position = { 215, 350 };
    blendModeButton->icon = g_iconBlendMode;
    blendModeButton->wxWidth = 30;
    blendModeButton->setCallbackListener(EVENT_COLORPICKER_TOGGLEBLENDMODE, this);
    blendModeButton->colorBGFocused = blendModeButton->colorBGUnfocused = caller->blendAlphaMode ? SDL_Color{ 0xff,0xff,0xff, 0x30 } : SDL_Color{ 0,0,0, 0x80 };
    subWidgets.addDrawable(blendModeButton);*/
}

void PalettizedEditorColorPicker::render(XY position)
{
    uint32_t colorNow = (upcastCaller->pickedPaletteIndex == -1 || upcastCaller->pickedPaletteIndex >= upcastCaller->palette.size()) ? 0x00000000 : upcastCaller->palette[upcastCaller->pickedPaletteIndex];
    SDL_Color colorNowB = { (colorNow >> 16) & 0xff, (colorNow >> 8) & 0xff, colorNow & 0xff, (colorNow >> 24) & 0xff };
    hsv colorNowHSV = rgb2hsv({colorNowB.r / 255.0, colorNowB.g / 255.0, colorNowB.b / 255.0});

    SDL_Color previewCol = colorNowB;

    SDL_Rect r = SDL_Rect{ position.x, position.y, wxWidth, wxHeight };
    SDL_Color devalColor = rgb2sdlcolor(hsv2rgb(hsv{ colorNowHSV.h, colorNowHSV.s, dxmax(colorNowHSV.v / 6, 0.1) }));
    SDL_Color devalColor2 = rgb2sdlcolor(hsv2rgb(hsv{ colorNowHSV.h, colorNowHSV.s, dxmax(colorNowHSV.v / 18, 0.05) }));
    devalColor.a = devalColor2.a = focused ? 0xaf : 0x30;
    renderGradient(r, sdlcolorToUint32(devalColor2), sdlcolorToUint32(devalColor), sdlcolorToUint32(devalColor), sdlcolorToUint32(devalColor));
    //SDL_SetRenderDrawColor(g_rd, previewCol.r/6, previewCol.g / 6, previewCol.b / 6, focused ? 0xaf : 0x30);
    //SDL_RenderFillRect(g_rd, &r);

    SDL_Color valCol = rgb2sdlcolor(hsv2rgb(hsv{ colorNowHSV.h, colorNowHSV.s, dxmin(colorNowHSV.v + 0.4, 1.0) }));
    if (focused) {
        SDL_SetRenderDrawColor(g_rd, valCol.r, valCol.g, valCol.b, 255);
        drawLine({ position.x, position.y }, { position.x, position.y + wxHeight }, XM1PW3P1(focusTimer.percentElapsedTime(300)));
        drawLine({ position.x, position.y }, { position.x + wxWidth, position.y }, XM1PW3P1(focusTimer.percentElapsedTime(300)));
    }

    XY tabOrigin = xyAdd(position, colorPaletteTabs->position);
    tabOrigin.y += colorPaletteTabs->buttonsHeight;

    r = SDL_Rect{ position.x + wxWidth - 60, position.y + wxHeight - 40, 55, 35 };
    SDL_SetRenderDrawColor(g_rd, previewCol.r, previewCol.g, previewCol.b, focused ? 0xff : 0x30);
    SDL_RenderFillRect(g_rd, &r);
    SDL_SetRenderDrawColor(g_rd, valCol.r, valCol.g, valCol.b, focused ? 0xff : 0x30);
    SDL_RenderDrawRect(g_rd, &r);

    g_fnt->RenderString("COLOR PICKER", position.x + 5, position.y + 1);

    subWidgets.renderAll(position);
}

void PalettizedEditorColorPicker::eventButtonPressed(int evt_id)
{
    if (evt_id == EVENT_COLORPICKER_TOGGLEERASER) {
        toggleEraser();
    }
    else if (evt_id == EVENT_COLORPICKER_TOGGLEBLENDMODE) {
        toggleAlphaBlendMode();
    }
    else if (evt_id >= 200) {
        //uint32_t col = upcastCaller->palette[evt_id - 200];
        setPickedPaletteIndex(evt_id - 200);
    }
}

void PalettizedEditorColorPicker::eventDropdownItemSelected(int evt_id, int index, std::string name)
{
    if (evt_id == EVENT_PALETTECOLORPICKER_PALETTELIST) {
		upcastCaller->setPalette(g_palettes[name]);
	}
}

void PalettizedEditorColorPicker::updateForcedColorPaletteButtons()
{
    colorPaletteTabs->tabs[0].wxs.freeAllDrawables();

    int paletteindex = 0;
    for (int y = 0; y < 16 && paletteindex < upcastCaller->palette.size(); y++) {
        for (int x = 0; x < 16 && paletteindex < upcastCaller->palette.size(); x++) {
            uint32_t col = upcastCaller->palette[paletteindex++];
            UIButton* colBtn = new UIButton();
            colBtn->colorBGFocused = colBtn->colorBGUnfocused = SDL_Color{ (uint8_t)((col >> 16) & 0xff), (uint8_t)((col >> 8) & 0xff), (uint8_t)(col & 0xff), (uint8_t)((col >> 24) & 0xff) };
            colBtn->wxHeight = 16;
            colBtn->wxWidth = 22;
            colBtn->position = { x * colBtn->wxWidth, 10 + y * colBtn->wxHeight };
            colBtn->setCallbackListener(200 + (y * 16 + x), this);
            colorPaletteTabs->tabs[0].wxs.addDrawable(colBtn);
        }
    }
}

void PalettizedEditorColorPicker::setPickedPaletteIndex(int32_t index)
{
    upcastCaller->pickedPaletteIndex = index;
}
