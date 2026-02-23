#include <fstream>

#include "PopupApplyFilter.h"
#include "BaseFilter.h"
#include "EventCallbackListener.h"
#include "UILabel.h"
#include "UISlider.h"
#include "UIDoubleSlider.h"
#include "UIButton.h"
#include "UIColorInputField.h"
#include "UICheckbox.h"
#include "maineditor.h"
#include "background_operation.h"
#include "Notification.h"
#include "FontRenderer.h"

PopupApplyFilter::~PopupApplyFilter() {
    if (previewRenderThreadObj.joinable()) {
        previewRenderThreadShouldRun = false;
        previewRenderThreadObj.join();
    }
    if (previewPixelData != NULL) {
        tracked_free(previewPixelData);
    }
    if (previewTexture != NULL) {
        target->effectPreviewTexture = NULL;
        tracked_destroyTexture(previewTexture);
    }
}

void PopupApplyFilter::eventFileSaved(int evt_id, PlatformNativePathString name, int exporterIndex) {
    if (evt_id == EVENT_APPLYFILTER_SAVEPRESET) {
        FilterPreset newPreset = FilterPreset(targetFilter->id(), buildParameterMap(params));
        std::string serialized = newPreset.serialize();
        std::ofstream outFile(convertStringToUTF8OnWin32(name));
        if (outFile.is_open()) {
            outFile << serialized;
            outFile.close();
            g_addNotification(SuccessShortNotification("Success", "Filter preset saved"));
        }
        else {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Failed to save filter preset"));
        }
    }
}

void PopupApplyFilter::eventFileOpen(int evt_id, PlatformNativePathString name, int importerIndex) {
    if (evt_id == EVENT_APPLYFILTER_LOADPRESET) {
        std::ifstream inFile(convertStringToUTF8OnWin32(name));
        if (inFile.is_open()) {
            std::string line;
            std::string serialized;
            while (std::getline(inFile, line)) {
                serialized += line;  //it's json so it shouldn't matter
            }
            inFile.close();
            try {
                FilterPreset preset = FilterPreset::deserialize(serialized);
                params = targetFilter->getParameters();
                params = setDefaultParametersFromPreset(params, preset);
                regenParameterUI();
                threadHasNewParameters = true;

                //applyPresetAndClose(preset);
                
            } catch (std::exception& e) {
                logerr(frmt("preset load failed:\n {}", e.what()));
                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Failed to parse filter preset"));
                return;
            }
        }
        else {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Failed to load filter preset"));
        }
    }
}

void PopupApplyFilter::render() {
    updatePreview();
    BasePopup::render();
    if (nowRendering) {
        g_fnt->RenderString(TL("vsp.applyfilter.renderingpreview"), 2, g_windowH - 30, {255,255,255,200});
    }
}

void PopupApplyFilter::defaultInputAction(SDL_Event evt)
{
    XY origin = getPopupOrigin();
    evt = convertTouchToMouseEvent(evt);
    if (evt.type == SDL_EVENT_MOUSE_BUTTON_DOWN && !pointInBox(XY{(int)evt.button.x, (int)evt.button.y}, {origin.x, origin.y, wxWidth, wxHeight})) {
        sessionDragging = true;
    }
    else if (evt.type == SDL_EVENT_MOUSE_BUTTON_UP) {
        sessionDragging = false;
    }
    else if (evt.type == SDL_EVENT_MOUSE_MOTION && sessionDragging) {
        session->canvas.panCanvas(XY{(int)evt.motion.xrel, (int)evt.motion.yrel});
    }
    else if (evt.type == SDL_EVENT_MOUSE_WHEEL) {
        session->canvas.zoomFromWheelInput(evt.wheel.y);
    }
}

void PopupApplyFilter::renderDefaultBackground()
{
    SDL_Color bgColor = SDL_Color{ 0,0,0,0xD0 };

    int topBarY = g_windowH / 5;
    int bottomBarY = g_windowH / 5 * 4;

    SDL_SetRenderDrawColor(g_rd, 0, 0, 0, (uint8_t)(0x60 * startTimer.percentElapsedTime(300)));
    //SDL_RenderFillRect(g_rd, NULL);
    
    renderGradient({ 0,0,wxWidth + 40,g_windowH }, 0x70000000, 0x70000000, 0xFF000000, 0xFF000000);
    //renderGradient({ 0,g_windowH / 2,g_windowW,g_windowH / 2 }, 0xFF000000, 0xFF000000, 0x70000000, 0x70000000);
}

