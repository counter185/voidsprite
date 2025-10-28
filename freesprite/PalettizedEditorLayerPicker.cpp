#include "PalettizedEditorLayerPicker.h"
#include "MainEditorPalettized.h"
#include "UIButton.h"
#include "ScrollingPanel.h"

PalettizedEditorLayerPicker::PalettizedEditorLayerPicker(MainEditorPalettized* editor)
{
    caller = editor;
    upcastCaller = editor;

    wxWidth = 250;
    wxHeight = 400;

    setupDraggable();
    setupCollapsible();
    addTitleText(TL("vsp.maineditor.panel.layerpicker.title"));

    UIButton* addBtn = new UIButton();
    addBtn->position = { 5, 30 };
    //addBtn->text = "+";
    addBtn->wxWidth = 30;
    addBtn->tooltip = "New layer";
    addBtn->icon = g_iconLayerAdd;
    addBtn->onClickCallback = [this](UIButton*) { caller->newLayer(); updateLayers(); };
    wxsTarget().addDrawable(addBtn);

    UIButton* removeBtn = new UIButton();
    removeBtn->position = { addBtn->wxWidth + 5 + 5, 30 };
    //removeBtn->text = "-";
    removeBtn->wxWidth = 30;
    removeBtn->icon = g_iconLayerDelete;
    removeBtn->tooltip = "Delete layer";
    removeBtn->onClickCallback = [this](UIButton*) { caller->deleteLayer(caller->selLayer); updateLayers(); };
    wxsTarget().addDrawable(removeBtn);

    UIButton* upBtn = new UIButton();
    upBtn->position = { addBtn->wxWidth + removeBtn->wxWidth + 5 + 5 + 5, 30 };
    //upBtn->text = "Up";
    upBtn->wxWidth = 30;
    upBtn->icon = g_iconLayerUp;
    upBtn->tooltip = "Move layer up";
    upBtn->onClickCallback = [this](UIButton*) { caller->moveLayerUp(caller->selLayer); updateLayers(); };
    wxsTarget().addDrawable(upBtn);

    UIButton* downBtn = new UIButton();
    downBtn->position = { addBtn->wxWidth + removeBtn->wxWidth + upBtn->wxWidth + 5 + 5 + 5 + 5, 30 };
    //downBtn->text = "Dn.";
    downBtn->wxWidth = 30;
    downBtn->icon = g_iconLayerDown;
    downBtn->tooltip = "Move layer down";
    downBtn->onClickCallback = [this](UIButton*) { caller->moveLayerDown(caller->selLayer); updateLayers(); };
    wxsTarget().addDrawable(downBtn);

    UIButton* mergeDownBtn = new UIButton();
    mergeDownBtn->position = { addBtn->wxWidth + removeBtn->wxWidth + upBtn->wxWidth + downBtn->wxWidth + 5 + 5 + 5 + 5 + 5, 30 };
    //mergeDownBtn->text = "Mrg";
    mergeDownBtn->wxWidth = 30;
    mergeDownBtn->icon = g_iconLayerDownMerge;
    mergeDownBtn->tooltip = "Merge down";
    mergeDownBtn->onClickCallback = [this](UIButton*) { caller->mergeLayerDown(caller->selLayer); updateLayers(); };
    wxsTarget().addDrawable(mergeDownBtn);

    UIButton* duplicateBtn = new UIButton();
    duplicateBtn->position = { addBtn->wxWidth + removeBtn->wxWidth + upBtn->wxWidth + downBtn->wxWidth + mergeDownBtn->wxWidth + 5 + 5 + 5 + 5 + 5 + 5, 30 };
    duplicateBtn->wxWidth = 30;
    duplicateBtn->icon = g_iconLayerDuplicate;
    duplicateBtn->tooltip = "Duplicate layer";
    duplicateBtn->onClickCallback = [this](UIButton*) { caller->duplicateLayer(caller->selLayer); updateLayers(); };
    wxsTarget().addDrawable(duplicateBtn);

    layerListPanel = new ScrollingPanel();
    layerListPanel->position = { 5, 80 };
    layerListPanel->scrollHorizontally = false;
    layerListPanel->scrollVertically = true;
    layerListPanel->wxWidth = wxWidth - 10;
    layerListPanel->wxHeight = wxHeight - layerListPanel->position.y - 5;
    wxsTarget().addDrawable(layerListPanel);
}
