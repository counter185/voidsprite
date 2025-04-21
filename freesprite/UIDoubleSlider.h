#pragma once
#include "globals.h"
#include "drawable.h"
#include "mathops.h"
#include "EventCallbackListener.h"

class UIDoubleSlider : public Drawable
{
public:
    UIDoubleSliderBounds sliderPos = {0.0, 0.0};

    int wxWidth = 250, wxHeight = 40;
    bool mouseHeld = false;
    bool focusMost = false;

    u32 bodyColor;

    std::function<void(UIDoubleSlider*, UIDoubleSliderBounds)> onChangeValueCallback = NULL;
    std::function<void(UIDoubleSlider*, UIDoubleSliderBounds)> onChangeValueFinishedCallback = NULL;

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

    UIDoubleSliderBounds getBounds(double min, double max) {
        return {
            .min = (sliderPos.min * (max - min)) + min,
            .max = (sliderPos.max * (max - min)) + min,
        };
    }

    void setBounds(double min, double max, UIDoubleSliderBounds bounds) {
        sliderPos.min = (bounds.min - min) / (max - min);
        sliderPos.max = (bounds.max - min) / (max - min);
        if (sliderPos.max < sliderPos.min) sliderPos.min = sliderPos.max;
    }

    void setBoundMin(double min, double max, double val) {
        sliderPos.min = (val - min) / (max - min);
    }
    void setBoundMax(double min, double max, double val) {
        sliderPos.max = (val - min) / (max - min);
    }

    void onSliderPosChanged() {
        if (onChangeValueCallback != NULL) {
            onChangeValueCallback(this, sliderPos);
        }
        if (callback != NULL) {
            callback->eventDoubleSliderPosChanged(callback_id, sliderPos);
        }
    }
    void onSliderPosFinishedChanging() {
        if (onChangeValueFinishedCallback != NULL) {
            onChangeValueFinishedCallback(this, sliderPos);
        }
        if (callback != NULL) {
            callback->eventDoubleSliderPosFinishedChanging(callback_id, sliderPos);
        }
    }
};
