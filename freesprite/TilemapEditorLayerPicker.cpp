#include "TilemapEditorLayerPicker.h"
#include "UIButton.h"
#include "UILayerButton.h"
#include "FontRenderer.h"

TilemapEditorLayerPicker::TilemapEditorLayerPicker(TilemapPreviewScreen* editor)
{
    caller = editor;

    UIButton* addBtn = new UIButton();
    addBtn->position = { 5, 30 };
    //addBtn->text = "+";
    addBtn->wxWidth = 30;
    addBtn->setCallbackListener(-1, this);
    addBtn->icon = g_iconLayerAdd;
    layerControlButtons.addDrawable(addBtn);

    UIButton* removeBtn = new UIButton();
    removeBtn->position = { addBtn->wxWidth + 5 + 5, 30 };
    //removeBtn->text = "-";
    removeBtn->wxWidth = 30;
    removeBtn->icon = g_iconLayerDelete;
    removeBtn->setCallbackListener(-2, this);
    layerControlButtons.addDrawable(removeBtn);

    UIButton* upBtn = new UIButton();
    upBtn->position = { addBtn->wxWidth + removeBtn->wxWidth + 5 + 5 + 5, 30 };
    //upBtn->text = "Up";
    upBtn->wxWidth = 30;
    upBtn->icon = g_iconLayerUp;
    upBtn->setCallbackListener(-3, this);
    layerControlButtons.addDrawable(upBtn);

    UIButton* downBtn = new UIButton();
    downBtn->position = { addBtn->wxWidth + removeBtn->wxWidth + upBtn->wxWidth + 5 + 5 + 5 + 5, 30 };
    //downBtn->text = "Dn.";
    downBtn->wxWidth = 30;
    downBtn->icon = g_iconLayerDown;
    downBtn->setCallbackListener(-4, this);
    layerControlButtons.addDrawable(downBtn);

    UIButton* mergeDownBtn = new UIButton();
    mergeDownBtn->position = { addBtn->wxWidth + removeBtn->wxWidth + upBtn->wxWidth + downBtn->wxWidth + 5 + 5 + 5 + 5 + 5, 30 };
    //mergeDownBtn->text = "Mrg";
    mergeDownBtn->wxWidth = 30;
    mergeDownBtn->icon = g_iconLayerDownMerge;
    mergeDownBtn->setCallbackListener(-5, this);
    layerControlButtons.addDrawable(mergeDownBtn);

    UIButton* duplicateBtn = new UIButton();
    duplicateBtn->position = { addBtn->wxWidth + removeBtn->wxWidth + upBtn->wxWidth + downBtn->wxWidth + mergeDownBtn->wxWidth + 5 + 5 + 5 + 5 + 5 + 5, 30 };
    duplicateBtn->wxWidth = 30;
    duplicateBtn->icon = g_iconLayerDuplicate;
    duplicateBtn->setCallbackListener(-6, this);
    layerControlButtons.addDrawable(duplicateBtn);

    updateLayers();
}

TilemapEditorLayerPicker::~TilemapEditorLayerPicker()
{
    layerButtons.freeAllDrawables();
    layerControlButtons.freeAllDrawables();
}


bool TilemapEditorLayerPicker::isMouseIn(XY thisPositionOnScreen, XY mousePos)
{
    return pointInBox(mousePos, SDL_Rect{ thisPositionOnScreen.x, thisPositionOnScreen.y, wxWidth, wxHeight })
        || layerButtons.mouseInAny(thisPositionOnScreen, mousePos);
}

