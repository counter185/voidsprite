
#include "globals.h"
#include "mathops.h"
#include "FontRenderer.h"
#include "EditorLayerPicker.h"
#include "UILayerButton.h"
#include "maineditor.h"

bool EditorLayerPicker::isMouseIn(XY thisPositionOnScreen, XY mousePos)
{
	return pointInBox(mousePos, SDL_Rect{ thisPositionOnScreen.x, thisPositionOnScreen.y, wxWidth, wxHeight });
}

void EditorLayerPicker::render(XY position)
{

    SDL_Rect r = SDL_Rect{ position.x, position.y, wxWidth, wxHeight };
    SDL_SetRenderDrawColor(g_rd, 0x30, 0x30, 0x30, focused ? 0x80 : 0x30);
    SDL_RenderFillRect(g_rd, &r);

    /*r = SDL_Rect{position.x + wxWidth - 60, position.y + wxHeight - 40, 55, 35};
    SDL_SetRenderDrawColor(g_rd, previewCol.r, previewCol.g, previewCol.b, focused ? 0xff : 0x30);
    SDL_RenderFillRect(g_rd, &r);*/

    g_fnt->RenderString("LAYERS", position.x + 1, position.y + 1);

    layerButtons.renderAll(position);
}

void EditorLayerPicker::handleInput(SDL_Event evt, XY gPosOffset)
{
    if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == 1 && evt.button.state) {
        layerButtons.tryFocusOnPoint(XY{ evt.button.x, evt.button.y }, position);
    }
    if (!layerButtons.anyFocused()) {

    }
    else {
        layerButtons.passInputToFocused(evt, gPosOffset);
    }
}

void EditorLayerPicker::eventGeneric(int evt_id, int data1, int data2)
{
    if (data1 == 0) {
        caller->selLayer = evt_id;
    }
    else if (data1 == 1) {
        caller->layers[evt_id]->hidden = !caller->layers[evt_id]->hidden;
    }
    layerButtons.forceUnfocus();
    updateLayers();
}

void EditorLayerPicker::eventButtonPressed(int evt_id)
{
    if (evt_id == -1) {
        caller->newLayer();
    }
    else if (evt_id == -2) {
        caller->deleteLayer(caller->selLayer);
    }
    updateLayers();
}

void EditorLayerPicker::updateLayers()
{
    layerButtons.forceUnfocus();
    layerButtons.freeAllDrawables();

    UIButton* addBtn = new UIButton();
    addBtn->position = { 5, 30 };
    //addBtn->text = "+";
    addBtn->wxWidth = 30;
    addBtn->setCallbackListener(-1, this);
    addBtn->icon = g_iconLayerAdd;
    layerButtons.addDrawable(addBtn);
    
    UIButton* removeBtn = new UIButton();
    removeBtn->position = { addBtn->wxWidth + 5 + 5, 30 };
    //removeBtn->text = "-";
    removeBtn->wxWidth = 30;
    removeBtn->icon = g_iconLayerDelete;
    removeBtn->setCallbackListener(-2, this);
    layerButtons.addDrawable(removeBtn);

    int yposition = 80;
    for (int lid = caller->layers.size(); lid --> 0;) {
        Layer* l = caller->layers[lid];
        UILayerButton* layerButton = new UILayerButton(l->name);
        layerButton->hideButton->colorBGFocused = layerButton->hideButton->colorBGUnfocused = (l->hidden ? SDL_Color{ 255,255,255,0x80 } : SDL_Color{0,0,0,0x80});
        layerButton->position = { 5, yposition };
        layerButton->mainButton->colorBGFocused = layerButton->mainButton->colorBGUnfocused = (caller->selLayer == lid ? SDL_Color{ 255,255,255,0x60 } : SDL_Color{ 0,0,0,0x80 });
        yposition += 30;
        layerButton->setCallbackListener(lid, this);
        layerButtons.addDrawable(layerButton);
    }
}
