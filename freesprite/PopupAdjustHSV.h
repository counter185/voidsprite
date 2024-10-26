#pragma once
#include "BasePopup.h"
#include "EventCallbackListener.h"

class PopupAdjustHSV :
    public BasePopup, public EventCallbackListener
{
public:
    std::string title = "";
    std::string text = "";

    double adjH = 0;
    double adjS = 0;
    double adjV = 0;

    PopupAdjustHSV(std::string tt, std::string tx, bool acceptAlpha = false);

    void render() override;

    void eventButtonPressed(int evt_id) override;
    void eventTextInput(int evt_id, std::string data) override;
    void eventSliderPosChanged(int evt_id, float value) override;

    void updateSliderValues();
private:
    UISlider* hSlider, *sSlider, *vSlider;
};

