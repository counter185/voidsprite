#pragma once
#include "globals.h"
#include "drawable.h"
#include "DrawableManager.h"
#include "EventCallbackListener.h"
#include "DraggablePanel.h"
#include "UISlider.h"

class EditorLayerPicker : public DraggablePanel, public EventCallbackListener
{
protected:
	EditorLayerPicker() {};
public:
	MainEditor* caller;
	DrawableManager layerButtons;
	UISlider* opacitySlider = NULL;

	EditorLayerPicker(MainEditor* editor);

	bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override;
	void render(XY position) override;
	void handleInput(SDL_Event evt, XY gPosOffset) override;

	void eventGeneric(int evt_id, int data1, int data2) override;
	void eventButtonPressed(int evt_id) override;
	void eventSliderPosChanged(int evt_id, float value) override;
	void eventSliderPosFinishedChanging(int evt_id, float value) override;

	void updateLayers();
};

