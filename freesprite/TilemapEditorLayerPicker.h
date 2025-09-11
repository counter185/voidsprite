#pragma once
#include "globals.h"
#include "DraggablePanel.h"
#include "EventCallbackListener.h"
#include "UISlider.h"
#include "TilemapPreviewScreen.h"

class TilemapEditorLayerPicker :
    public DraggablePanel, public EventCallbackListener
{
public:
	TilemapPreviewScreen* caller;
	std::vector<Drawable*> layerButtons;

	TilemapEditorLayerPicker(TilemapPreviewScreen* editor);

	void render(XY position) override;
	XY getDimensions() override { return XY{ wxWidth,wxHeight }; };

	void eventGeneric(int evt_id, int data1, int data2) override;

	void updateLayers();
};

