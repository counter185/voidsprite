#pragma once
#include "drawable.h"
#include "mathops.h"
#include "EventCallbackListener.h"

class UISlider : public Drawable
{
public:
    float sliderPos = 0.0f;

    int wxWidth = 250, wxHeight = 40;
    bool verticalSlider = false;
    bool mouseHeld = false;

    Fill backgroundFill = Fill::None();

    std::function<void(UISlider*, float)> onChangeValueCallback = NULL;
    std::function<void(UISlider*, float)> onChangeValueFinishedCallback = NULL;

    UISlider();

    void drawPosIndicator(XY origin);

    bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override {
        return pointInBox(mousePos, SDL_Rect{ thisPositionOnScreen.x, thisPositionOnScreen.y, wxWidth, wxHeight });
    }
    void render(XY pos) override;
    void handleInput(SDL_Event evt, XY gPosOffset) override;
    XY getDimensions() override { return XY{ wxWidth, wxHeight }; }
    XY getRenderDimensions() override { return xyAdd(getDimensions(), { 0, 3 }); }
    void focusOut() override {
        Drawable::focusOut();
        mouseHeld = false;
    }

    virtual double getValue(double min, double max) {
        return min + (max - min) * sliderPos;
    }
    virtual void setValue(double min, double max, double val) {
        sliderPos = (val - min) / (max - min);
    }

    virtual void onSliderPosChanged() {
        if (onChangeValueCallback != NULL) {
            onChangeValueCallback(this, sliderPos);
        }
        if (callback != NULL) {
            callback->eventSliderPosChanged(callback_id, sliderPos);
        }
    }
    virtual void onSliderPosFinishedChanging() {
        if (onChangeValueFinishedCallback != NULL) {
            onChangeValueFinishedCallback(this, sliderPos);
        }
        if (callback != NULL) {
            callback->eventSliderPosFinishedChanging(callback_id, sliderPos);
        }
    }

};

