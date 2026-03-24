#include "PalettizedEditorColorPicker.h"
#include "FontRenderer.h"
#include "UIDropdown.h"
#include "Notification.h"
#include "PopupPickColor.h"
#include "UIColorInputField.h"
#include "FileIO.h"
#include "TabbedView.h"
#include "UILabel.h"
#include "MainEditorPalettized.h"

PalettizedEditorColorPicker::PalettizedEditorColorPicker(MainEditorPalettized* c)
{
    caller = c;

    wxWidth = 400;
    wxHeight = 390;

    setupDraggable();
    setupCollapsible();
    addTitleText(TL("vsp.maineditor.panel.colorpicker.title"));

    colorPaletteTabs = new TabbedView({ {"Palette"}, {"Options"} }, 75);
    colorPaletteTabs->position = { 20,30 };
    wxsTarget().addDrawable(colorPaletteTabs);

    eraserButton = new UIButton();
    eraserButton->position = { 20, 350 };
    //eraserButton->text = "E";
    eraserButton->icon = g_iconEraser;
    eraserButton->wxWidth = 30;
    eraserButton->tooltip = "Eraser";
    eraserButton->onClickCallback = [this](UIButton* btn) { _toggleEraser(); };
    wxsTarget().addDrawable(eraserButton);

    pickedColorLabel = new UILabel();
    pickedColorLabel->position = { 60, 350 };
    wxsTarget().addDrawable(pickedColorLabel);

    std::vector<std::string> palettes;
    for (auto& pal : g_namedColorMap) {
        palettes.push_back(pal.getName());
    }

    UIButton* buttonSavePalette = new UIButton();
    buttonSavePalette->position = { 20, 60 };
    buttonSavePalette->text = "Save palette";
    buttonSavePalette->wxWidth = 120;
    buttonSavePalette->wxHeight = 30;
    buttonSavePalette->onClickCallback = [this](UIButton*) {
        platformTrySaveOtherFile(this, { {".voidplt", "voidsprite palette"} }, "save palette", EVENT_PALETTECOLORPICKER_SAVEPALETTE);
    };
    colorPaletteTabs->tabs[1].wxs.addDrawable(buttonSavePalette);

    UIButton* buttonLoadPalette = new UIButton();
    buttonLoadPalette->position = { 150, 60 };
    buttonLoadPalette->text = "Load palette";
    buttonLoadPalette->wxWidth = 120;
    buttonLoadPalette->wxHeight = 30;
    buttonLoadPalette->onClickCallback = [&](UIButton*) {
        std::vector<std::pair<std::string, std::string>> filetypes;
        for (auto& importer : g_paletteImporters) {
            filetypes.push_back({ importer->extension(), importer->name() });
        }
        platformTryLoadOtherFile(this, filetypes, "load palette", EVENT_PALETTECOLORPICKER_LOADPALETTE);
    };
    //buttonLoadPalette->setCallbackListener(EVENT_PALETTECOLORPICKER_LOADPALETTE, this);
    colorPaletteTabs->tabs[1].wxs.addDrawable(buttonLoadPalette);

    UIDropdown* defaultpalettePicker = new UIDropdown(palettes);
    defaultpalettePicker->customButtonGenFunction = [](std::string name, int index) {
        IPalette* p = g_paletteByName(name);
        PaletteButton* ret = new PaletteButton(p != NULL ? p->toRawColorList() : std::vector<u32>{}, 36);
        ret->text = name;
        ret->wxWidth = 300;
        ret->wxHeight = 70;
        return ret;
    };
    defaultpalettePicker->position = XY{ 20, 20 };
    defaultpalettePicker->wxWidth = 300;
    defaultpalettePicker->wxHeight = 30;
    defaultpalettePicker->text = "Change palette";
    defaultpalettePicker->setCallbackListener(EVENT_PALETTECOLORPICKER_PALETTELIST, this);
    colorPaletteTabs->tabs[1].wxs.addDrawable(defaultpalettePicker);

    updateForcedColorPaletteButtons();
}

