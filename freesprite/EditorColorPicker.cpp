#include "globals.h"
#include "FontRenderer.h"
#include "maineditor.h"
#include "EditorColorPicker.h"
#include "mathops.h"
#include "ScrollingPanel.h"

EditorColorPicker::EditorColorPicker(MainEditor* c) {
    caller = c;

    wxWidth = 400;
    wxHeight = 390;

    colorModeTabs = new TabbedView({ { "Colors" },{ "Last" }, { "Palettes"} }, 85);
    colorModeTabs->position = XY{ 20,30 };
    subWidgets.addDrawable(colorModeTabs);

    //-----------------
    //| Colors tab

    colorTabs = new TabbedView({ { "Visual", g_iconColorVisual },{ "Sliders", g_iconColorHSV }, { "Other" }}, 100);
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

    UILabel* labelH = new UILabel();
    UILabel* labelR = new UILabel();
    labelR->text = "R";
    labelH->text = "H";
    labelH->position = XY{ 0, 120 + 10 };
    labelR->position = XY{ 0, 10 };
    colorTabs->tabs[1].wxs.addDrawable(labelH);
    colorTabs->tabs[1].wxs.addDrawable(labelR);

    UILabel* labelS = new UILabel();
    UILabel* labelG = new UILabel();
    labelG->text = "G";
    labelS->text = "S";
    labelS->position = XY{ 0, 120 + 45 };
    labelG->position = XY{ 0, 45 };
    colorTabs->tabs[1].wxs.addDrawable(labelS);
    colorTabs->tabs[1].wxs.addDrawable(labelG);

    UILabel* labelV = new UILabel();
    UILabel* labelB = new UILabel();
    labelB->text = "B";
    labelV->text = "V";
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

    sliderH = new UISlider();
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
    colorModelsPanel->position = XY{0, 5};
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
                cvalueLabel->position = XY{ 30, yNow };
                colorModelsPanel->subWidgets.addDrawable(cvalueLabel);
                UIColorSlider* slider = new UIColorSlider();
                slider->position = XY{ 80, yNow };
                slider->wxWidth = 200;
                slider->wxHeight = 20;
                slider->setCallbackListener(100 + i, this);
                colorModelsPanel->subWidgets.addDrawable(slider);
                yNow += 30;
                components[component] = {slider, cvalueLabel, 0, componentRanges[component]};
            }

            colorModels.push_back({ name, {mptr, components} });
            i++;
        }
    }
    //-----------------
    //| Palettes tab
    ScrollingPanel* palettePanel = new ScrollingPanel();
    palettePanel->position = XY{ 0,5 };
    palettePanel->scrollHorizontally = false;
    palettePanel->scrollVertically = true;
    palettePanel->wxWidth = 370;
    palettePanel->wxHeight = 270;
    int yNow = 5;
    for (auto& p : g_namedColorMap) {
        UILabel* nameLabel = new UILabel(p.name);
        nameLabel->position = XY{5, yNow};
        palettePanel->subWidgets.addDrawable(nameLabel);
        yNow += 30;
        int xNow = 0;
        for (auto& color : p.colorMap) {
            if (color.first == HINT_NEXT_LINE) {
                xNow = 0;
                yNow += 20;
                continue;
            }

            ColorPickerColorButton* b = new ColorPickerColorButton(this, color.second);
            b->position = XY{xNow, yNow};
            b->tooltip = color.first;
            b->wxWidth = 24;
            b->wxHeight = 20;
            palettePanel->subWidgets.addDrawable(b);
            xNow += b->wxWidth;
            if (xNow + b->wxWidth >= palettePanel->wxWidth) {
                xNow = 0;
                yNow += b->wxHeight;
            }
        }
        yNow += 30;
    }
    colorModeTabs->tabs[2].wxs.addDrawable(palettePanel);


    //widgets outside of tabs

    colorTextField = new UITextField();
    colorTextField->isColorField = true;
    colorTextField->position = XY{ 60, 350 };
    colorTextField->wxWidth = 140;
    colorTextField->tooltip = "Enter the color here in #RRGGBB format.\nPredefined color names are also supported.";
    colorTextField->setCallbackListener(EVENT_COLORPICKER_TEXTFIELD, this);
    subWidgets.addDrawable(colorTextField);

    eraserButton = new UIButton();
    eraserButton->position = { 20, 350 };
    //eraserButton->text = "E";
    eraserButton->icon = g_iconEraser;
    eraserButton->wxWidth = 30;
    eraserButton->tooltip = "Eraser";
    eraserButton->setCallbackListener(EVENT_COLORPICKER_TOGGLEERASER, this);
    subWidgets.addDrawable(eraserButton);

    blendModeButton = new UIButton();
    blendModeButton->position = { 215, 350 };
    blendModeButton->icon = g_iconBlendMode;
    blendModeButton->wxWidth = 30;
    blendModeButton->tooltip = "Alpha blend";
    blendModeButton->setCallbackListener(EVENT_COLORPICKER_TOGGLEBLENDMODE, this);
    subWidgets.addDrawable(blendModeButton);

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
        default:
            if (evt_id >= 100 && (evt_id - 100) < colorModels.size()) {
                int modelid = evt_id - 100;
                auto& model = colorModels[modelid];
                std::string modelName = model.first;
                ColorModelData& modelData = model.second;
                ColorModel* mptr = modelData.targetModel;
                std::map<std::string, double> componentData;
                for (auto& component : modelData.components) {
                    component.second.valueNow = component.second.range.first + component.second.valueSlider->sliderPos * (component.second.range.second - component.second.range.first);
                    component.second.valueLabel->text = std::format("{:.2f}", component.second.valueNow);
                    componentData[component.first] = component.second.valueNow;
                }
                
                u32 color = mptr->toRGB(componentData);
                setMainEditorColorRGB(uint32ToSDLColor(color), true, true, true, modelName);
            }
            break;
    }
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
    setMainEditorColorRGB({currentR, currentG, currentB}, true, false, true);
}

