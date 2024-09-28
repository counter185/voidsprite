#pragma once
#include "DraggablePanel.h"
#include "EventCallbackListener.h"

class PanelSpritesheetPreview :
    public DraggablePanel, public EventCallbackListener
{
protected:
    SpritesheetPreviewScreen* caller;

    UILabel* msPerSpriteLabel;
    UITextField* textfieldMSPerSprite;
public:
    PanelSpritesheetPreview(SpritesheetPreviewScreen* caller);
    void render(XY position) override;
    void eventTextInput(int evt_id, std::string data) override;

};

