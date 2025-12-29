#include "EditorFramePicker.h"
#include "ScrollingPanel.h"
#include "UIStackPanel.h"
#include "maineditor.h"
#include "UIButton.h"
#include "FontRenderer.h"
#include "PopupContextMenu.h"

EditorFramePicker::EditorFramePicker(MainEditor* caller)
{
    wxWidth = 210;
    wxHeight = 80;
    parent = caller;

    setupDraggable();
    setupCollapsible();
    addTitleText("FRAMES");
    //setupResizable({100,70});
    setupCloseButton([this]() {
        this->enabled = false;
    });

    frameButtonPanel = new ScrollingPanel();
    frameButtonPanel->position = { 5,30 };
    frameButtonPanel->wxWidth = 200;
    frameButtonPanel->wxHeight = 45;
    frameButtonPanel->scrollHorizontally = true;
    frameButtonPanel->scrollVertically = false;
    wxsTarget().addDrawable(frameButtonPanel);

    frameButtonStack = UIStackPanel::Horizontal(0, {});
    frameButtonStack->position = { 0, 0 };
    frameButtonPanel->subWidgets.addDrawable(frameButtonStack);

    createFrameButtons();
}

void EditorFramePicker::createFrameButtons()
{
    frameButtonStack->subWidgets.freeAllDrawables();
    int buttonW = g_fnt->StatStringDimensions("000").x + 5;
    for (int i = 0; i < parent->frames.size(); i++) {
        UIButton* frameBtn = new UIButton(std::to_string(i + 1));
        frameBtn->wxWidth = buttonW;
        frameBtn->wxHeight = 30;
        frameBtn->onClickCallback = [this, i](UIButton* btn) {
            parent->switchFrame(i);
        };
        frameBtn->onRightClickCallback = [this, i](UIButton* btn) {
            PopupContextMenu* ctx = new PopupContextMenu({
                {"Delete frame", [this, i]() {
                    parent->deleteFrame(i);
                }},
                {"Duplicate frame", [this, i]() {
                    parent->duplicateFrame(i);
                }}
            });

            g_addPopup(ctx);
        };
        if (i == parent->activeFrame) {
            frameBtn->fill = Fill::Gradient(0x80FFFFFF, 0x80FFFFFF, 0x20FFFFFF, 0x20FFFFFF);
        }
        frameButtonStack->addWidget(frameBtn);
    }
}