void TilemapEditorLayerPicker::render(XY position)
{

    SDL_Rect r = SDL_Rect{ position.x, position.y, wxWidth, wxHeight };
    //SDL_SetRenderDrawColor(g_rd, 0x30, 0x30, 0x30, focused ? 0x80 : 0x30);
    //SDL_RenderFillRect(g_rd, &r);

    SDL_Color colorBG1 = { 0x30, 0x30, 0x30, u8(focused ? 0xa0 : 0x90) };
    SDL_Color colorBG2 = { 0x10, 0x10, 0x10, u8(focused ? 0xa0 : 0x90) };
    renderGradient(r, sdlcolorToUint32(colorBG2), sdlcolorToUint32(colorBG1), sdlcolorToUint32(colorBG1), sdlcolorToUint32(colorBG1));
    if (focused) {
        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 255);
        drawLine({ position.x, position.y }, { position.x, position.y + wxHeight }, XM1PW3P1(focusTimer.percentElapsedTime(300)));
        drawLine({ position.x, position.y }, { position.x + wxWidth, position.y }, XM1PW3P1(focusTimer.percentElapsedTime(300)));
    }

    /*r = SDL_Rect{position.x + wxWidth - 60, position.y + wxHeight - 40, 55, 35};
    SDL_SetRenderDrawColor(g_rd, previewCol.r, previewCol.g, previewCol.b, focused ? 0xff : 0x30);
    SDL_RenderFillRect(g_rd, &r);*/

    g_fnt->RenderString("LAYERS", position.x + 4, position.y + 1);

    layerButtons.renderAll(position);
    layerControlButtons.renderAll(position);
}

void TilemapEditorLayerPicker::handleInput(SDL_Event evt, XY gPosOffset)
{
    if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == 1 && evt.button.down) {
        if (!layerButtons.tryFocusOnPoint(XY{ (int)evt.button.x, (int)evt.button.y }, position)) {
            layerControlButtons.tryFocusOnPoint(XY{ (int)evt.button.x, (int)evt.button.y }, position);
        }
    }
    if (layerButtons.anyFocused()) {
        layerButtons.passInputToFocused(evt, gPosOffset);
    }
    else if (layerControlButtons.anyFocused()) {
        layerControlButtons.passInputToFocused(evt, gPosOffset);
    }
    else {

    }
}

void TilemapEditorLayerPicker::eventGeneric(int evt_id, int data1, int data2)
{
    (void) data2;

    if (data1 == 0) {
        caller->switchActiveLayer(evt_id);
    }
    else if (data1 == 1) {
        //change layer visibility
        //caller->layers[evt_id]->hidden = !caller->layers[evt_id]->hidden;
    }
    layerButtons.forceUnfocus();
    updateLayers();
}

void TilemapEditorLayerPicker::eventButtonPressed(int evt_id)
{
    if (evt_id == -1) {
        caller->newLayer();
    }
    else if (evt_id == -2) {
        caller->deleteLayer(caller->activeLayerIndex());
    }
    else if (evt_id == -3) {
        caller->moveLayerUp(caller->activeLayerIndex());
    }
    else if (evt_id == -4) {
        caller->moveLayerDown(caller->activeLayerIndex());
    }
    else if (evt_id == -5) {
        caller->mergeLayerDown(caller->activeLayerIndex());
    }
    else if (evt_id == -6) {
        caller->duplicateLayer(caller->activeLayerIndex());
    }
    updateLayers();
}

void TilemapEditorLayerPicker::updateLayers()
{
    layerButtons.forceUnfocus();
    layerButtons.freeAllDrawables();

    int yposition = 80;
    int selectedLayerIndex = caller->activeLayerIndex();
    for (int lid = caller->tilemap.size(); lid-- > 0;) {
        auto& l = caller->tilemap[lid];
        (void) l;
        //todo:
        //UILayerButton* layerButton = new UILayerButton(l->name);
        UILayerButton* layerButton = new UILayerButton(std::format("Layer {}", lid+1));
        //todo:
        //layerButton->hideButton->colorBGFocused = layerButton->hideButton->colorBGUnfocused = (l->hidden ? SDL_Color{ 255,255,255,0x80 } : SDL_Color{ 0,0,0,0x80 });
        layerButton->hideButton->fill = SDL_Color{ 0,0,0,0x80 };
        layerButton->position = { 5, yposition };
        layerButton->mainButton->fill = (selectedLayerIndex == lid ? SDL_Color{ 255,255,255,0x60 } : SDL_Color{ 0,0,0,0x80 });
        yposition += 30;
        layerButton->setCallbackListener(lid, this);
        layerButtons.addDrawable(layerButton);
    }
}
