#include "globals.h"
#include "FontRenderer.h"
#include "maineditor.h"
#include "EditorColorPicker.h"
#include "mathops.h"
#include "ScrollingPanel.h"
#include "Notification.h"
#include "TabbedView.h"
#include "UITextField.h"
#include "UIColorPicker.h"
#include "UIDropdown.h"

EditorColorPicker::EditorColorPicker(MainEditor* c) : callerColorList(c) {
    caller = c;

    wxWidth = 400;
    wxHeight = 390;

    setupDraggable();
    setupCollapsible();
    addTitleText(TL("vsp.maineditor.panel.colorpicker.title"));

    wColorPicker = new UIColorPicker();
    wColorPicker->position = {0,0};
    wColorPicker->addExtraPalette(&this->callerColorList);
    wxsTarget().addDrawable(wColorPicker);

    eraserButton = new UIButton();
    eraserButton->position = { 20, 350 };
    //eraserButton->text = "E";
    eraserButton->icon = g_iconEraser;
    eraserButton->wxWidth = 30;
    eraserButton->tooltip = "Eraser";
    eraserButton->onClickCallback = [this](UIButton* btn) { _toggleEraser(); };
    wxsTarget().addDrawable(eraserButton);

    UINumberInputField* alphaInput = new UINumberInputField((int*)&c->pickedAlpha);
    alphaInput->position = { 230, 350 };
    alphaInput->wxWidth = g_fnt->StatStringDimensions("888_").x + 4;
    alphaInput->textColor = SDL_Color{255, 255, 255, 255};
    alphaInput->tooltip = "Alpha (transparency)";
    alphaInput->validateFunction = [](int v) { return v >= 0 && v <= 255; };
    alphaInput->valueUpdatedCallback = [this](int newVal) {
        updateAlphaSlider();
    };
    wxsTarget().addDrawable(alphaInput);

    alphaSlider = new UIColorSlider();
    alphaSlider->colors = { 0xFFFFFFFF, 0x00FFFFFF };
    alphaSlider->allowAlpha = true;
    alphaSlider->verticalSlider = true;
    alphaSlider->position = g_config.hueWheelInsteadOfSlider ? XY{320, 0} : XY{ 320, 40 };
    alphaSlider->wxWidth = 30;
    alphaSlider->wxHeight = g_config.hueWheelInsteadOfSlider ? 240 : 200;
    updateAlphaSlider();
    alphaSlider->onChangeValueCallback = [this](UISlider* s, float v) {
        caller->pickedAlpha = (u8)(v * 255);
    };
    wColorPicker->colorTabs->tabs[0].wxs.addDrawable(alphaSlider);

    wColorPicker->onColorChangedCallback = [this](UIColorPicker* from, u32 col) {
        caller->pickedColor = col;
    };

    updateEraserAndAlphaBlendButtons();
}

void EditorColorPicker::renderAfterBG(XY position)
{
    if (!enabled) {
        return; 
    }
    SDL_Color previewCol = rgb2sdlcolor(hsv2rgb(hsv{ wColorPicker->currentH, wColorPicker->currentS, wColorPicker->currentV }));
    SDL_Color valCol = rgb2sdlcolor(hsv2rgb(hsv{ wColorPicker->currentH, wColorPicker->currentS, dxmin(wColorPicker->currentV + 0.4, 1.0) }));

    SDL_Rect r = SDL_Rect{ position.x, position.y, wxWidth, wxHeight };

    r = SDL_Rect{ position.x + wxWidth - 60, position.y + wxHeight - 40, 55, 35 };
    SDL_SetRenderDrawColor(g_rd, previewCol.r, previewCol.g, previewCol.b, 0xff);
    SDL_RenderFillRect(g_rd, &r);
    SDL_SetRenderDrawColor(g_rd, valCol.r, valCol.g, valCol.b, 0xff);
    SDL_RenderDrawRect(g_rd, &r);
}

void EditorColorPicker::updateEraserAndAlphaBlendButtons() {
    eraserButton->fill = caller->eraserMode ? Fill::Gradient(0x30FFFFFF, 0x80000000, 0x80FFFFFF, 0x30FFFFFF)
                                            : Fill::Gradient(0x80000000, 0x80000000, 0x80707070, 0x80000000);
}

