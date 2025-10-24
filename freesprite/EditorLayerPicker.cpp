
#include "globals.h"
#include "mathops.h"
#include "FontRenderer.h"
#include "EditorLayerPicker.h"
#include "UILayerButton.h"
#include "maineditor.h"
#include "Panel.h"
#include "UISlider.h"
#include "UILabel.h"

EditorLayerPicker::EditorLayerPicker(MainEditor* editor) {
    caller = editor;

    wxWidth = 250;
    wxHeight = 400;
    borderColor = visualConfigHexU32("ui/panel/border");

    UIButton* addBtn = new UIButton();
    addBtn->position = { 5, 30 };
    //addBtn->text = "+";
    addBtn->wxWidth = 30;
    addBtn->icon = g_iconLayerAdd;
    addBtn->tooltip = "New layer";
    addBtn->onClickCallback = [this](UIButton*) { caller->newLayer(); updateLayers(); };
    subWidgets.addDrawable(addBtn);

    UIButton* removeBtn = new UIButton();
    removeBtn->position = { addBtn->wxWidth + 5 + 5, 30 };
    //removeBtn->text = "-";
    removeBtn->wxWidth = 30;
    removeBtn->icon = g_iconLayerDelete;
    removeBtn->tooltip = "Delete layer";
    removeBtn->onClickCallback = [this](UIButton*) { caller->deleteLayer(caller->selLayer); updateLayers(); };
    subWidgets.addDrawable(removeBtn);

    UIButton* upBtn = new UIButton();
    upBtn->position = { addBtn->wxWidth + removeBtn->wxWidth + 5 + 5 + 5, 30 };
    upBtn->wxWidth = 30;
    upBtn->icon = g_iconLayerUp;
    upBtn->tooltip = "Move layer up";
    upBtn->onClickCallback = [this](UIButton*) { caller->moveLayerUp(caller->selLayer); updateLayers(); };
    subWidgets.addDrawable(upBtn);

    UIButton* downBtn = new UIButton();
    downBtn->position = { addBtn->wxWidth + removeBtn->wxWidth + upBtn->wxWidth + 5 + 5 + 5 + 5, 30 };
    downBtn->wxWidth = 30;
    downBtn->icon = g_iconLayerDown;
    downBtn->tooltip = "Move layer down";
    downBtn->onClickCallback = [this](UIButton*) { caller->moveLayerDown(caller->selLayer); updateLayers(); };
    subWidgets.addDrawable(downBtn);

    UIButton* mergeDownBtn = new UIButton();
    mergeDownBtn->position = { addBtn->wxWidth + removeBtn->wxWidth + upBtn->wxWidth + downBtn->wxWidth + 5 + 5 + 5 + 5 + 5, 30 };
    //mergeDownBtn->text = "Mrg";
    mergeDownBtn->wxWidth = 30;
    mergeDownBtn->icon = g_iconLayerDownMerge;
    mergeDownBtn->tooltip = "Merge down";
    mergeDownBtn->onClickCallback = [this](UIButton*) { caller->mergeLayerDown(caller->selLayer); updateLayers(); };
    subWidgets.addDrawable(mergeDownBtn);

    UIButton* duplicateBtn = new UIButton();
    duplicateBtn->position = { addBtn->wxWidth + removeBtn->wxWidth + upBtn->wxWidth + downBtn->wxWidth + mergeDownBtn->wxWidth + 5 + 5 + 5 + 5 + 5 + 5, 30 };
    duplicateBtn->wxWidth = 30;
    duplicateBtn->icon = g_iconLayerDuplicate;
    duplicateBtn->tooltip = "Duplicate layer";
    duplicateBtn->onClickCallback = [this](UIButton*) { caller->duplicateLayer(caller->selLayer); updateLayers(); };
    subWidgets.addDrawable(duplicateBtn);

    UILabel* opacityLabel = new UILabel(TL("vsp.cmn.opacity"));
    opacityLabel->position = { 7, 67 };
    opacityLabel->fontsize = 16;
    subWidgets.addDrawable(opacityLabel);

    opacitySlider = new UISlider();
    opacitySlider->position = { 80, 70 };
    opacitySlider->wxWidth = 165;
    opacitySlider->wxHeight = 20;
    opacitySlider->setCallbackListener(EVENT_LAYERPICKER_OPACITYSLIDER, this);
    subWidgets.addDrawable(opacitySlider);

    layerListPanel = new ScrollingPanel();
    layerListPanel->position = { 5, 100 };
    layerListPanel->scrollHorizontally = false;
    layerListPanel->scrollVertically = true;
    layerListPanel->wxWidth = wxWidth - 10;
    layerListPanel->wxHeight = wxHeight - layerListPanel->position.y - 5;
    subWidgets.addDrawable(layerListPanel);
}

