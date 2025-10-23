#include "UIColorPicker.h"
#include "UITextField.h"
#include "UILabel.h"
#include "TabbedView.h"
#include "ScrollingPanel.h"
#include "Notification.h"
#include "FileIO.h"
#include "UIStackPanel.h"
#include "PopupPickColor.h"
#include "PopupYesNo.h"
#include "PopupTextBox.h"
#include "io/io_voidsprite.h"

#if _WIN32
#include <windows.h>
#endif

ColorPickerColorButton::ColorPickerColorButton(UIColorPicker* parent, u32 color) : UIButton()
{
    this->parent = parent;
    this->color = color;
    this->fill = Fill::Solid(color);
    this->onClickCallback = [this](UIButton*) {
        this->parent->setColorRGB(this->color);
    };
}

UIColorPicker::UIColorPicker(int w, int h)
{
    wxWidth = w;
    wxHeight = h;
}

UIColorPicker::UIColorPicker() : UIColorPicker(400, 390)
{

    colorModeTabs = new TabbedView({ 
        { TL("vsp.maineditor.panel.colorpicker.tab.colors")},
        { TL("vsp.maineditor.panel.colorpicker.tab.last")},
        { TL("vsp.maineditor.panel.colorpicker.tab.palettes")}
        }, 85);
    colorModeTabs->position = XY{ 20,30 };
    subWidgets.addDrawable(colorModeTabs);

    //-----------------
    //| Colors tab

    colorTabs = new TabbedView({ 
        { TL("vsp.maineditor.panel.colorpicker.tab.visual"), g_iconColorVisual},
        { TL("vsp.maineditor.panel.colorpicker.tab.sliders"), g_iconColorHSV},
        { TL("vsp.maineditor.panel.colorpicker.tab.other")}
        }, 100);
    colorTabs->position = XY{ 0,5 };
    colorModeTabs->tabs[0].wxs.addDrawable(colorTabs);

    //   -------------------
    //   | Visual tab

    hueSlider = new UIHueSlider(this);
    hueSlider->position = XY{ 0,10 };
    colorTabs->tabs[0].wxs.addDrawable(hueSlider);

    satValSlider = new UISVPicker(this);
    satValSlider->position = XY{ 0,40 };
    colorTabs->tabs[0].wxs.addDrawable(satValSlider);

    //   -------------------
    //   | Sliders tab 

    UILabel* labelH = new UILabel("H");
    UILabel* labelR = new UILabel("R");
    labelH->position = XY{ 0, 120 + 10 };
    labelR->position = XY{ 0, 10 };
    colorTabs->tabs[1].wxs.addDrawable(labelH);
    colorTabs->tabs[1].wxs.addDrawable(labelR);

    UILabel* labelS = new UILabel("S");
    UILabel* labelG = new UILabel("G");
    labelS->position = XY{ 0, 120 + 45 };
    labelG->position = XY{ 0, 45 };
    colorTabs->tabs[1].wxs.addDrawable(labelS);
    colorTabs->tabs[1].wxs.addDrawable(labelG);

    UILabel* labelV = new UILabel("V");
    UILabel* labelB = new UILabel("B");
    labelV->position = XY{ 0, 120 + 80 };
    labelB->position = XY{ 0, 80 };
    colorTabs->tabs[1].wxs.addDrawable(labelV);
    colorTabs->tabs[1].wxs.addDrawable(labelB);

    txtR = new UITextField();
    txtG = new UITextField();
    txtB = new UITextField();
    txtH = new UITextField();
    txtS = new UITextField();
    txtV = new UITextField();
    txtH->tooltip = txtS->tooltip = txtV->tooltip = "Must be a floating point number";
    txtR->tooltip = txtG->tooltip = txtB->tooltip = "Must be a whole number from 0 to 255.\nFor hex numbers, enter x<number>";
    txtR->position = XY{ 240, 10 };
    txtH->position = XY{ 240, 120 + 10 };
    txtR->textColor = { 0xff,0,0,0xff };
    txtG->position = XY{ 240, 45 };
    txtS->position = XY{ 240, 120 + 45 };
    txtG->textColor = { 0,0xff,0,0xff };
    txtB->position = XY{ 240, 80 };
    txtV->position = XY{ 240, 120 + 80 };
    txtB->textColor = { 0,0,0xff,0xff };
    txtR->wxWidth = txtG->wxWidth = txtB->wxWidth = 60;
    txtH->wxWidth = txtS->wxWidth = txtV->wxWidth = 120;
    txtR->wxHeight = txtG->wxHeight = txtB->wxHeight = txtH->wxHeight = txtS->wxHeight = txtV->wxHeight = 28;
    txtR->setCallbackListener(EVENT_COLORPICKER_TBOXR, this);
    txtG->setCallbackListener(EVENT_COLORPICKER_TBOXG, this);
    txtB->setCallbackListener(EVENT_COLORPICKER_TBOXB, this);
    txtH->setCallbackListener(EVENT_COLORPICKER_TBOXH, this);
    txtS->setCallbackListener(EVENT_COLORPICKER_TBOXS, this);
    txtV->setCallbackListener(EVENT_COLORPICKER_TBOXV, this);
    colorTabs->tabs[1].wxs.addDrawable(txtR);
    colorTabs->tabs[1].wxs.addDrawable(txtG);
    colorTabs->tabs[1].wxs.addDrawable(txtB);
    colorTabs->tabs[1].wxs.addDrawable(txtH);
    colorTabs->tabs[1].wxs.addDrawable(txtS);
    colorTabs->tabs[1].wxs.addDrawable(txtV);

    sliderH = new UIColorSlider();
    sliderH->position = XY{ 30, 120 + 10 };
    sliderH->wxWidth = 200;
    sliderH->wxHeight = 25;
    sliderH->setCallbackListener(EVENT_COLORPICKER_SLIDERH, this);
    colorTabs->tabs[1].wxs.addDrawable(sliderH);

    sliderS = new UIColorSlider();
    sliderS->position = XY{ 30, 120 + 45 };
    sliderS->wxWidth = 200;
    sliderS->wxHeight = 25;
    sliderS->setCallbackListener(EVENT_COLORPICKER_SLIDERS, this);
    colorTabs->tabs[1].wxs.addDrawable(sliderS);

    sliderV = new UIColorSlider();
    sliderV->position = XY{ 30, 120 + 80 };
    sliderV->wxWidth = 200;
    sliderV->wxHeight = 25;
    sliderV->setCallbackListener(EVENT_COLORPICKER_SLIDERV, this);
    colorTabs->tabs[1].wxs.addDrawable(sliderV);

    sliderR = new UIColorSlider();
    sliderR->position = XY{ 30, 10 };
    sliderR->wxWidth = 200;
    sliderR->wxHeight = 25;
    sliderR->setCallbackListener(EVENT_COLORPICKER_SLIDERR, this);
    colorTabs->tabs[1].wxs.addDrawable(sliderR);

    sliderG = new UIColorSlider();
    sliderG->position = XY{ 30, 45 };
    sliderG->wxWidth = 200;
    sliderG->wxHeight = 25;
    sliderG->setCallbackListener(EVENT_COLORPICKER_SLIDERG, this);
    colorTabs->tabs[1].wxs.addDrawable(sliderG);

    sliderB = new UIColorSlider();
    sliderB->position = XY{ 30, 80 };
    sliderB->wxWidth = 200;
    sliderB->wxHeight = 25;
    sliderB->setCallbackListener(EVENT_COLORPICKER_SLIDERB, this);
    colorTabs->tabs[1].wxs.addDrawable(sliderB);

    //   -------------------
    //   | Other tab 
    ScrollingPanel* colorModelsPanel = new ScrollingPanel();
    colorModelsPanel->position = XY{ 0, 5 };
    colorModelsPanel->scrollHorizontally = false;
    colorModelsPanel->scrollVertically = true;
    colorModelsPanel->wxWidth = 370;
    colorModelsPanel->wxHeight = 250;
    colorTabs->tabs[2].wxs.addDrawable(colorModelsPanel);
    {
        int yNow = 5;
        int i = 0;
        for (auto& model : g_colorModels) {
            std::string name = model.first;
            ColorModel* mptr = model.second;

            UILabel* nameLabel = new UILabel(name);
            nameLabel->position = XY{ 5, yNow };
            colorModelsPanel->subWidgets.addDrawable(nameLabel);
            yNow += 30;

            std::map<std::string, ColorModelValue> components;
            auto componentRanges = mptr->componentRanges();

            for (auto& component : mptr->components()) {
                UILabel* cnameLabel = new UILabel(component);
                cnameLabel->position = XY{ 10, yNow };
                colorModelsPanel->subWidgets.addDrawable(cnameLabel);
                UILabel* cvalueLabel = new UILabel("0");
                cvalueLabel->position = XY{ 50, yNow };
                colorModelsPanel->subWidgets.addDrawable(cvalueLabel);
                UIColorSlider* slider = new UIColorSlider();
                slider->position = XY{ 120, yNow };
                slider->wxWidth = 200;
                slider->wxHeight = 20;
                slider->setCallbackListener(100 + i, this);
                colorModelsPanel->subWidgets.addDrawable(slider);
                yNow += 30;
                components[component] = { slider, cvalueLabel, 0, componentRanges[component] };
            }

            colorModels.push_back({ name, {mptr, components} });
            i++;
        }

#if _WIN32
        UIButton* oldColorPickerButton = new UIButton(TL("vsp.maineditor.panel.colorpicker.win32picker"));
        oldColorPickerButton->position = XY{ 5, yNow };
        oldColorPickerButton->wxWidth = 210;
        oldColorPickerButton->onClickCallback = [this](UIButton*) { openOldWindowsColorPicker(); };
        colorModelsPanel->subWidgets.addDrawable(oldColorPickerButton);
        yNow += 35;
#endif
    }
    //-----------------
    //| Palettes tab
    palettePanel = new ScrollingPanel();
    palettePanel->position = XY{ 0,5 };
    palettePanel->scrollHorizontally = false;
    palettePanel->scrollVertically = true;
    palettePanel->wxWidth = 370;
    palettePanel->wxHeight = 270;
    reloadColorLists();
    colorModeTabs->tabs[2].wxs.addDrawable(palettePanel);


    //widgets outside of tabs

    colorTextField = new UITextField();
    colorTextField->isColorField = true;
    colorTextField->position = XY{ 60, 350 };
    colorTextField->wxWidth = 140;
    colorTextField->tooltip = TL("vsp.maineditor.panel.colorpicker.colorinput.tooltip");
    colorTextField->setCallbackListener(EVENT_COLORPICKER_TEXTFIELD, this);
    subWidgets.addDrawable(colorTextField);
}

