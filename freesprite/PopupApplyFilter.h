#pragma once
#include "BasePopup.h"
#include "EventCallbackListener.h"
#include "BaseFilter.h"

class PopupApplyFilter :
    public BasePopup, public EventCallbackListener
{
protected:
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
    }

    void render() override {
        renderDefaultBackground();
        BasePopup::render();
    }
    void defaultInputAction(SDL_Event evt) override;

    void eventButtonPressed(int evt_id) override;
    void eventSliderPosChanged(int evt_id, float value) override;

    void setupWidgets();
    void applyAndClose();
};