void PalettizedEditorColorPicker::renderAfterBG(XY position)
{
    if (!enabled) {
        return;
    }
    uint32_t colorNow = (caller->pickedPaletteIndex == -1 || caller->pickedPaletteIndex >= caller->palette.size()) ? 0x00000000 : caller->palette[caller->pickedPaletteIndex];
    SDL_Color colorNowB = { (colorNow >> 16) & 0xff, (colorNow >> 8) & 0xff, colorNow & 0xff, (colorNow >> 24) & 0xff };
    hsv colorNowHSV = rgb2hsv({colorNowB.r / 255.0, colorNowB.g / 255.0, colorNowB.b / 255.0});
    SDL_Color valCol = rgb2sdlcolor(hsv2rgb(hsv{ colorNowHSV.h, colorNowHSV.s, dxmin(colorNowHSV.v + 0.4, 1.0) }));

    SDL_Color previewCol = colorNowB;

    SDL_Rect r = SDL_Rect{ position.x + wxWidth - 60, position.y + wxHeight - 40, 55, 35 };

    SDL_SetRenderDrawColor(g_rd, previewCol.r, previewCol.g, previewCol.b, 0xff);
    SDL_RenderFillRect(g_rd, &r);
    SDL_SetRenderDrawColor(g_rd, valCol.r, valCol.g, valCol.b, 0xff);
    SDL_RenderDrawRect(g_rd, &r);

    pickedColorLabel->setText(frmt("#{}/x{:02X} - #{:08X}", caller->pickedPaletteIndex, caller->pickedPaletteIndex, colorNow));
    pickedColorLabel->color = {valCol.r, valCol.g, valCol.b, 0xff};
}

void PalettizedEditorColorPicker::_toggleEraser() {
    caller->eraserMode = !caller->eraserMode;
    updateEraserButton();
}

void PalettizedEditorColorPicker::updateEraserButton() {
    eraserButton->fill = caller->eraserMode ? Fill::Gradient(0x30FFFFFF, 0x80000000, 0x80FFFFFF, 0x30FFFFFF)
                                            : Fill::Gradient(0x80000000, 0x80000000, 0x80707070, 0x80000000);
}

void PalettizedEditorColorPicker::updatePanelColors() {
    u32 colorNow = (caller->pickedPaletteIndex == -1 || caller->pickedPaletteIndex >= caller->palette.size()) ? 0x00000000 : caller->palette[caller->pickedPaletteIndex];
    SDL_Color colorNowB = { (colorNow >> 16) & 0xff, (colorNow >> 8) & 0xff, colorNow & 0xff, (colorNow >> 24) & 0xff };
    hsv colorNowHSV = rgb2hsv({colorNowB.r / 255.0, colorNowB.g / 255.0, colorNowB.b / 255.0});
    SDL_Color valCol = rgb2sdlcolor(hsv2rgb(hsv{ colorNowHSV.h, colorNowHSV.s, dxmin(colorNowHSV.v + 0.4, 1.0) }));

    u32 devalColor =  sdlcolorToUint32(rgb2sdlcolor(hsv2rgb(hsv{ colorNowHSV.h, colorNowHSV.s, dxmax(colorNowHSV.v / 6, 0.1) })));
    u32 devalColor2 = sdlcolorToUint32(rgb2sdlcolor(hsv2rgb(hsv{ colorNowHSV.h, colorNowHSV.s, dxmax(colorNowHSV.v / 18, 0.05) })));

    fillFocused = Fill::Gradient(modAlpha(devalColor2, 0xaf), modAlpha(devalColor, 0xaf), modAlpha(devalColor, 0xaf), modAlpha(devalColor, 0xaf));
    fillUnfocused = Fill::Gradient(modAlpha(devalColor2, 0x90), modAlpha(devalColor, 0x90), modAlpha(devalColor, 0x90), modAlpha(devalColor, 0x90));

    borderColor = modAlpha(sdlcolorToUint32(valCol), 0x30);

    focusBorderLightup = 1.0;

    focusBorderColor = valCol;
}

void PalettizedEditorColorPicker::eventDropdownItemSelected(int evt_id, int index, std::string name)
{
    if (evt_id == EVENT_PALETTECOLORPICKER_PALETTELIST) {
        IPalette* p = g_paletteByName(name);
        if (p != NULL) {
            caller->setPalette(p->toRawColorList());
        }
        else {
            logerr(frmt("palette not found: {}", name));
        }
    }
}

void PalettizedEditorColorPicker::eventFileSaved(int evt_id, PlatformNativePathString name, int exporterIndex)
{
    if (evt_id == EVENT_PALETTECOLORPICKER_SAVEPALETTE) {
        FILE* f = platformOpenFile(name, PlatformFileModeWB);
        if (f != NULL) {
            fwrite("VOIDPLT", 7, 1, f);
            uint8_t fileversion = 1;
            fwrite(&fileversion, 1, 1, f);
            uint32_t count = caller->palette.size();
            fwrite(&count, 1, 4, f);
            for (uint32_t col : caller->palette) {
                fwrite(&col, 1, 4, f);
            }
            fclose(f);
            g_addNotification(SuccessNotification("Success", "Palette file saved"));
        }
        else {
            g_addNotification(ErrorNotification("Error", "Could not save palette file"));
        }
    }
}

