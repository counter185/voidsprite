#include "EditorFramePicker.h"
#include "ScrollingPanel.h"
#include "UIStackPanel.h"
#include "maineditor.h"
#include "UIButton.h"
#include "FontRenderer.h"
#include "PopupContextMenu.h"
#include "UILabel.h"
#include "UITextField.h"

EditorFramePicker::EditorFramePicker(MainEditor* caller)
{
    wxWidth = 400;
    wxHeight = 120;
    parent = caller;

    setupDraggable();
    setupCollapsible();
    addTitleText("FRAMES");
    //setupResizable({100,70});
    setupCloseButton([this]() {
        this->enabled = false;
    });

    UIButton* newFrame = new UIButton("+", "New frame");
    newFrame->wxWidth = 30;
    newFrame->wxHeight = 30;
    newFrame->onClickCallback = [this](UIButton* btn) {
        parent->newFrame();
    };

    UIButton* rmFrame = new UIButton("-", "Remove frame");
    rmFrame->wxWidth = 30;
    rmFrame->wxHeight = 30;
    rmFrame->onClickCallback = [this](UIButton* btn) {
        parent->deleteFrame(parent->activeFrame);
    };

    UIButton* duplicateFrame = new UIButton("D", "Duplicate frame");
    duplicateFrame->wxWidth = 30;
    duplicateFrame->wxHeight = 30;
    duplicateFrame->onClickCallback = [this](UIButton* btn) {
        parent->duplicateFrame(parent->activeFrame);
    };

    UIButton* playpauseBtn = new UIButton("P", "Play/pause");
    playpauseBtn->wxWidth = 30;
    playpauseBtn->wxHeight = 30;
    playpauseBtn->onClickCallback = [this](UIButton* btn) {
        parent->toggleFrameAnimation();
    };

    UIButton* mvLeft = new UIButton("<", "Move frame left");
    mvLeft->wxWidth = 30;
    mvLeft->wxHeight = 30;
    mvLeft->onClickCallback = [this](UIButton* btn) {
        parent->moveFrameLeft(parent->activeFrame);
    };

    UIButton* mvRight = new UIButton(">", "Move frame right");
    mvRight->wxWidth = 30;
    mvRight->wxHeight = 30;
    mvRight->onClickCallback = [this](UIButton* btn) {
        parent->moveFrameRight(parent->activeFrame);
    };


    msPerFrameInput = new UINumberInputField(&parent->frameAnimMSPerFrame);
    msPerFrameInput->wxWidth = 50;
    msPerFrameInput->validateFunction = [](int v) { return v > 0; };

    UIStackPanel* topRow = UIStackPanel::Horizontal(4, {
        newFrame,
        rmFrame,
        duplicateFrame,
        mvLeft,
        mvRight,
        playpauseBtn,
        Panel::Space(6,2),
        new UILabel("MS per frame"),
        msPerFrameInput
    });

    frameButtonPanel = new ScrollingPanel();
    frameButtonPanel->wxWidth = wxWidth - 10;
    frameButtonPanel->wxHeight = 50;
    frameButtonPanel->scrollHorizontally = true;
    frameButtonPanel->scrollVertically = false;

    UIStackPanel* content = UIStackPanel::Vertical(4, { topRow, frameButtonPanel });
    content->position = { 5, 30 };
    wxsTarget().addDrawable(content);

    frameButtonStack = UIStackPanel::Horizontal(0, {});
    frameButtonStack->position = { 0, 0 };
    frameButtonPanel->subWidgets.addDrawable(frameButtonStack);

    createFrameButtons();
}

void EditorFramePicker::createFrameButtons()
{
    std::lock_guard<std::recursive_mutex> lock(parent->framesMutex);
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
