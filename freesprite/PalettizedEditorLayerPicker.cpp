#include "PalettizedEditorLayerPicker.h"
#include "MainEditorPalettized.h"

PalettizedEditorLayerPicker::PalettizedEditorLayerPicker(MainEditorPalettized* editor)
{
    caller = editor;
    upcastCaller = editor;

    wxWidth = 250;
    wxHeight = 400;

    UIButton* addBtn = new UIButton();
    addBtn->position = { 5, 30 };
    //addBtn->text = "+";
    addBtn->wxWidth = 30;
    addBtn->setCallbackListener(-1, this);
    addBtn->tooltip = "New layer";
    addBtn->icon = g_iconLayerAdd;
    subWidgets.addDrawable(addBtn);

    UIButton* removeBtn = new UIButton();
    removeBtn->position = { addBtn->wxWidth + 5 + 5, 30 };
    //removeBtn->text = "-";
    removeBtn->wxWidth = 30;
    removeBtn->icon = g_iconLayerDelete;
    removeBtn->tooltip = "Delete layer";
    removeBtn->setCallbackListener(-2, this);
    subWidgets.addDrawable(removeBtn);

    UIButton* upBtn = new UIButton();
    upBtn->position = { addBtn->wxWidth + removeBtn->wxWidth + 5 + 5 + 5, 30 };
    //upBtn->text = "Up";
    upBtn->wxWidth = 30;
    upBtn->icon = g_iconLayerUp;
    upBtn->tooltip = "Move layer up";
    upBtn->setCallbackListener(-3, this);
    subWidgets.addDrawable(upBtn);

    UIButton* downBtn = new UIButton();
    downBtn->position = { addBtn->wxWidth + removeBtn->wxWidth + upBtn->wxWidth + 5 + 5 + 5 + 5, 30 };
    //downBtn->text = "Dn.";
    downBtn->wxWidth = 30;
    downBtn->icon = g_iconLayerDown;
    downBtn->tooltip = "Move layer down";
    downBtn->setCallbackListener(-4, this);
    subWidgets.addDrawable(downBtn);

    UIButton* mergeDownBtn = new UIButton();
    mergeDownBtn->position = { addBtn->wxWidth + removeBtn->wxWidth + upBtn->wxWidth + downBtn->wxWidth + 5 + 5 + 5 + 5 + 5, 30 };
    //mergeDownBtn->text = "Mrg";
    mergeDownBtn->wxWidth = 30;
    mergeDownBtn->icon = g_iconLayerDownMerge;
    mergeDownBtn->tooltip = "Merge down";
    mergeDownBtn->setCallbackListener(-5, this);
    subWidgets.addDrawable(mergeDownBtn);

    UIButton* duplicateBtn = new UIButton();
    duplicateBtn->position = { addBtn->wxWidth + removeBtn->wxWidth + upBtn->wxWidth + downBtn->wxWidth + mergeDownBtn->wxWidth + 5 + 5 + 5 + 5 + 5 + 5, 30 };
    duplicateBtn->wxWidth = 30;
    duplicateBtn->icon = g_iconLayerDuplicate;
    duplicateBtn->tooltip = "Duplicate layer";
    duplicateBtn->setCallbackListener(-6, this);
    subWidgets.addDrawable(duplicateBtn);

    layerListPanel = new ScrollingPanel();
    layerListPanel->position = { 5, 80 };
    layerListPanel->scrollHorizontally = false;
    layerListPanel->scrollVertically = true;
    layerListPanel->wxWidth = wxWidth - 10;
    layerListPanel->wxHeight = wxHeight - layerListPanel->position.y - 5;
    subWidgets.addDrawable(layerListPanel);
}
