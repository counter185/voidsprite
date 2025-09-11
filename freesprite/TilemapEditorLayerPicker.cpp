#include "TilemapEditorLayerPicker.h"
#include "UIButton.h"
#include "UILayerButton.h"
#include "FontRenderer.h"
#include "UILabel.h"

TilemapEditorLayerPicker::TilemapEditorLayerPicker(TilemapPreviewScreen* editor)
{
    wxWidth = 250;
    wxHeight = 400;

    caller = editor;

    subWidgets.addDrawable(new UILabel("LAYERS", {4,1}));

    UIButton* addBtn = new UIButton();
    addBtn->position = { 5, 30 };
    //addBtn->text = "+";
    addBtn->wxWidth = 30;
    addBtn->icon = g_iconLayerAdd;
    addBtn->onClickCallback = [this](UIButton* b) { 
        caller->newLayer(); 
        updateLayers();
    };
    subWidgets.addDrawable(addBtn);

    UIButton* removeBtn = new UIButton();
    removeBtn->position = { addBtn->wxWidth + 5 + 5, 30 };
    //removeBtn->text = "-";
    removeBtn->wxWidth = 30;
    removeBtn->icon = g_iconLayerDelete;
    removeBtn->onClickCallback = [this](UIButton* b) { 
        caller->deleteLayer(caller->activeLayerIndex()); 
        updateLayers();
    };
    subWidgets.addDrawable(removeBtn);

    UIButton* upBtn = new UIButton();
    upBtn->position = { addBtn->wxWidth + removeBtn->wxWidth + 5 + 5 + 5, 30 };
    //upBtn->text = "Up";
    upBtn->wxWidth = 30;
    upBtn->icon = g_iconLayerUp;
    upBtn->onClickCallback = [this](UIButton* b) { 
        caller->moveLayerUp(caller->activeLayerIndex()); 
        updateLayers();
    };
    subWidgets.addDrawable(upBtn);

    UIButton* downBtn = new UIButton();
    downBtn->position = { addBtn->wxWidth + removeBtn->wxWidth + upBtn->wxWidth + 5 + 5 + 5 + 5, 30 };
    //downBtn->text = "Dn.";
    downBtn->wxWidth = 30;
    downBtn->icon = g_iconLayerDown;
    downBtn->onClickCallback = [this](UIButton* b) { 
        caller->moveLayerDown(caller->activeLayerIndex()); 
        updateLayers();
    };
    subWidgets.addDrawable(downBtn);

    UIButton* mergeDownBtn = new UIButton();
    mergeDownBtn->position = { addBtn->wxWidth + removeBtn->wxWidth + upBtn->wxWidth + downBtn->wxWidth + 5 + 5 + 5 + 5 + 5, 30 };
    //mergeDownBtn->text = "Mrg";
    mergeDownBtn->wxWidth = 30;
    mergeDownBtn->icon = g_iconLayerDownMerge;
    mergeDownBtn->onClickCallback = [this](UIButton* b) { 
        caller->mergeLayerDown(caller->activeLayerIndex()); 
        updateLayers();
    };
    subWidgets.addDrawable(mergeDownBtn);

    UIButton* duplicateBtn = new UIButton();
    duplicateBtn->position = { addBtn->wxWidth + removeBtn->wxWidth + upBtn->wxWidth + downBtn->wxWidth + mergeDownBtn->wxWidth + 5 + 5 + 5 + 5 + 5 + 5, 30 };
    duplicateBtn->wxWidth = 30;
    duplicateBtn->icon = g_iconLayerDuplicate;
    duplicateBtn->onClickCallback = [this](UIButton* b) { 
        caller->duplicateLayer(caller->activeLayerIndex());
        updateLayers();
    };
    subWidgets.addDrawable(duplicateBtn);

    updateLayers();
}

void TilemapEditorLayerPicker::render(XY position)
{

    SDL_Rect r = SDL_Rect{ position.x, position.y, wxWidth, wxHeight };
    //SDL_SetRenderDrawColor(g_rd, 0x30, 0x30, 0x30, focused ? 0x80 : 0x30);
    //SDL_RenderFillRect(g_rd, &r);

    SDL_Color colorBG1 = { 0x30, 0x30, 0x30, focused ? 0xa0 : 0x90 };
    SDL_Color colorBG2 = { 0x10, 0x10, 0x10, focused ? 0xa0 : 0x90 };
    renderGradient(r, sdlcolorToUint32(colorBG2), sdlcolorToUint32(colorBG1), sdlcolorToUint32(colorBG1), sdlcolorToUint32(colorBG1));
    if (focused) {
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 255);
        drawLine({ position.x, position.y }, { position.x, position.y + wxHeight }, XM1PW3P1(focusTimer.percentElapsedTime(300)));
        drawLine({ position.x, position.y }, { position.x + wxWidth, position.y }, XM1PW3P1(focusTimer.percentElapsedTime(300)));
    }

    DraggablePanel::render(position);
}

void TilemapEditorLayerPicker::eventGeneric(int evt_id, int data1, int data2)
{
    if (data1 == 0) {
        caller->switchActiveLayer(evt_id);
    }
    else if (data1 == 1) {
        //change layer visibility
        //caller->layers[evt_id]->hidden = !caller->layers[evt_id]->hidden;
    }
    updateLayers();
}

void TilemapEditorLayerPicker::updateLayers()
{
    for (Drawable*& b : layerButtons) {
        subWidgets.removeDrawable(b);
    }
    layerButtons.clear();

    int yposition = 80;
    int selectedLayerIndex = caller->activeLayerIndex();
    for (int lid = caller->tilemap.size(); lid-- > 0;) {
        auto& l = caller->tilemap[lid];
        //todo:
        //UILayerButton* layerButton = new UILayerButton(l->name);
        UILayerButton* layerButton = new UILayerButton(frmt("Layer {}", lid+1), NULL);
        //todo:
        layerButton->hideButton->fill = SDL_Color{ 0,0,0,0x80 };
        layerButton->position = { 5, yposition };
        layerButton->mainButton->fill = (selectedLayerIndex == lid ? SDL_Color{ 255,255,255,0x60 } : SDL_Color{ 0,0,0,0x80 });
        yposition += 30;
        layerButton->setCallbackListener(lid, this);
        subWidgets.addDrawable(layerButton);
        layerButtons.push_back(layerButton);
    }
}