void PopupApplyFilter::regenParameterUI() {
    if (paramUI != NULL) {
        wxsManager.removeDrawable(paramUI);
    }
    paramUI = generateParameterUI(&params, [this]() {
        threadHasNewParameters = true;
    });
    paramUI->position = { 0, 50 };
    wxsManager.addDrawable(paramUI);
}

void PopupApplyFilter::setupWidgets()
{
    makeTitleAndDesc(targetFilter->name(), "");

#if _DEBUG
    UILabel* title = new UILabel(targetFilter->id());
    title->fontsize = 14;
    title->color = { 255,255,255,0x80 };
    title->position = XY{10, 30};
    wxsManager.addDrawable(title);
#endif

    params = targetFilter->getParameters();

    regenParameterUI();

    setSize({ 550, 50 + paramUI->getContentBoxSize().y + 70});

    actionButton(TL("vsp.cmn.cancel"))->onClickCallback = [this](UIButton* b) { closePopup(); };
    actionButton(TL("vsp.cmn.apply"))->onClickCallback = [this](UIButton* b) { applyAndClose(); };
    actionButton("Load preset")->onClickCallback = [this](UIButton* b) { 
        platformTryLoadOtherFile(this, {{".voidfpreset", "Filter preset"}}, TL("vsp.popup.loadfilterpreset"), EVENT_APPLYFILTER_LOADPRESET);
    };
    actionButton("Save preset")->onClickCallback = [this](UIButton* b) { 
        platformTrySaveOtherFile(this, {{".voidfpreset", "Filter preset"}}, TL("vsp.popup.savefilterpreset"), EVENT_APPLYFILTER_SAVEPRESET);
    };
}

void PopupApplyFilter::apply(std::map<std::string, std::string> parameterMap) {
    auto target = this->target;
    auto session = this->session;
    auto targetFilter = this->targetFilter;
    g_startNewOperation([parameterMap, targetFilter, session, target]() {
        Layer* copy = targetFilter->run(target, parameterMap);
        session->commitStateToCurrentLayer();
        if (session->isolateEnabled) {
            u32* srcpx = copy->pixels32();
            u32* dstpx = target->pixels32();
            session->isolatedFragment.forEachPoint([&](XY a) {
                if (pointInBox(a, { 0,0,target->w,target->h })) {
                    ARRAY2DPOINT(dstpx, a.x, a.y, target->w) = ARRAY2DPOINT(srcpx, a.x, a.y, target->w);
                }
            });
        }
        else {
            memcpy(target->pixels32(), copy->pixels32(), 4 * target->w * target->h);
        }
        target->markLayerDirty();
        delete copy;
    });
}

void PopupApplyFilter::applyAndClose()
{
    apply(makeParameterMap());
    closePopup();
}

void PopupApplyFilter::applyPresetAndClose(FilterPreset preset) {
    apply(makeParameterMapFromPreset(preset));
    closePopup();
}

void PopupApplyFilter::setupPreview()
{
//can't have threads on emscripten
#if VSP_PLATFORM != VSP_PLATFORM_EMSCRIPTEN
    previewPixelData = (u8*)tracked_malloc(4 * target->w * target->h);
    previewTexture = tracked_createTexture(g_rd, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, target->w, target->h);
    SDL_SetTextureBlendMode(previewTexture, SDL_BLENDMODE_BLEND);
    target->effectPreviewTexture = previewTexture;

    previewRenderThreadObj = std::thread(&PopupApplyFilter::previewRenderThread, this);
#endif
}