void PalettizedEditorColorPicker::eventFileOpen(int evt_id, PlatformNativePathString name, int importerIndex)
{
    if (evt_id == EVENT_PALETTECOLORPICKER_LOADPALETTE) {
        importerIndex--;
        auto result = g_paletteImporters[importerIndex]->importPalette(name);
        if (result.first) {
            caller->setPalette(result.second);
            updateForcedColorPaletteButtons();
            g_addNotification(SuccessNotification("Success", "Palette file loaded"));
        }
        else {
            g_addNotification(ErrorNotification("Error", "Could not load palette file"));
        }
    }
}

void PalettizedEditorColorPicker::updateForcedColorPaletteButtons()
{
    colorPaletteTabs->tabs[0].wxs.freeAllDrawables();
    colorButtons.clear();

    int paletteIndex = 0;
    for (int y = 0; y < 16 && paletteIndex < caller->palette.size(); y++) {
        for (int x = 0; x < 16 && paletteIndex < caller->palette.size(); x++) {
            u32 col = caller->palette[paletteIndex];
            UIButton* colBtn = new UIButton();
            colBtn->fill = Fill::Solid(col);
            colBtn->wxHeight = 16;
            colBtn->wxWidth = 22;
            colBtn->position = { x * colBtn->wxWidth, 10 + y * colBtn->wxHeight };
            colBtn->onClickCallback = [this, paletteIndex](UIButton* btn) { 
                setPickedPaletteIndex(paletteIndex);
                highlightActiveColorButton();
            };
            colBtn->onRightClickCallback = [this, col, paletteIndex](UIButton* btn) { 
                PopupPickColor* ppc = new PopupPickColor("Pick color", frmt("Select color for palette index {}", paletteIndex), true);
                ppc->setCallbackListener(paletteIndex, this);
                ppc->onColorConfirmedCallback = [this, paletteIndex](PopupPickColor*, u32 color) {
                    caller->palette[paletteIndex] = color;
                    caller->setPalette(caller->palette);
                    updateForcedColorPaletteButtons();
                };
                ppc->setRGB(col);
                ppc->setAlpha(col >> 24);
                g_addPopup(ppc);
            };
            colorPaletteTabs->tabs[0].wxs.addDrawable(colBtn);
            colorButtons.push_back(colBtn);
            paletteIndex++;
        }
    }

    int paletteCount = caller->palette.size();
    if (paletteCount < 256) {
        UIButton* newColorButton = new UIButton();
        newColorButton->wxWidth = 22;
        newColorButton->wxHeight = 16;
        newColorButton->fullWidthIcon = true;
        newColorButton->icon = g_iconNewColor;
        newColorButton->position = { newColorButton->wxWidth * (paletteCount%16), 10 + newColorButton->wxHeight * (paletteCount /16)};
        newColorButton->onClickCallback = [this](UIButton*) { 
            auto paletteCopy = caller->palette;
            paletteCopy.push_back(0xFF000000);
            caller->setPalette(paletteCopy);
            updateForcedColorPaletteButtons();
        };
        colorPaletteTabs->tabs[0].wxs.addDrawable(newColorButton);
    }
    highlightActiveColorButton();
}

void PalettizedEditorColorPicker::highlightActiveColorButton() {
    static SDL_Color defaultBorder = SDL_Color{ 0xff,0xff,0xff,0x30 };

    static SDL_Color highlightBorder = SDL_Color{ 0,0xff,0,0xd0 };
    for (int i = 0; i < colorButtons.size(); i++) {
        colorButtons[i]->colorBorder = i == caller->pickedPaletteIndex ? highlightBorder : defaultBorder;
    }
}

void PalettizedEditorColorPicker::setPickedPaletteIndex(int32_t index)
{
    caller->pickedPaletteIndex = index;
}

void PalettizedEditorColorPicker::toggleEraser() {
    eraserButton->click();
}

void PalettizedEditorColorPicker::setColorRGB(u32 color) {
    setPickedPaletteIndex(color);
}

void PaletteButton::render(XY pos)
{
    UIButton::render(pos);

    if (!colors.empty()) {
        const int colorW = 24, colorH = 12;
        XY origin = xyAdd(pos, { 5, wxHeight - 5 });
        int colorsInOneLine = ixmax(1, (wxWidth - 10) / colorW);
        int lines = ((colors.size() - 1) / colorsInOneLine) + 1;
        origin.y -= lines * colorH;

        for (int i = 0; i < colors.size(); i++) {
            XY pos = xyAdd(origin, { (i % colorsInOneLine) * colorW, (i / colorsInOneLine) * colorH });
            SDL_Rect r = { pos.x, pos.y, colorW, colorH };
            Fill::Solid(colors[i]).fill(r);
        }
    }
}