void EditorLayerPicker::render(XY position)
{
    if (!enabled) return;

    SDL_Rect r = SDL_Rect{ position.x, position.y, wxWidth, wxHeight };
    //SDL_SetRenderDrawColor(g_rd, 0x30, 0x30, 0x30, focused ? 0x80 : 0x30);
    //SDL_RenderFillRect(g_rd, &r);

    SDL_Color colorBG1 = { 0x30, 0x30, 0x30, (u8)(focused ? 0xa0 : 0x90)};
    SDL_Color colorBG2 = { 0x10, 0x10, 0x10, (u8)(focused ? 0xa0 : 0x90)};
    renderGradient(r, sdlcolorToUint32(colorBG2), sdlcolorToUint32(colorBG1), sdlcolorToUint32(colorBG1), sdlcolorToUint32(colorBG1));
    if (thisOrParentFocused()) {
        renderFocusBorder(position, SDL_Color{ 255,255,255,255 });
    }

    g_fnt->RenderString("LAYERS", position.x + 4, position.y + 1);

    DraggablePanel::render(position);
}

void EditorLayerPicker::eventGeneric(int evt_id, int data1, int data2)
{
    if (data1 == 0) {
        caller->switchActiveLayer(evt_id);
    }
    else if (data1 == 1) {
        caller->layers[evt_id]->hidden = !caller->layers[evt_id]->hidden;
    }
    else if (data1 == 2) {
        if (caller->selLayer != evt_id) {
            caller->switchActiveLayer(evt_id);
        }
        caller->layer_switchVariant(caller->layers[evt_id], data2);
    }
    else if (data1 == 3) {
        caller->layer_removeVariant(caller->layers[evt_id], data2);
    }
    updateLayers();
}

void EditorLayerPicker::eventSliderPosChanged(int evt_id, float value)
{
    if (evt_id == EVENT_LAYERPICKER_OPACITYSLIDER) {
        caller->getCurrentLayer()->layerAlpha = (uint8_t)(value * 255);
    }
}

void EditorLayerPicker::eventSliderPosFinishedChanging(int evt_id, float value)
{
    if (evt_id == EVENT_LAYERPICKER_OPACITYSLIDER) {
        caller->layer_setOpacity((uint8_t)(value * 255));
        //logprintf("eventSliderPosFinishedChanging, %x\n", (uint8_t)(value * 255));
    }
}

void EditorLayerPicker::updateLayers()
{
    layerListPanel->subWidgets.freeAllDrawables();

    int yposition = 0;
    for (int lid = caller->layers.size(); lid --> 0;) {
        Layer* l = caller->layers[lid];
        UILayerButton* layerButton = new UILayerButton(l->name, l);
        layerButton->hideButton->fill = (l->hidden ? Fill::Gradient(0x00FFFFFF, 0x70FFFFFF, 0x00FFFFFF, 0x70FFFFFF) : SDL_Color{0,0,0,0x80});
        layerButton->position = { 0, yposition };
        layerButton->mainButton->fill = (caller->selLayer == lid ? Fill::Gradient(0x70FFFFFF, 0x00FFFFFF, 0x70FFFFFF, 0x00FFFFFF) : SDL_Color{ 0,0,0,0x80 });
        yposition += layerButton->getDimensions().y;
        layerButton->setCallbackListener(lid, this);
        layerListPanel->subWidgets.addDrawable(layerButton);
    }

    if (opacitySlider != NULL && !caller->layers.empty()) {
        opacitySlider->sliderPos = caller->getCurrentLayer()->layerAlpha / 255.0f;
    }
}
