#include "EditorFramePicker.h"
#include "ScrollingPanel.h"
#include "UIStackPanel.h"
#include "maineditor.h"
#include "UIButton.h"
#include "FontRenderer.h"
#include "PopupContextMenu.h"
#include "UILabel.h"
#include "UITextField.h"
#include "UISlider.h"

EditorFramePicker::EditorFramePicker(MainEditor* caller)
{
    wxWidth = 500;
    wxHeight = 154;
    parent = caller;

    setupDraggable();
    setupCollapsible();
    addTitleText("FRAMES");
    //setupResizable({100,70});
    setupCloseButton([this]() {
        this->enabled = false;
    });

    UIButton* newFrame = new UIButton("", "New frame");
    newFrame->icon = g_iconFrameNew;
    newFrame->wxWidth = 30;
    newFrame->wxHeight = 30;
    newFrame->onClickCallback = [this](UIButton* btn) {
        parent->newFrame();
    };

    UIButton* rmFrame = new UIButton("", "Remove frame");
    rmFrame->icon = g_iconFrameDelete;
    rmFrame->wxWidth = 30;
    rmFrame->wxHeight = 30;
    rmFrame->onClickCallback = [this](UIButton* btn) {
        parent->deleteFrame(parent->activeFrame);
    };

    UIButton* duplicateFrame = new UIButton("", "Duplicate frame");
    duplicateFrame->icon = g_iconFrameDuplicate;
    duplicateFrame->wxWidth = 30;
    duplicateFrame->wxHeight = 30;
    duplicateFrame->onClickCallback = [this](UIButton* btn) {
        parent->duplicateFrame(parent->activeFrame);
    };

    playpauseBtn = new UIButton("", "Play/pause");
    playpauseBtn->icon = g_iconFramePlayPause;
    playpauseBtn->wxWidth = 30;
    playpauseBtn->wxHeight = 30;
    playpauseBtn->onClickCallback = [this](UIButton* btn) {
        parent->toggleFrameAnimation();
        createFrameButtons();
    };

    UIButton* mvLeft = new UIButton("", "Move frame left");
    mvLeft->icon = g_iconFrameMoveLeft;
    mvLeft->wxWidth = 30;
    mvLeft->wxHeight = 30;
    mvLeft->onClickCallback = [this](UIButton* btn) {
        parent->moveFrameLeft(parent->activeFrame);
    };

    UIButton* mvRight = new UIButton("", "Move frame right");
    mvRight->icon = g_iconFrameMoveRight;
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

    UINumberInputField* backtraceInput = new UINumberInputField(&parent->backtraceFrames);
    backtraceInput->wxWidth = 30;

    UINumberInputField* fwdtraceInput = new UINumberInputField(&parent->fwdtraceFrames);
    fwdtraceInput->wxWidth = 30;

    fwdtraceInput->valueUpdatedCallback = backtraceInput->valueUpdatedCallback = [this](int) {
        createFrameButtons();
    };

    UISlider* opacitySlider = new UISlider();
    opacitySlider->wxHeight = 25;
    opacitySlider->wxWidth = 130;
    opacitySlider->setValue(0, 1, parent->traceOpacity);
    opacitySlider->onChangeValueCallback = [this](UISlider* sld, float val) {
        parent->traceOpacity = val;
    };

    UIStackPanel* traceRow = UIStackPanel::Horizontal(4, {
        new UILabel("Trace"),
        Panel::Space(6,2),
        new UILabel("Back"),
        backtraceInput,
        Panel::Space(4,2),
        new UILabel("Next"),
        fwdtraceInput,
        Panel::Space(6,2),
        new UILabel("Opacity"),
        opacitySlider
    });


    frameButtonPanel = new ScrollingPanel();
    frameButtonPanel->wxWidth = wxWidth - 10;
    frameButtonPanel->wxHeight = 50;
    frameButtonPanel->scrollHorizontally = true;
    frameButtonPanel->scrollVertically = false;

    UIStackPanel* content = UIStackPanel::Vertical(4, { topRow, traceRow, frameButtonPanel });
    content->position = { 5, 30 };
    wxsTarget().addDrawable(content);

    frameButtonStack = UIStackPanel::Horizontal(0, {});
    frameButtonStack->position = { 0, 0 };
    frameButtonPanel->subWidgets.addDrawable(frameButtonStack);

    createFrameButtons();
}

void EditorFramePicker::createFrameButtons()
{
    playpauseBtn->fill = parent->frameAnimationPlaying ? fillPlayButtonPlaying : fillPlayButtonPaused;

    std::lock_guard<std::recursive_mutex> lock(parent->framesMutex);
    frameButtons.clear();
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
            frameBtn->fill = fillActiveFrame;
        }
        else if (i >= parent->activeFrame - parent->backtraceFrames 
                 && i <= parent->activeFrame + parent->fwdtraceFrames) {
            frameBtn->fill = fillTracedFrame;
        }
        frameButtonStack->addWidget(frameBtn);
        frameButtons.push_back(frameBtn);
    }
}

void EditorFramePicker::flashFrame(int index)
{
    if (index >= 0 && index < frameButtons.size()) {
        frameButtons[index]->playClickVFXNextFrame();
    }
}
