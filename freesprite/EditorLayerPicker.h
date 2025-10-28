#pragma once
#include "globals.h"
#include "EventCallbackListener.h"
#include "PanelUserInteractable.h"

class EditorLayerPicker : public PanelUserInteractable, public EventCallbackListener
{
protected:
	EditorLayerPicker() {};
public:
	MainEditor* caller = NULL;
	ScrollingPanel* layerListPanel = NULL;
	UISlider* opacitySlider = NULL;

	EditorLayerPicker(MainEditor* editor);

	void eventGeneric(int evt_id, int data1, int data2) override;
	void eventSliderPosChanged(int evt_id, float value) override;
	void eventSliderPosFinishedChanging(int evt_id, float value) override;

	void updateLayers();
};

