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
    std::vector<UILabel*> paramLabels;
public:
    PopupApplyFilter(MainEditor* session, Layer* target, BaseFilter* targetFilter) {
        this->session = session;
        this->target = target;
        this->targetFilter = targetFilter;

        setupWidgets();
    }

    void render() override {
        renderFilterPopupBackground();
        BasePopup::render();
    }
    void defaultInputAction(SDL_Event evt) override;
    XY getPopupOrigin() override {
        return XY{ 20, BasePopup::getPopupOrigin().y };
    }

    void eventButtonPressed(int evt_id) override;
    void eventSliderPosChanged(int evt_id, float value) override;

    void renderFilterPopupBackground();
    void setupWidgets();
    void applyAndClose();
    void updateLabels();
};

