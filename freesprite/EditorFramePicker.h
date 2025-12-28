#pragma once
#include "PanelUserInteractable.h"
class EditorFramePicker :
    public PanelUserInteractable
{
protected:
    MainEditor* parent = NULL;
    ScrollingPanel* frameButtonPanel = NULL;
    UIStackPanel* frameButtonStack = NULL;
public:
    EditorFramePicker(MainEditor* caller);

    void createFrameButtons();
};

