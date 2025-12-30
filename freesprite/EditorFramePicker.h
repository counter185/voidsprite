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
    UINumberInputField* msPerFrameInput = NULL;

    EditorFramePicker(MainEditor* caller);

    void createFrameButtons();
};

