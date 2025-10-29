#pragma once
#include "PanelUserInteractable.h"
#include "EventCallbackListener.h"

class PanelMCBlockPreview :
    public PanelUserInteractable, EventCallbackListener
{
protected:
    MinecraftBlockPreviewScreen* caller;
    bool small = false;

public:
    PanelMCBlockPreview(MinecraftBlockPreviewScreen *caller, bool small = false);
    void renderAfterBG(XY position) override;
};

