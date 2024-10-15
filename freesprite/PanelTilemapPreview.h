#pragma once
#include "DraggablePanel.h"

class PanelTilemapPreview :
    public DraggablePanel
{
public:
    TilemapPreviewScreen* caller;

    PanelTilemapPreview(TilemapPreviewScreen* parent);

    void render(XY position) override;
};

