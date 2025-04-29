#pragma once
#include "DraggablePanel.h"
#include "DrawableManager.h"
#include "UILabel.h"
#include "UIButton.h"

/// <summary>
/// Wraps around a Panel to add a button that collapses it + title text
/// </summary>
class CollapsableDraggablePanel : public DraggablePanel 
{
protected:
    const int COLLAPSED_HEIGHT = 30;
    Panel* collapsablePanel;
    UILabel* titleText;
    UIButton* collapseButton;

public:
    CollapsableDraggablePanel(std::string title, Panel* child) {
        collapsablePanel = child;
        collapsablePanel->passThroughMouse = true;
        collapsablePanel->position = {0, 0};
        collapsablePanel->parent = this;
        subWidgets.addDrawable(collapsablePanel);

        titleText = new UILabel(title);
        titleText->position = {30, 5};
        subWidgets.addDrawable(titleText);

        collapseButton = new UIButton();
        collapseButton->position = {5, 5};
        collapseButton->wxWidth = 20;
        collapseButton->wxHeight = 20;
        collapseButton->onClickCallback = [this](UIButton* btn) { toggleCollapse(); };
        collapseButton->text = "-";
        subWidgets.addDrawable(collapseButton);
    }

    void render(XY position) override {
        wxWidth = collapsablePanel->wxWidth;
        //wxHeight = COLLAPSED_HEIGHT + (collapsablePanel->enabled ? collapsablePanel->wxHeight : 0);
        wxHeight = collapsablePanel->enabled ? collapsablePanel->wxHeight : 0;
        DraggablePanel::render(position);
    }

    void toggleCollapse() {
        collapsablePanel->enabled = !collapsablePanel->enabled;
        subWidgets.forceUnfocus();
        focusTimer.start();
        collapseButton->text = collapsablePanel->enabled ? "-" : "+";
    }
};
