#pragma once
#include "PanelUserInteractable.h"
class EditorTouchToggle :
    public PanelUserInteractable
{
protected:
    MainEditor* parent = NULL;
public:
    EditorTouchToggle(MainEditor* caller);
};

