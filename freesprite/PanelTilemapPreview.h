#pragma once
#include "PanelUserInteractable.h"

class PanelTilemapPreview :
    public PanelUserInteractable
{
public:
    TilemapPreviewScreen* caller;

    PanelTilemapPreview(TilemapPreviewScreen* parent);

    void renderAfterBG(XY position) override;
};

