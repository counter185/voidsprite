#pragma once
#include "globals.h"
#include "drawable.h"
#include "EventCallbackListener.h"
#include "DrawableManager.h"
#include "UISlider.h"
#include "TilemapPreviewScreen.h"

class TilemapEditorLayerPicker :
    public Drawable, public EventCallbackListener
{
public:
    int wxWidth = 250;
    int wxHeight = 400;
    TilemapPreviewScreen* caller;
    DrawableManager layerButtons;
    DrawableManager layerControlButtons;

    TilemapEditorLayerPicker(TilemapPreviewScreen* editor);
    ~TilemapEditorLayerPicker();

    bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override;
    void render(XY position) override;
    void handleInput(SDL_Event evt, XY gPosOffset) override;
    XY getDimensions() override { return XY{ wxWidth,wxHeight }; };

    void eventGeneric(int evt_id, int data1, int data2) override;
    void eventButtonPressed(int evt_id) override;

    void updateLayers();
};