void PopupApplyFilter::updatePreview()
{
#if VSP_PLATFORM != VSP_PLATFORM_EMSCRIPTEN
    if (pixelDataDirty) {
        u8* ppx;
        int pitch;
        SDL_LockTexture(previewTexture, NULL, (void**)&ppx, &pitch);
        u32* previewPx = (u32*)previewPixelData;
        if (session->isolateEnabled) {
            copyPixelsToTexture(target->pixels32(), target->w, target->h, ppx, pitch);
            session->isolatedFragment.forEachPoint([&](XY a) {
                if (pointInBox(a, { 0,0,target->w,target->h })) {
                    *(u32*)(&ARRAY2DPOINT(ppx, a.x * 4, a.y, pitch)) = ARRAY2DPOINT(previewPx, a.x, a.y, target->w);
                }
            });
        }
        else {
            copyPixelsToTexture((u32*)previewPixelData, target->w, target->h, ppx, pitch);
        }
        SDL_UnlockTexture(previewTexture);
        pixelDataDirty = false;
    }
#endif
}

std::map<std::string, std::string> PopupApplyFilter::makeParameterMap()
{
    std::map<std::string, std::string> parameterMap = buildParameterMap(params);
    
    parameterMap["!editor:activecolor"] = std::to_string(session->getActiveColor());
    return parameterMap;
}

std::map<std::string, std::string> PopupApplyFilter::makeParameterMapFromPreset(FilterPreset preset) {
    auto paramMap = makeParameterMap();
    for (auto& [k, v] : preset.options) {
        paramMap[k] = v;
    }
    return paramMap;
}

void PopupApplyFilter::previewRenderThread()
{
    while (previewRenderThreadShouldRun) {
        if (threadHasNewParameters) {
            nowRendering = true;
            std::map<std::string, std::string> parameterMap = makeParameterMap();
            threadHasNewParameters = false;
            Layer* l = targetFilter->run(target, parameterMap);
            memcpy(previewPixelData, l->pixels32(), 4 * target->w * target->h);
            pixelDataDirty = true;
            delete l;
            nowRendering = false;
        }
        SDL_Delay(1);
    }
}

std::vector<FilterParameter> PopupApplyFilter::setDefaultParametersFromPreset(std::vector<FilterParameter>& src, FilterPreset preset)
{
    std::vector<FilterParameter> copy = src;
    for (FilterParameter& p : copy) {
        if (preset.options.contains(p.name) 
            || (p.paramType == PT_INT_RANGE && preset.options.contains(p.name+".min") && preset.options.contains(p.name + ".max"))) {
            try {
                switch (p.paramType) {
                    case PT_BOOL:
                        p.defaultValue = preset.options[p.name] == "1" ? 1 : 0;
                        break;
                    case PT_INT:
                    case PT_COLOR_L:
                        p.defaultValue = std::stoi(preset.options[p.name]);
                        break;
                    case PT_FLOAT:
                        p.defaultValue = std::stod(preset.options[p.name]);
                        break;
                    case PT_INT_RANGE:
                        p.defaultValue = std::stoi(preset.options[p.name + ".min"]);
                        p.defaultValueTwo = std::stoi(preset.options[p.name + ".max"]);
                        break;
                    case PT_COLOR_RGB:
                        p.vU32 = std::stoi(preset.options[p.name], NULL, 16);
                        break;
                }
            }
            catch (std::exception&) {
                
            }
        }
    }
    return copy;
}

