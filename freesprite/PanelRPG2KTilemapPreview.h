#pragma once
#include "globals.h"
#if VSP_USE_LIBLCF
#include "DraggablePanel.h"
#include "RPG2KTilemapPreviewScreen.h"
#include "EventCallbackListener.h"

class PanelRPG2KTilemapPreview :
    public DraggablePanel, public EventCallbackListener
{
protected:
    RPG2KTilemapPreviewScreen* caller;

    UILayerButton* btnLL, *btnUL, *btnEL;
public:
    PanelRPG2KTilemapPreview(RPG2KTilemapPreviewScreen* caller);

    void render(XY position) override;

    void eventGeneric(int evt_id, int data1, int data2) override;
};

#endif