void EditorColorPicker::_toggleEraser() {
    caller->eraserMode = !caller->eraserMode;
    updateEraserAndAlphaBlendButtons();
}

void EditorColorPicker::updatePanelColors() {
    double minValue = g_config.acrylicPanels ? 0.01 : 0.1;
    double minValue2 = g_config.acrylicPanels ? 0.005 : 0.05;

    u32 devalColor = sdlcolorToUint32(rgb2sdlcolor(hsv2rgb(hsv{ wColorPicker->currentH, wColorPicker->currentS, dxmax(wColorPicker->currentV / 6, minValue) })));
    u32 devalColor2 = sdlcolorToUint32(rgb2sdlcolor(hsv2rgb(hsv{ wColorPicker->currentH, wColorPicker->currentS, dxmax(wColorPicker->currentV / 18, minValue2) })));
    //devalColor.a = devalColor2.a = focused ? 0xaf : 0x90;
    SDL_Color valCol = rgb2sdlcolor(hsv2rgb(hsv{ wColorPicker->currentH, wColorPicker->currentS, dxmin(wColorPicker->currentV + 0.4, 1.0) }));

    fillFocused = Fill::Gradient(modAlpha(devalColor2, 0xaf), modAlpha(devalColor, 0xaf), modAlpha(devalColor, 0xaf), modAlpha(devalColor, 0xaf));
    fillUnfocused = Fill::Gradient(modAlpha(devalColor2, 0x90), modAlpha(devalColor, 0x90), modAlpha(devalColor, 0x90), modAlpha(devalColor, 0x90));

    borderColor = modAlpha(sdlcolorToUint32(valCol), 0x30);

    focusBorderLightup = 1.0;

    focusBorderColor = valCol;
}

void EditorColorPicker::toggleEraser() 
{
    eraserButton->click();
}

void EditorColorPicker::forceFocusOnColorInputField() {
    wColorPicker->subWidgets.forceFocusOn(wColorPicker->colorTextField);
    wColorPicker->colorTextField->setText("");
}

void EditorColorPicker::setColorRGB(u32 color) {
    wColorPicker->setColorRGB(color);
}

void EditorColorPicker::updateAlphaSlider()
{
    alphaSlider->setValue(0, 255, caller->pickedAlpha);
}

void EditorColorPicker::reloadColorLists() {
    wColorPicker->reloadColorLists();
}

void EditorColorPicker::actionCtrlScroll(float scrollAmount) {
    wColorPicker->setColorHSV(wColorPicker->currentH, fxmin(fxmax(wColorPicker->currentS + 0.1 * scrollAmount, 0), 1), wColorPicker->currentV);
}

void EditorColorPicker::actionShiftScroll(float scrollAmount) {
    double newH = dxmin(dxmax(wColorPicker->currentH + (360.0 / 12) * scrollAmount, 0), 359);
    wColorPicker->setColorHSV(newH, wColorPicker->currentS, wColorPicker->currentV);
}

void EditorColorPicker::pushLastColor(u32 col)
{
    col |= 0xff000000;
    auto fnd = std::find(lastColors.begin(), lastColors.end(), col);

    if (fnd == lastColors.begin() && lastColors.size() > 0) {
        return;
    }

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

void EditorColorPicker::clearLastColors() {
    lastColors.clear();
    lastColorsChanged = true;
}

void EditorColorPicker::updateLastColorButtons()
{
    if (!lastColorsChanged) {
        return;
    }
    wColorPicker->colorModeTabs->tabs[1].wxs.freeAllDrawables();
    int x = 0;
    int xx = 0;
    int y = 0;
    int posX = 0;
    int posY = 5;
    for (u32& col : lastColors) {
        ColorPickerColorButton* colBtn = new ColorPickerColorButton(wColorPicker, col);
        colBtn->position = { posX, posY };
        colBtn->wxHeight = 24;
        colBtn->wxWidth = 30;
        wColorPicker->colorModeTabs->tabs[1].wxs.addDrawable(colBtn);

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