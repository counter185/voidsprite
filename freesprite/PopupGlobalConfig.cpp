#include "PopupGlobalConfig.h"
#include "UILabel.h"
#include "UIButton.h"
#include "TabbedView.h"
#include "FontRenderer.h"
#include "UICheckbox.h"
#include "UITextField.h"
#include "Notification.h"
#include "UIDropdown.h"

enum ConfigOptions : int {
    CHECKBOX_OPEN_SAVED_PATH = 1,
    CHECKBOX_ANIMATED_BACKGROUND = 2,
    TEXTFIELD_MAX_UNDO_HISTORY_SIZE = 3,
    CHECKBOX_SCROLL_WITH_TOUCHPAD = 4,
    CHECKBOX_ISOLATE_RECT_ON_LOCK_TILE = 5,
    BUTTON_OPEN_CONFIG_DIR,

};

PopupGlobalConfig::PopupGlobalConfig()
{
    previousConfig = g_config;

    wxHeight = 400;
    wxWidth = 600;

    TabbedView* configTabs = new TabbedView({ {"Editor"}, {"Misc."}}, 90);
    configTabs->position = XY{ 10,50 };
    wxsManager.addDrawable(configTabs);


    /*
        -------------------------
        EDITOR TAB
        -------------------------
    */
    XY posInTab = { 0,10 };

    UICheckbox* cb1 = new UICheckbox("Open saved file location", g_config.openSavedPath);
    cb1->position = posInTab;
    cb1->setCallbackListener(CHECKBOX_OPEN_SAVED_PATH, this);
    configTabs->tabs[0].wxs.addDrawable(cb1);
    posInTab.y += 35;

    UILabel* lbl3 = new UILabel("Animated background");
    lbl3->position = posInTab;
    configTabs->tabs[0].wxs.addDrawable(lbl3);
    UIDropdown* dd1 = new UIDropdown({ "Off", "Sharp", "Smooth", "Sharp (static)", "Smooth (static)" });
    dd1->position = xyAdd(posInTab, { 200, 0 });
    dd1->wxWidth = 120;
    dd1->setCallbackListener(CHECKBOX_ANIMATED_BACKGROUND, this);
    dd1->setTextToSelectedItem = true;
    dd1->text = g_config.animatedBackground < dd1->items.size() ? dd1->items[g_config.animatedBackground] : "--";
    configTabs->tabs[0].wxs.addDrawable(dd1);
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

    UICheckbox* cb4 = new UICheckbox("Isolate rect on locking tile", g_config.isolateRectOnLockTile);
    cb4->position = posInTab;
    cb4->checkbox->tooltip = "When locking a tile loop preview (CTRL+Q), Isolate Rect will be activated on the tile's area.";
    cb4->setCallbackListener(CHECKBOX_ISOLATE_RECT_ON_LOCK_TILE, this);
    configTabs->tabs[0].wxs.addDrawable(cb4);
    posInTab.y += 35;

    /*
        -------------------------
        MISC TAB
        -------------------------
    */
    posInTab = { 0,10 };
    UIButton* btn = new UIButton();
    btn->text = "Open app data directory";
    btn->tooltip = "Open the directory where voidsprite stores its data.";
    btn->position = posInTab;
    btn->wxWidth = 230;
    btn->setCallbackListener(BUTTON_OPEN_CONFIG_DIR, this);
    configTabs->tabs[1].wxs.addDrawable(btn);
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
    else if (evt_id == BUTTON_OPEN_CONFIG_DIR) {
        platformOpenFileLocation(platformEnsureDirAndGetConfigFilePath());
    }
}

void PopupGlobalConfig::eventCheckboxToggled(int evt_id, bool checked)
{
    switch (evt_id) {
        case CHECKBOX_OPEN_SAVED_PATH:
            g_config.openSavedPath = checked;
            break;
        case CHECKBOX_SCROLL_WITH_TOUCHPAD:
            g_config.scrollWithTouchpad = checked;
            break;
        case CHECKBOX_ISOLATE_RECT_ON_LOCK_TILE:
            g_config.isolateRectOnLockTile = checked;
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

void PopupGlobalConfig::eventDropdownItemSelected(int evt_id, int index, std::string name)
{
    if (evt_id == CHECKBOX_ANIMATED_BACKGROUND) {
        g_config.animatedBackground = index;
    }
}
