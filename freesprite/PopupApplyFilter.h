#pragma once
#include <thread>

#include "BasePopup.h"
#include "EventCallbackListener.h"
#include "BaseFilter.h"

class PopupApplyFilter :
    public BasePopup, public EventCallbackListener
{
protected:
    u8* previewPixelData = NULL;
    std::atomic<bool> pixelDataDirty = true;
    SDL_Texture* previewTexture = NULL;
    std::thread previewRenderThreadObj;
    std::atomic<bool> previewRenderThreadShouldRun = true;
    std::atomic<bool> threadHasNewParameters = true;
    std::atomic<bool> nowRendering = false;

    MainEditor* session;
    Layer* target;
    BaseFilter* targetFilter;
    std::vector<FilterParameter> params;
    std::vector<UILabel*> paramLabels;
public:
    PopupApplyFilter(MainEditor* session, Layer* target, BaseFilter* targetFilter) {
        this->session = session;
        this->target = target;
        this->targetFilter = targetFilter;

        setupWidgets();
        setupPreview();
    }
    ~PopupApplyFilter();

    void render() override;
    void defaultInputAction(SDL_Event evt) override;
    XY getPopupOrigin() override {
        return XY{ 20, BasePopup::getPopupOrigin().y };
    }

    void eventButtonPressed(int evt_id) override;
    void eventSliderPosChanged(int evt_id, float value) override;
    void eventDoubleSliderPosChanged(int evt_id, UIDoubleSliderBounds value) override;
    void eventCheckboxToggled(int evt_id, bool newState) override;

    void renderFilterPopupBackground();
    void setupWidgets();
    void applyAndClose();
    void updateLabels();
    void setupPreview();
    void updatePreview();
    std::map<std::string, std::string> makeParameterMap();

    void previewRenderThread();
};