void UIColorPicker::eventTextInput(int evt_id, std::string data)
{
    if (evt_id == EVENT_COLORPICKER_TEXTFIELD) {
        if (data.size() == 6 && colorTextField->isValidOrPartialColor()) {
            data = "#" + data;
        }
        if (data.size() == 7) {
            unsigned int col;
            if (tryRgbStringToColor(data.substr(1), &col)) {
                col |= 0xff000000;
                setColorRGB(col);
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

void UIColorPicker::eventTextInputConfirm(int evt_id, std::string data)
{
    if (evt_id == EVENT_COLORPICKER_TEXTFIELD) {
        if (data == "rand" || data == "random") {
            setColorRGB(PackRGBAtoARGB(rand() % 256, rand() % 256, rand() % 256, 255));
        }
        else if (g_colors.contains(data)) {
            setColorRGB(g_colors[data]);
        }
    }
}

void UIColorPicker::eventSliderPosChanged(int evt_id, float f)
{
    switch (evt_id) {
    case EVENT_COLORPICKER_SLIDERH:
        currentH = f * 360.0f;
        colorUpdatedFromHSVSliders();
        break;
    case EVENT_COLORPICKER_SLIDERS:
        currentS = f;
        colorUpdatedFromHSVSliders();
        break;
    case EVENT_COLORPICKER_SLIDERV:
        currentV = f;
        colorUpdatedFromHSVSliders();
        break;
    case EVENT_COLORPICKER_SLIDERR:
        currentR = f * 255;
        colorUpdatedFromRGBSliders();
        break;
    case EVENT_COLORPICKER_SLIDERG:
        currentG = f * 255;
        colorUpdatedFromRGBSliders();
        break;
    case EVENT_COLORPICKER_SLIDERB:
        currentB = f * 255;
        colorUpdatedFromRGBSliders();
        break;
    default:
        if (evt_id >= 100 && (evt_id - 100) < colorModels.size()) {
            int modelid = evt_id - 100;
            auto& model = colorModels[modelid];
            std::string modelName = model.first;
            ColorModelData& modelData = model.second;
            ColorModel* mptr = modelData.targetModel;
            std::map<std::string, double> componentData;
            for (auto& component : modelData.components) {
                component.second.valueNow = component.second.valueSlider->getValue(component.second.range.first, component.second.range.second);
                component.second.valueLabel->setText(frmt("{:.2f}", component.second.valueNow));
                componentData[component.first] = component.second.valueNow;
            }

            u32 color = mptr->toRGB(componentData);
            colorUpdatedRGB(uint32ToSDLColor(color), COLORMODELS_SLIDER, modelName);
        }
        break;
    }
}

void UIColorPicker::eventFileOpen(int evt_id, PlatformNativePathString name, int importerIndex)
{
    if (evt_id == EVENT_PALETTECOLORPICKER_LOADPALETTE) {
        std::string fname = fileNameFromPath(convertStringToUTF8OnWin32(name));
        if (platformCopyFile(name, platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("/palettes/" + fname))) {
            g_addNotification(SuccessShortNotification("Success", "Palette imported into colorlist"));
            g_reloadColorMap();
            reloadColorLists();
        }
        else {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Failed to import palette"));
        }
    }
}

void UIColorPicker::reloadColorLists()
{
    palettePanel->subWidgets.freeAllDrawables();

    UIStackPanel* stack = new UIStackPanel();
    stack->manuallyRecalculateLayout = true;
    stack->takeMouseWheelEvents = false;
    stack->position = { 0,0 };

    palettePanel->subWidgets.addDrawable(stack);

    Panel* topButtonsPanel = new Panel();
    topButtonsPanel->sizeToContent = true;

    UIButton* toggleAllButton = new UIButton(TL("vsp.maineditor.panel.colorpicker.tab.palettes.toggleall"));
    toggleAllButton->position = XY{ 5, 5 };
    toggleAllButton->wxWidth = 100;
    topButtonsPanel->subWidgets.addDrawable(toggleAllButton);

    stack->addWidget(topButtonsPanel);

    std::vector<UIButton*> collapseButtons;


    for (auto& p : g_namedColorMap) {
        Panel* pp = new Panel();
        pp->sizeToContent = true;
        UIButton* collapseButton = new UIButton("-");
        collapseButton->position = { 5,10 };
        collapseButton->wxWidth = 20;
        collapseButton->wxHeight = 20;
        pp->subWidgets.addDrawable(collapseButton);
        collapseButtons.push_back(collapseButton);


        UILabel* nameLabel = new UILabel(p.name);
        nameLabel->position = XY{ 30, 10 };
        pp->subWidgets.addDrawable(nameLabel);

        stack->addWidget(pp);

        int yNow = 0;
        int xNow = 0;
        
        Panel* colorButtonsPanel = new Panel();
        colorButtonsPanel->sizeToContent = true;
        colorButtonsPanel->passThroughMouse = true;

        //check if we should create large color buttons
        int lineCount = 1;
        int currentCountInLine = 0;
        int maxCountInLine = 0;
        for (auto& color : p.colorMap) {
            if (color.first == HINT_NEXT_LINE) {
                currentCountInLine = 0;
                lineCount++;
            }
            else {
                if (++currentCountInLine > maxCountInLine) {
                    maxCountInLine = currentCountInLine;
                }
            }
        }
        bool largeColorButtons = maxCountInLine < 8;

        int itemHeight = largeColorButtons ? 32 : 20;

        bool paletteModifiable = canEditPalettes && p.correspondingExporter != NULL && !p.path.empty();

        //create all the color buttons
        int colorIndex = 0;
        for (auto& color : p.colorMap) {
            if (color.first == HINT_NEXT_LINE) {
                xNow = 0;
                yNow += itemHeight;
                colorIndex++;
                continue;
            }

            ColorPickerColorButton* b = new ColorPickerColorButton(this, color.second);
            b->position = XY{ xNow, yNow };
            b->tooltip = color.first;
            b->wxWidth = largeColorButtons ? 48 : 24;
            b->wxHeight = itemHeight;
            if (paletteModifiable) {
                b->onRightClickCallback = [colorIndex, this, p, color](UIButton*) {
                    //todo: make this a context menu with remove, move up, move down options
                    PopupYesNo* popup = new PopupYesNo("Remove color",
                        frmt("Remove the color #{} : {:06X} from palette {}?", colorIndex, color.second, p.name));
                    popup->onFinishCallback = [this, colorIndex, p](PopupYesNo*, bool result) {
                        if (result) {
                            if (p.colorMap.size() > colorIndex) {
                                NamedColorPalette newP = p;
                                newP.colorMap.erase(newP.colorMap.begin() + colorIndex);
                                if (g_updateColorMapFile(newP)) {
                                    g_reloadColorMap();
                                    reloadColorLists();
                                }
                            }
                        }
                    };
                    g_addPopup(popup);
                };
            }
            colorButtonsPanel->subWidgets.addDrawable(b);
            xNow += b->wxWidth;
            if (xNow + b->wxWidth >= palettePanel->wxWidth) {
                xNow = 0;
                yNow += b->wxHeight;
            }
            colorIndex++;
        }

        //create the + button if applicable
        if (paletteModifiable) {
            UIButton* addColorButton = new UIButton("+");
            addColorButton->wxWidth = 24;
            addColorButton->wxHeight = 20;
            addColorButton->position = XY{ xNow, yNow };
            addColorButton->tooltip = TL("vsp.maineditor.panel.colorpicker.tab.palettes.addcolor.tooltip");
            addColorButton->onClickCallback = [this, p](UIButton*) {
                if (!g_shiftModifier) {
                    PopupPickColor* popup = new PopupPickColor("Add color", "");
                    popup->colorPicker->canEditPalettes = false;
                    popup->colorPicker->reloadColorLists();
                    popup->setRGB(colorNowU32);
                    popup->onColorConfirmedCallback = [this, p](PopupPickColor*, u32 color) {
                        addColorToPalette(p, color);
                        };
                    g_addPopup(popup);
                }
                else {
                    addColorToPalette(p, colorNowU32);
                }
            };
            colorButtonsPanel->subWidgets.addDrawable(addColorButton);
        }
        stack->addWidget(colorButtonsPanel);

        collapseButton->onClickCallback = [colorButtonsPanel, stack](UIButton* btn) {
            colorButtonsPanel->enabled = !colorButtonsPanel->enabled;
            btn->text = colorButtonsPanel->enabled ? "-" : "+";
            stack->recalculateLayout();
        };
    }

    Panel* otherButtons = new Panel();
    otherButtons->sizeToContent = true;
    otherButtons->passThroughMouse = true;

    UIButton* loadNewButton = new UIButton(TL("vsp.maineditor.panel.colorpicker.tab.palettes.loadpalette"));
    loadNewButton->position = XY{ 5, 20 };
    loadNewButton->wxWidth = 140;
    loadNewButton->onClickCallback = [&](UIButton*) {
        std::vector<std::pair<std::string, std::string>> filetypes;
        for (auto& importer : g_paletteImporters) {
            filetypes.push_back({ importer->extension(), importer->name() });
        }
        platformTryLoadOtherFile(this, filetypes, TL("vsp.popup.openpalette"), EVENT_PALETTECOLORPICKER_LOADPALETTE);
        };
    otherButtons->subWidgets.addDrawable(loadNewButton);

    UIButton* reloadButton = new UIButton(TL("vsp.maineditor.panel.colorpicker.tab.palettes.refresh"));
    reloadButton->position = XY{ 150, 20 };
    reloadButton->wxWidth = 100;
    reloadButton->onClickCallback = [this](UIButton* btn) { g_reloadColorMap(); reloadColorLists(); };
    otherButtons->subWidgets.addDrawable(reloadButton);

    UIButton* newPaletteButton = new UIButton(TL("vsp.maineditor.panel.colorpicker.tab.palettes.newpalette"));
    newPaletteButton->position = XY{ 5, 60 };
    newPaletteButton->wxWidth = 140;
    newPaletteButton->onClickCallback = [this](UIButton*) {
        PopupTextBox* popup = new PopupTextBox(TL("vsp.maineditor.panel.colorpicker.tab.palettes.newpalette.popup.title"),
            TL("vsp.maineditor.panel.colorpicker.tab.palettes.newpalette.popup.desc"), "");
        popup->onTextInputConfirmedCallback = [this](PopupTextBox*, std::string name) {
            if (name.find_first_of("/\\") == std::string::npos) {
                PlatformNativePathString newPalettePath = platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("/palettes/" + name + ".voidplt");
                if (writePltVOIDPLT(newPalettePath, {})) {
                    g_reloadColorMap();
                    reloadColorLists();
                }
                else {
                    g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.error.exportfail")));
                }
            }
            else {
                //invalid characters in name
                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.error.exportfail")));
            }
        };
        g_addPopup(popup);
    };
    otherButtons->subWidgets.addDrawable(newPaletteButton);

    stack->addWidget(otherButtons);

    toggleAllButton->onClickCallback = [collapseButtons](UIButton*) {
        for (auto b : collapseButtons) {
            b->click();
        }
    };
}

void UIColorPicker::addColorToPalette(NamedColorPalette p, u32 color)
{
    NamedColorPalette newP = p;
    newP.colorMap.push_back(std::pair<std::string, u32>(std::string("newcolor"), color));
    if (g_updateColorMapFile(newP)) {
        g_reloadColorMap();
        reloadColorLists();
    }
}

void UIColorPicker::updateColorModelSliders(std::string dontUpdate)
{
    for (auto& model : colorModels) {
        ColorModel* mptr = model.second.targetModel;
        std::map<std::string, double> modelColorValue;
        if (model.first != dontUpdate) {
            modelColorValue = mptr->fromRGB(PackRGBAtoARGB(currentR, currentG, currentB, 255));
        }
        else {
            for (auto& component : model.second.components) {
                modelColorValue[component.first] = component.second.valueNow;
            }
        }

        for (auto& component : model.second.components) {
            UIColorSlider* slider = component.second.valueSlider;
            UILabel* label = component.second.valueLabel;

            auto modelColorMin = modelColorValue;
            auto modelColorMid = modelColorValue;
            auto modelColorMax = modelColorValue;

            modelColorMin[component.first] = component.second.range.first;
            modelColorMid[component.first] = component.second.range.first + (component.second.range.second - component.second.range.first) / 2;
            modelColorMax[component.first] = component.second.range.second;

            double val = modelColorValue[component.first];
            if (model.first != dontUpdate) {
                slider->sliderPos = (val - component.second.range.first) / (component.second.range.second - component.second.range.first);
                label->setText(frmt("{:.2f}", val));
                component.second.valueNow = val;
            }
            slider->colors = { component.first == "H" ? 0x80000000 : mptr->toRGB(modelColorMin),
                              component.first == "H" ? 0x80000000 : mptr->toRGB(modelColorMid),
                              component.first == "H" ? 0x80000000 : mptr->toRGB(modelColorMax) };
        }
    }
}

void UIColorPicker::updateRGBTextBoxOnInputEvent(std::string data, uint8_t* value)
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
            colorUpdatedFromRGBTextBoxes();
        }
    }
    catch (std::exception&) {

    }
}

void UIColorPicker::updateHSVTextBoxOnInputEvent(std::string data, double* value)
{
    try {
        double val = std::stod(data);
        if ((value != &currentH && val >= 0 && val <= 1) || (value == &currentH && val >= 0 && val <= 360)) {
            *value = val;
            colorUpdatedFromHSVTextBoxes();
        }
    }
    catch (std::exception&) {

    }
}

void UIColorPicker::colorUpdatedFromVisualHSV()
{
    colorUpdatedHSV(hsv{ currentH, currentS, currentV }, VISUAL_HSV);
}

void UIColorPicker::colorUpdatedFromHSVSliders()
{
    colorUpdatedHSV(hsv{ currentH, currentS, currentV }, SLIDERS_HSV_SLIDER);
}

void UIColorPicker::colorUpdatedFromHSVTextBoxes()
{;
    colorUpdatedHSV(hsv{ currentH, currentS, currentV }, SLIDERS_HSV_TBOX);
}

void UIColorPicker::colorUpdatedFromRGBSliders()
{
    colorUpdatedRGB({ currentR, currentG, currentB }, SLIDERS_RGB_SLIDER);
}

void UIColorPicker::colorUpdatedFromRGBTextBoxes()
{
    colorUpdatedRGB({ currentR, currentG, currentB }, SLIDERS_RGB_TBOX);
}

void UIColorPicker::setColorHSV(double h, double s, double v)
{
    colorUpdatedHSV(hsv{h,s,v}, COLORCHANGE_EXTERNAL);
}

void UIColorPicker::setColorRGB(u32 col)
{
    colorUpdatedRGB(SDL_Color{ (uint8_t)((col >> 16) & 0xff), (uint8_t)((col >> 8) & 0xff), (uint8_t)(col & 0xff) }, COLORCHANGE_EXTERNAL);
}

void UIColorPicker::colorUpdatedRGB(SDL_Color col, ColorChangeSource from, std::string dontUpdateThisColorModel)
{
    hsv a = rgb2hsv(rgb{ col.r / 255.0f, col.g / 255.0f, col.b / 255.0f });
    colorNowU32 = sdlcolorToUint32(col) | 0xFF000000; //(0xFF << 24) + (col.r << 16) + (col.g << 8) + col.b;

    currentH = (float)a.h;
    currentS = (float)a.s;
    currentV = (float)a.v;
    currentR = col.r;
    currentG = col.g;
    currentB = col.b;

    if (onColorChangedCallback) {
        onColorChangedCallback(this, colorNowU32);
    }

    updateUIFrom(from, dontUpdateThisColorModel);
}

void UIColorPicker::colorUpdatedHSV(hsv col, ColorChangeSource from, std::string dontUpdateThisColorModel)
{
    rgb a = hsv2rgb(col);
    SDL_Color b = rgb2sdlcolor(a);
    colorNowU32 = sdlcolorToUint32(b);

    currentH = col.h;
    currentS = col.s;
    currentV = col.v;
    currentR = b.r;
    currentG = b.g;
    currentB = b.b;

    if (onColorChangedCallback) {
        onColorChangedCallback(this, colorNowU32);
    }

    updateUIFrom(from, dontUpdateThisColorModel);
}

void UIColorPicker::updateUIFrom(ColorChangeSource from, std::string dontUpdateThisColorModel)
{
    if (from != SLIDERS_HSV_SLIDER) {
        updateSliderTabHSVSliders();
    }

    if (from != VISUAL_HSV) {
        updateVisualTabHSVPicker();
    }

    if (from != SLIDERS_RGB_SLIDER) {
        updateSliderTabRGBSliders();
    }

    if (from != SLIDERS_RGB_TBOX) {
        updateSliderTabRGBTextboxes();
    }

    if (from != SLIDERS_HSV_TBOX) {
        updateSliderTabHSVTextboxes();
    }

    updateAllSliderColors();
    colorTextField->setText(frmt("#{:02X}{:02X}{:02X}", currentR, currentG, currentB), false);
    updateColorModelSliders(dontUpdateThisColorModel);
}

void UIColorPicker::updateSliderTabRGBTextboxes()
{
    txtR->setText(std::to_string(currentR));
    txtG->setText(std::to_string(currentG));
    txtB->setText(std::to_string(currentB));
}

void UIColorPicker::updateSliderTabHSVTextboxes()
{
    txtH->setText(std::to_string(currentH));
    txtS->setText(std::to_string(currentS));
    txtV->setText(std::to_string(currentV));
}

void UIColorPicker::updateSliderTabRGBSliders()
{
    sliderR->sliderPos = (float)(currentR / 255.0f);
    sliderG->sliderPos = (float)(currentG / 255.0f);
    sliderB->sliderPos = (float)(currentB / 255.0f);
}

void UIColorPicker::updateVisualTabHSVPicker()
{
    hueSlider->sliderPos = (float)(currentH / 360.0);
    satValSlider->sPos = (float)currentS;
    satValSlider->vPos = (float)currentV;
}

void UIColorPicker::updateSliderTabHSVSliders()
{
    sliderH->sliderPos = (float)(currentH / 360.0f);
    sliderS->sliderPos = (float)currentS;
    sliderV->sliderPos = (float)currentV;
}

void UIColorPicker::updateAllSliderColors()
{
    //hsv
    rgb colorSMin = hsv2rgb(hsv{ currentH, 0, currentV });
    rgb colorSMax = hsv2rgb(hsv{ currentH, 1, currentV });
    rgb colorVMin = hsv2rgb(hsv{ currentH, currentS, 0 });
    rgb colorVMax = hsv2rgb(hsv{ currentH, currentS, 1 });

    std::vector<u32> hueColors = {
        0xFFFF0000,
        0xFFFFFF00,
        0xFF00FF00,
        0xFF00FFFF,
        0xFF0000FF,
        0xFFFF00FF,
        0xFFFF0000
    };
    std::transform(hueColors.begin(), hueColors.end(), hueColors.begin(), [this](u32 col) {
        hsv c = rgb2hsv(u32ToRGB(col));
        c.s = currentS;
        c.v = currentV;
        return sdlcolorToUint32(rgb2sdlcolor(hsv2rgb(c)));
    });

    sliderH->colors = hueColors;

    sliderS->colors = { PackRGBAtoARGB(colorSMin.r * 255, colorSMin.g * 255, colorSMin.b * 255, 255),
                       PackRGBAtoARGB(colorSMax.r * 255, colorSMax.g * 255, colorSMax.b * 255, 255) };

    sliderV->colors = { PackRGBAtoARGB(colorVMin.r * 255, colorVMin.g * 255, colorVMin.b * 255, 255),
                       PackRGBAtoARGB(colorVMax.r * 255, colorVMax.g * 255, colorVMax.b * 255, 255) };

    //rgb
    sliderR->colors = {
       colorNowU32 & 0x00FFFF,
       colorNowU32 | 0xFF0000
    };
    sliderG->colors = {
        colorNowU32 & 0xFF00FF,
        colorNowU32 | 0x00FF00
    };
    sliderB->colors = {
        colorNowU32 & 0xFFFF00,
        colorNowU32 | 0x0000FF
    };
}

#if _WIN32
void UIColorPicker::openOldWindowsColorPicker()
{
    CHOOSECOLOR cc{};
    static COLORREF customColorList[16] = {
        RGB(0, 0, 0),RGB(0, 0, 0),RGB(0, 0, 0),RGB(0, 0, 0),RGB(0, 0, 0),RGB(0, 0, 0),RGB(0, 0, 0),RGB(0, 0, 0),
        RGB(0, 0, 0),RGB(0, 0, 0),RGB(0, 0, 0),RGB(0, 0, 0),RGB(0, 0, 0),RGB(0, 0, 0),RGB(0, 0, 0),RGB(0, 0, 0)
    };
    cc.lStructSize = sizeof(CHOOSECOLOR);
    cc.hwndOwner = (HWND)SDL_GetPointerProperty(SDL_GetWindowProperties(g_wd), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);;
    cc.hInstance = NULL;
    SDL_Color sdlc = uint32ToSDLColor(colorNowU32);
    cc.rgbResult = RGB(sdlc.r, sdlc.g, sdlc.b);
    cc.lpCustColors = customColorList;
    cc.Flags = CC_RGBINIT | CC_FULLOPEN;
    cc.lCustData = 0;
    cc.lpfnHook = NULL;
    cc.lpTemplateName = NULL;

    if (ChooseColor(&cc)) {
        SDL_Color colorbgr = uint32ToSDLColor(cc.rgbResult);
        setColorRGB(PackRGBAtoARGB(colorbgr.b, colorbgr.g, colorbgr.r, 255));
        logprintf("win32 picked color: %x\n", cc.rgbResult);
    }
}
#endif