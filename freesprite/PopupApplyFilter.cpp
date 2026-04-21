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
        FilterPreset newPreset = FilterPreset(targetFilter->id(), params.buildParameterMap());
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
                ParamList list = targetFilter->getParameters();
                list = setDefaultParametersFromPreset(list, preset);
                params = ParameterStore(list);
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
    paramUI = params.generateVerticalUI([this]() {
        threadHasNewParameters = true;
    });
    /*generateParameterUI(&params, [this]() {
        threadHasNewParameters = true;
    });*/
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

void PopupApplyFilter::apply(ParameterStore* parameterMap) {
    auto target = this->target;
    auto session = this->session;
    auto targetFilter = this->targetFilter;
    auto seed = this->filterRandomSeed;
    g_startNewOperation([seed, parameterMap, targetFilter, session, target]() {
        srand(seed);
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
    apply(&params);
    closePopup();
}

void PopupApplyFilter::applyPresetAndClose(FilterPreset preset) {
    ParameterStore newParams = makeParamsFromPreset(preset);
    apply(&newParams);
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

ParameterStore PopupApplyFilter::makeParamsFromPreset(FilterPreset preset) {
    auto paramMap = params.copy();
    params.setParametersFromParameterMap(preset.options);
    return paramMap;
}

void PopupApplyFilter::previewRenderThread()
{
    srand(filterRandomSeed);
    while (previewRenderThreadShouldRun) {
        if (threadHasNewParameters) {
            filterRandomSeed = randomInt(0, 0xFFFF);
            //loginfo(frmt("random seed: {:08x}", filterRandomSeed));
            srand(filterRandomSeed);
            nowRendering = true;
            ParameterStore previewParams = params.copy();
            threadHasNewParameters = false;
            Layer* l = targetFilter->run(target, &previewParams);
            memcpy(previewPixelData, l->pixels32(), 4 * target->w * target->h);
            pixelDataDirty = true;
            delete l;
            nowRendering = false;
        }
        SDL_Delay(1);
    }
}

ParamList PopupApplyFilter::setDefaultParametersFromPreset(ParamList& src, FilterPreset preset)
{
    ParamList copy = src;
    for (Parameter& p : copy) {
        if (preset.options.contains(p.name) 
            || (p.paramType == PT_INT_RANGE && preset.options.contains(p.name+".min") && preset.options.contains(p.name + ".max"))) {
            try {
                switch (p.paramType) {
                    case PT_BOOL:
                        p.vNum = preset.options[p.name] == "1" ? 1 : 0;
                        break;
                    case PT_INT:
                    case PT_COLOR_L:
                        p.vNum = std::stoi(preset.options[p.name]);
                        break;
                    case PT_FLOAT:
                        p.vNum = std::stod(preset.options[p.name]);
                        break;
                    case PT_INT_RANGE:
                        p.vNum = std::stoi(preset.options[p.name + ".min"]);
                        p.vNum2 = std::stoi(preset.options[p.name + ".max"]);
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

std::map<std::string, std::string> PopupApplyFilter::buildParameterMap(ParamList& params)
{
    std::map<std::string, std::string> ret = {};
    for (auto& p : params) {
        switch (p.paramType) {
        case PT_COLOR_RGB:
            ret[p.name] = frmt("{:08X}", p.vU32);
            break;
        case PT_INT_RANGE:
            ret[p.name + ".min"] = std::to_string(p.vNum);
            ret[p.name + ".max"] = std::to_string(p.vNum2);
            break;
        default:
            ret[p.name] = std::to_string(p.vNum);
            break;
        }
    }
    return ret;
}
