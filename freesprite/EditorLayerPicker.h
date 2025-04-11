#pragma once
#include "globals.h"
#include "drawable.h"
#include "DrawableManager.h"
#include "EventCallbackListener.h"
#include "DraggablePanel.h"
#include "ScrollingPanel.h"

class EditorLayerPicker : public DraggablePanel, public EventCallbackListener
{
protected:
	EditorLayerPicker() {};
public:
	MainEditor* caller = NULL;
	ScrollingPanel* layerListPanel = NULL;
	UISlider* opacitySlider = NULL;

	EditorLayerPicker(MainEditor* editor);

	void render(XY position) override;

	void eventGeneric(int evt_id, int data1, int data2) override;
	void eventSliderPosChanged(int evt_id, float value) override;
	void eventSliderPosFinishedChanging(int evt_id, float value) override;

	void updateLayers();
};

