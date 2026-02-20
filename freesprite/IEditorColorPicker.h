#pragma once
#include "globals.h"

class IEditorColorPicker {
public:
    virtual Panel* getPanel() = 0;

    virtual void toggleEraser() = 0;
    virtual void toggleBlendMode() = 0;
    virtual void forceFocusOnColorInputField() = 0;

    virtual void setColorRGB(u32 color) = 0;

    virtual void updateAlphaSlider() = 0;

    virtual void pushLastColor(u32 color) = 0;
    virtual void clearLastColors() = 0;
    virtual void reloadColorLists() = 0;

    virtual void actionCtrlScroll(float scrollAmount) = 0;
    virtual void actionShiftScroll(float scrollAmount) = 0;
};