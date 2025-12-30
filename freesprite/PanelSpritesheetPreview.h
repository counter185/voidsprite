#pragma once
#include "PanelUserInteractable.h"

class PanelSpritesheetPreview :
    public PanelUserInteractable
{
protected:
    SpritesheetPreviewScreen* caller;
public:
    PanelSpritesheetPreview(SpritesheetPreviewScreen* caller);
    void render(XY position) override;

};

