#pragma once
#include "DraggablePanel.h"
class EditorTouchToggle :
    public DraggablePanel
{
protected:
    MainEditor* parent = NULL;
public:
    EditorTouchToggle(MainEditor* caller);

    void render(XY position) override;
};