Panel* PopupApplyFilter::generateParameterUI(std::vector<FilterParameter>* params, std::function<void()> valuesChangedCallback)
{
    auto updateLabelFn = [](FilterParameter& p, UILabel* l) {
        switch (p.paramType) {
            case PT_BOOL:
                break;
            case PT_INT:
            case PT_COLOR_L:
                l->setText(std::to_string((int)p.defaultValue));
                break;
            case PT_FLOAT:
                l->setText(frmt("{:.1f}", p.defaultValue));
                break;
            case PT_INT_RANGE:
                l->setText(std::to_string((int)p.defaultValue) + ":" + std::to_string((int)p.defaultValueTwo));
                break;
        }
    };

    Panel* panel = new Panel();
    panel->sizeToContent = true;
    panel->passThroughMouse = true;

    int y = 0;
    int i = 0;
    for (auto& p : *params) {
        UILabel* label = new UILabel(p.name);
        label->position = XY{ 10, y + 2 };
        panel->subWidgets.addDrawable(label);

        switch (p.paramType) {
        case PT_BOOL:
        {
            UICheckbox* checkbox = new UICheckbox("", p.defaultValue == 1);
            checkbox->position = XY{ 250, y };
            checkbox->onStateChangeCallback = [params, i, valuesChangedCallback](UICheckbox* c, bool v) {
                params->at(i).defaultValue = v ? 1 : 0;
                valuesChangedCallback();
            };
            panel->subWidgets.addDrawable(checkbox);
        }
        break;
        case PT_COLOR_L:
        case PT_INT:
        case PT_FLOAT:
        {
            float v = (p.defaultValue - p.minValue) / (p.maxValue - p.minValue);
            UISlider* slider = new UISlider();
            slider->position = XY{ 285, y };
            slider->sliderPos = v;
            slider->wxHeight = 25;
            if (p.paramType == PT_COLOR_L) {
                slider->backgroundFill = Fill::Gradient(0xA0000000, 0xA0FFFFFF, 0xA0000000, 0xA0FFFFFF);
            }
            panel->subWidgets.addDrawable(slider);

            UILabel* valueLabel = new UILabel();
            valueLabel->position = xySubtract(slider->position, { 84, 0 });
            panel->subWidgets.addDrawable(valueLabel);

            slider->onChangeValueCallback = [params, i, valueLabel, valuesChangedCallback, updateLabelFn](UISlider* s, float v) {
                FilterParameter& p = params->at(i);
                switch (p.paramType) {
                    case PT_INT:
                        p.defaultValue = (int)(p.minValue + (p.maxValue - p.minValue) * v);
                        break;
                    case PT_COLOR_L:
                        p.defaultValue = (int)(255 * v);
                        break;
                    case PT_FLOAT:
                        p.defaultValue = p.minValue + (p.maxValue - p.minValue) * v;
                        break;
                    default: break;
                }
                updateLabelFn(p, valueLabel);
                valuesChangedCallback();
            };
            updateLabelFn(p, valueLabel);
        }
        break;
        case PT_COLOR_RGB:
        {
            UIColorInputField* colorInput = new UIColorInputField();
            colorInput->position = XY{ 250, y };
            colorInput->wxHeight = 25;
            colorInput->setColor(p.vU32);
            panel->subWidgets.addDrawable(colorInput);
            colorInput->onColorChangedCallback = [params, i, valuesChangedCallback](UIColorInputField* p, u32 c) {
                params->at(i).vU32 = c;
                valuesChangedCallback();
            };
        }
        break;
        case PT_INT_RANGE:
        {
            float vl = (p.defaultValue - p.minValue) / (p.maxValue - p.minValue);
            float vm = (p.defaultValueTwo - p.minValue) / (p.maxValue - p.minValue);
            UIDoubleSlider* slider = new UIDoubleSlider();
            slider->position = XY{ 285, y };
            slider->sliderPos.min = vl;
            slider->sliderPos.max = vm;
            slider->bodyColor = p.vU32;
            slider->wxHeight = 25;
            panel->subWidgets.addDrawable(slider);

            UILabel* valueLabel = new UILabel();
            valueLabel->position = xySubtract(slider->position, { 84, 0 });
            panel->subWidgets.addDrawable(valueLabel);

            slider->onChangeValueCallback = [params, i, valueLabel, valuesChangedCallback, updateLabelFn](UIDoubleSlider* s, UIDoubleSliderBounds v) {
                FilterParameter& p = params->at(i);
                p.defaultValue = (int)(p.minValue + (p.maxValue - p.minValue) * v.min);
                p.defaultValueTwo = (int)(p.minValue + (p.maxValue - p.minValue) * v.max);
                updateLabelFn(p, valueLabel);
                valuesChangedCallback();
            };
            updateLabelFn(p, valueLabel);
        }
        break;
        }
        y += 40;
        i++;
    }
    return panel;
}

std::map<std::string, std::string> PopupApplyFilter::buildParameterMap(std::vector<FilterParameter>& params)
{
    std::map<std::string, std::string> ret = {};
    for (auto& p : params) {
        switch (p.paramType) {
        case PT_COLOR_RGB:
            ret[p.name] = frmt("{:08X}", p.vU32);
            break;
        case PT_INT_RANGE:
            ret[p.name + ".min"] = std::to_string(p.defaultValue);
            ret[p.name + ".max"] = std::to_string(p.defaultValueTwo);
            break;
        default:
            ret[p.name] = std::to_string(p.defaultValue);
            break;
        }
    }
    return ret;
}
