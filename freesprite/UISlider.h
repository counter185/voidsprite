#pragma once
#include "drawable.h"
#include "mathops.h"
#include "EventCallbackListener.h"

class UISlider : public Drawable
{
public:
	float sliderPos = 0.0f;

	int wxWidth = 250, wxHeight = 40;
	bool mouseHeld = false;

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

	virtual void onSliderPosChanged() {
		if (callback != NULL) {
			callback->eventSliderPosChanged(callback_id, sliderPos);
		}
	}
	virtual void onSliderPosFinishedChanging() {
		if (callback != NULL) {
			callback->eventSliderPosFinishedChanging(callback_id, sliderPos);
		}
	}

};