void EditorColorPicker::updateColorModelSliders(std::string dontUpdate)
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
                label->text = std::format("{:.2f}", val);
                component.second.valueNow = val;
            }
            slider->colors = {component.first == "H" ? 0x80000000 : mptr->toRGB(modelColorMin),
                              component.first == "H" ? 0x80000000 : mptr->toRGB(modelColorMid),
                              component.first == "H" ? 0x80000000 : mptr->toRGB(modelColorMax)};
        }
    }
}

void EditorColorPicker::updateMainEditorColorFromRGBTextBoxes()
{
    setMainEditorColorRGB({currentR, currentG, currentB}, true, true, true);
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
void EditorColorPicker::setMainEditorColorRGB(SDL_Color col, bool updateHSVSliders, bool updateRGBSliders, bool updateHSVTextBoxes, std::string dontUpdateThisColorModel) {
    hsv a = rgb2hsv(rgb{ col.r / 255.0f, col.g / 255.0f, col.b / 255.0f });
    if (updateHSVSliders) {
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
    sliderS->colors = {PackRGBAtoARGB(colorSMin.r * 255, colorSMin.g * 255, colorSMin.b * 255, 255),
                       PackRGBAtoARGB(colorSMax.r * 255, colorSMax.g * 255, colorSMax.b * 255, 255)};

    sliderV->colors = {PackRGBAtoARGB(colorVMin.r * 255, colorVMin.g * 255, colorVMin.b * 255, 255),
                       PackRGBAtoARGB(colorVMax.r * 255, colorVMax.g * 255, colorVMax.b * 255, 255)};

    txtR->setText(std::to_string(currentR));
    txtG->setText(std::to_string(currentG));
    txtB->setText(std::to_string(currentB));
    if (updateHSVTextBoxes) {
        txtH->setText(std::to_string(currentH));
        txtS->setText(std::to_string(currentS));
        txtV->setText(std::to_string(currentV));
    }

    uint32_t rgbColor = sdlcolorToUint32(col) | 0xFF000000; //(0xFF << 24) + (col.r << 16) + (col.g << 8) + col.b;

    sliderR->colors = {
        rgbColor & 0x00FFFF,
        rgbColor | 0xFF0000
    };
    sliderG->colors = {
        rgbColor & 0xFF00FF, 
        rgbColor | 0x00FF00
    };
    sliderB->colors = {
        rgbColor & 0xFFFF00, 
        rgbColor | 0x0000FF
    };

    colorTextField->setText(std::format("#{:02X}{:02X}{:02X}", col.r, col.g, col.b));
    caller->pickedColor = rgbColor;

    updateColorModelSliders(dontUpdateThisColorModel);
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

ColorPickerColorButton::ColorPickerColorButton(EditorColorPicker* parent, u32 color) : UIButton()
{
    this->parent = parent;
    this->color = color;
    this->fill = Fill::Solid(color);
}

void ColorPickerColorButton::click()
{
    UIButton::click();
    parent->setMainEditorColorRGB(color);
}
