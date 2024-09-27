#include "PopupGlobalConfig.h"
#include "UILabel.h"
#include "UIButton.h"
#include "TabbedView.h"
#include "FontRenderer.h"
#include "UICheckbox.h"
#include "UITextField.h"
#include "Notification.h"

enum ConfigOptions : int {
    CHECKBOX_OPEN_SAVED_PATH = 1,
    CHECKBOX_ANIMATED_BACKGROUND = 2,
    TEXTFIELD_MAX_UNDO_HISTORY_SIZE = 3,
    CHECKBOX_SCROLL_WITH_TOUCHPAD = 4
};

PopupGlobalConfig::PopupGlobalConfig()
{
    previousConfig = g_config;

    wxHeight = 400;
    wxWidth = 600;

    TabbedView* configTabs = new TabbedView({ {"Editor"}, {"Misc."}}, 90);
    configTabs->position = XY{ 10,50 };
    wxsManager.addDrawable(configTabs);

    XY posInTab = { 0,10 };

    UICheckbox* cb1 = new UICheckbox("Open saved file location", g_config.openSavedPath);
    cb1->position = posInTab;
    cb1->setCallbackListener(CHECKBOX_OPEN_SAVED_PATH, this);
    configTabs->tabs[0].wxs.addDrawable(cb1);
    posInTab.y += 35;

    UICheckbox* cb2 = new UICheckbox("Animated background", g_config.animatedBackground);
    cb2->position = posInTab;
    cb2->setCallbackListener(CHECKBOX_ANIMATED_BACKGROUND, this);
    configTabs->tabs[0].wxs.addDrawable(cb2);
    posInTab.y += 35;

    UICheckbox* cb3 = new UICheckbox("Pan canvas with touchpad", g_config.scrollWithTouchpad);
    cb3->position = posInTab;
    cb3->setCallbackListener(CHECKBOX_SCROLL_WITH_TOUCHPAD, this);
    configTabs->tabs[0].wxs.addDrawable(cb3);
    posInTab.y += 35;

    UILabel* lbl2 = new UILabel("Max undo history");
    lbl2->position = posInTab;
    configTabs->tabs[0].wxs.addDrawable(lbl2);
    UITextField* tf2 = new UITextField();
    tf2->isNumericField = true;
    tf2->position = XY{ posInTab.x + 10 + g_fnt->StatStringDimensions(lbl2->text).x, posInTab.y};
    tf2->wxWidth = 80;
    tf2->text = std::to_string(g_config.maxUndoHistory);
    tf2->setCallbackListener(TEXTFIELD_MAX_UNDO_HISTORY_SIZE, this);
    configTabs->tabs[0].wxs.addDrawable(tf2);
    posInTab.y += 35;

    UIButton* closeButton = new UIButton();
    closeButton->text = "Close";
    closeButton->position = XY{ wxWidth - 140, wxHeight - 40 };
    closeButton->wxHeight = 30;
    closeButton->wxWidth = 130;
    closeButton->setCallbackListener(0, this);
    wxsManager.addDrawable(closeButton);

    UIButton* saveAndCloseButton = new UIButton();
    saveAndCloseButton->text = "Save and close";
    saveAndCloseButton->position = XY{ wxWidth - 280, wxHeight - 40 };
    saveAndCloseButton->wxHeight = 30;
    saveAndCloseButton->wxWidth = 130;
    saveAndCloseButton->setCallbackListener(1, this);
    wxsManager.addDrawable(saveAndCloseButton);
}

void PopupGlobalConfig::render()
{
    renderDefaultBackground();

    XY titlePos = getDefaultTitlePosition();
    g_fnt->RenderString("Preferences", titlePos.x, titlePos.y);

    renderDrawables();
}

void PopupGlobalConfig::eventButtonPressed(int evt_id)
{
    if (evt_id == 0) {
        // close and discard
        g_config = previousConfig;
        closePopup();
    }
    else if (evt_id == 1) {
        //save and close
        if (g_saveConfig()) {
            g_addNotification(SuccessShortNotification("Success", "Preferences saved"));
            closePopup();
        }
        else {
            g_addNotification(ErrorNotification("Error", "Failed to save preferences"));
        }
    }
}

void PopupGlobalConfig::eventCheckboxToggled(int evt_id, bool checked)
{
    switch (evt_id) {
        case CHECKBOX_OPEN_SAVED_PATH:
            g_config.openSavedPath = checked;
            break;
        case CHECKBOX_ANIMATED_BACKGROUND:
            g_config.animatedBackground = checked;
            break;
        case CHECKBOX_SCROLL_WITH_TOUCHPAD:
            g_config.scrollWithTouchpad = checked;
            break;
    }
}

void PopupGlobalConfig::eventTextInput(int evt_id, std::string text)
{
    switch (evt_id) {
        case TEXTFIELD_MAX_UNDO_HISTORY_SIZE:
            try {
                g_config.maxUndoHistory = std::stoi(text);
            }
            catch (std::exception) {
                g_config.maxUndoHistory = previousConfig.maxUndoHistory;
            }
            break;
    }
}
