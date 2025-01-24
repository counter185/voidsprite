#pragma once
#include "DraggablePanel.h"
#include "EventCallbackListener.h"

class PanelMCBlockPreview :
    public DraggablePanel, EventCallbackListener
{
protected:
    MinecraftBlockPreviewScreen* caller;

public:
    PanelMCBlockPreview(MinecraftBlockPreviewScreen *caller, bool small = false);
    void render(XY position) override;

    void eventSliderPosChanged(int evt_id, float value) override;
    void eventCheckboxToggled(int evt_id, bool newState) override;
};

