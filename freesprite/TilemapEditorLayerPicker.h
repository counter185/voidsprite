#pragma once
#include "globals.h"
#include "PanelUserInteractable.h"
#include "EventCallbackListener.h"
#include "UISlider.h"
#include "TilemapPreviewScreen.h"

class TilemapEditorLayerPicker :
    public PanelUserInteractable, public EventCallbackListener
{
public:
	TilemapPreviewScreen* caller;
	std::vector<Drawable*> layerButtons;

	TilemapEditorLayerPicker(TilemapPreviewScreen* editor);

	void eventGeneric(int evt_id, int data1, int data2) override;

	void updateLayers();
};

