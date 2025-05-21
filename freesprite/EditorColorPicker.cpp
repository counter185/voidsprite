#include "globals.h"
#include "FontRenderer.h"
#include "maineditor.h"
#include "EditorColorPicker.h"
#include "mathops.h"
#include "ScrollingPanel.h"
#include "Notification.h"

EditorColorPicker::EditorColorPicker(MainEditor* c) : UIColorPicker() {
    caller = c;

    eraserButton = new UIButton();
    eraserButton->position = { 20, 350 };
    //eraserButton->text = "E";
    eraserButton->icon = g_iconEraser;
    eraserButton->wxWidth = 30;
    eraserButton->tooltip = "Eraser";
	eraserButton->onClickCallback = [this](UIButton* btn) { toggleEraser(); };
    subWidgets.addDrawable(eraserButton);

    blendModeButton = new UIButton();
    blendModeButton->position = { 215, 350 };
    blendModeButton->icon = g_iconBlendMode;
    blendModeButton->wxWidth = 30;
    blendModeButton->tooltip = "Alpha blend";
	blendModeButton->onClickCallback = [this](UIButton* btn) { toggleAlphaBlendMode(); };
    subWidgets.addDrawable(blendModeButton);

	onColorChangedCallback = [this](UIColorPicker* from, u32 col) {
		caller->pickedColor = col;
	};

    updateEraserAndAlphaBlendButtons();
}

void EditorColorPicker::render(XY position)
{
    if (!enabled) {
        return; 
    }
    SDL_Color previewCol = rgb2sdlcolor(hsv2rgb(hsv{ currentH, currentS, currentV }));

    SDL_Rect r = SDL_Rect{ position.x, position.y, wxWidth, wxHeight };
    SDL_Color devalColor = rgb2sdlcolor(hsv2rgb(hsv{ currentH, currentS, dxmax(currentV / 6, 0.1) }));
    SDL_Color devalColor2 = rgb2sdlcolor(hsv2rgb(hsv{ currentH, currentS, dxmax(currentV / 18, 0.05) }));
    devalColor.a = devalColor2.a = focused ? 0xaf : 0x90;
    renderGradient(r, sdlcolorToUint32(devalColor2), sdlcolorToUint32(devalColor), sdlcolorToUint32(devalColor), sdlcolorToUint32(devalColor));
    //SDL_SetRenderDrawColor(g_rd, previewCol.r/6, previewCol.g / 6, previewCol.b / 6, focused ? 0xaf : 0x30);
    //SDL_RenderFillRect(g_rd, &r);

    SDL_Color valCol = rgb2sdlcolor(hsv2rgb(hsv{ currentH, currentS, dxmin(currentV + 0.4, 1.0) }));
    if (thisOrParentFocused()) {
        SDL_SetRenderDrawColor(g_rd, valCol.r, valCol.g, valCol.b, 255);
        drawLine({ position.x, position.y }, { position.x, position.y + wxHeight }, XM1PW3P1(thisOrParentFocusTimer().percentElapsedTime(300)));
        drawLine({ position.x, position.y }, { position.x + wxWidth, position.y  }, XM1PW3P1(thisOrParentFocusTimer().percentElapsedTime(300)));
    }

    XY tabOrigin = xyAdd(position, colorTabs->position);
    tabOrigin.y += colorTabs->buttonsHeight;

    r = SDL_Rect{ position.x + wxWidth - 60, position.y + wxHeight - 40, 55, 35 };
    SDL_SetRenderDrawColor(g_rd, previewCol.r, previewCol.g, previewCol.b, 0xff);
    SDL_RenderFillRect(g_rd, &r);
    SDL_SetRenderDrawColor(g_rd, valCol.r, valCol.g, valCol.b, 0xff);
    SDL_RenderDrawRect(g_rd, &r);

    //g_fnt->RenderString("COLOR PICKER", position.x + 5, position.y + 1);

    UIColorPicker::render(position);
}

void EditorColorPicker::updateEraserAndAlphaBlendButtons() {
    eraserButton->fill = caller->eraserMode ? Fill::Gradient(0x30FFFFFF, 0x80000000, 0x80FFFFFF, 0x30FFFFFF)
                                            : Fill::Gradient(0x80000000, 0x80000000, 0x80707070, 0x80000000);
    blendModeButton->fill = caller->blendAlphaMode ? Fill::Gradient(0x30FFFFFF, 0x80000000, 0x80FFFFFF, 0x30FFFFFF)
                                                   : Fill::Gradient(0x80000000, 0x80000000, 0x80707070, 0x80000000);
}

void EditorColorPicker::toggleEraser() 
{
    caller->eraserMode = !caller->eraserMode;
    updateEraserAndAlphaBlendButtons();
}

void EditorColorPicker::toggleAlphaBlendMode()
{
    caller->blendAlphaMode = !caller->blendAlphaMode;
    updateEraserAndAlphaBlendButtons();
}

void EditorColorPicker::pushLastColor(uint32_t col)
{
    col |= 0xff000000;
    auto fnd = std::find(lastColors.begin(), lastColors.end(), col);

    if (fnd == lastColors.begin() && lastColors.size() > 0) {
        return;
    }

    //logprintf("pushing new color!\n");
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
        ColorPickerColorButton* colBtn = new ColorPickerColorButton(this, col);
        colBtn->position = { posX, posY };
        colBtn->wxHeight = 24;
        colBtn->wxWidth = 30;
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