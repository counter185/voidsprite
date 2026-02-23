#pragma once
#include <thread>

#include "BasePopup.h"
#include "BaseFilter.h"
#include "EventCallbackListener.h"

class PopupApplyFilter :
    public BasePopup, public EventCallbackListener
{
protected:
    bool sessionDragging = false;

    u8* previewPixelData = NULL;
    std::atomic<bool> pixelDataDirty = true;
    SDL_Texture* previewTexture = NULL;
    std::thread previewRenderThreadObj;
    std::atomic<bool> previewRenderThreadShouldRun = true;
    std::atomic<bool> threadHasNewParameters = true;
    std::atomic<bool> nowRendering = false;

    Panel* paramUI = NULL;
    MainEditor* session;
    Layer* target;
    BaseFilter* targetFilter;
    std::vector<FilterParameter> params;
public:
    PopupApplyFilter(MainEditor* session, Layer* target, BaseFilter* targetFilter) {
        this->session = session;
        this->target = target;
        this->targetFilter = targetFilter;

        setupWidgets();
        setupPreview();
    }
    ~PopupApplyFilter();

    void eventFileSaved(int evt_id, PlatformNativePathString name, int exporterIndex = -1) override;
    void eventFileOpen(int evt_id, PlatformNativePathString name, int importerIndex = -1) override;

    void render() override;
    void defaultInputAction(SDL_Event evt) override;
    XY getPopupOrigin() override {
        return XY{ 20, BasePopup::getPopupOrigin().y };
    }

    void renderDefaultBackground() override;
    void regenParameterUI();
    void setupWidgets();
    void apply(std::map<std::string, std::string> parameterMap);
    void applyAndClose();
    void applyPresetAndClose(FilterPreset preset);
    void setupPreview();
    void updatePreview();
    std::map<std::string, std::string> makeParameterMap();
    std::map<std::string, std::string> makeParameterMapFromPreset(FilterPreset preset);

    void previewRenderThread();

    static std::vector<FilterParameter> setDefaultParametersFromPreset(std::vector<FilterParameter>& src, FilterPreset preset);
    static Panel* generateParameterUI(std::vector<FilterParameter>* params, std::function<void()> valuesChangedCallback);
    static std::map<std::string, std::string> buildParameterMap(std::vector<FilterParameter>& params);
};

