#pragma once
#include "globals.h"
#include "EditorLayerPicker.h"

class PalettizedEditorLayerPicker :
    public EditorLayerPicker
{
public:
    MainEditorPalettized* upcastCaller;

    PalettizedEditorLayerPicker(MainEditorPalettized* editor);
};

