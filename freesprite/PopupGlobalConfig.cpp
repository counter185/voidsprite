#include "PopupGlobalConfig.h"
#include "UILabel.h"
#include "UIButton.h"
#include "TabbedView.h"
#include "FontRenderer.h"
#include "UICheckbox.h"
#include "UITextField.h"
#include "Notification.h"
#include "UIDropdown.h"
#include "ScrollingPanel.h"
#include "BaseBrush.h"
#include "discord_rpc.h"

enum ConfigOptions : int {
    CHECKBOX_OPEN_SAVED_PATH = 1,
    CHECKBOX_ANIMATED_BACKGROUND = 2,
    TEXTFIELD_MAX_UNDO_HISTORY_SIZE = 3,
    CHECKBOX_SCROLL_WITH_TOUCHPAD = 4,
    CHECKBOX_ISOLATE_RECT_ON_LOCK_TILE = 5,
    CHECKBOX_FILL_TOOL_TILE_BOUND = 6,
    BUTTON_OPEN_CONFIG_DIR,
    CHECKBOX_VSYNC,
    CHECKBOX_SAVE_LOAD_FLAT_IMAGE_EXT_DATA,
    CHECKBOX_DISCORD_RPC,
    CHECKBOX_RENDERER,
};

PopupGlobalConfig::PopupGlobalConfig()
{
    //do not do   previousConfig = g_config  it will crash 
    previousConfig = GlobalConfig(g_config);

    wxHeight = 400;
    wxWidth = 700;

    TabbedView* configTabs = new TabbedView({ {"General"}, {"Editor"}, {"Keybinds"}, {"Misc."}}, 90);
    configTabs->position = XY{ 10,50 };
    wxsManager.addDrawable(configTabs);

    /*
        -------------------------
        GENERAL TAB
        -------------------------
    */
    XY posInTab = { 0,10 };

    UICheckbox* cb6 = new UICheckbox("Vertical sync", g_config.vsync);
    cb6->position = posInTab;
    cb6->checkbox->tooltip = "When enabled, the framerate will be locked to your display's refresh rate.\nDisabling this will make brushes smoother but also increase energy consumption.\nvoidsprite must be restarted for this change to take effect.";
    cb6->setCallbackListener(CHECKBOX_VSYNC, this);
    configTabs->tabs[0].wxs.addDrawable(cb6);
    posInTab.y += 35;

    UICheckbox* cb7 = new UICheckbox("Save/load extra data to PNGs", g_config.saveLoadFlatImageExtData);
    cb7->position = posInTab;
    cb7->checkbox->tooltip = "When enabled, voidsprite will load and save extra data such as canvas comments,\ntile grid and symmetry to PNGs.";
    cb7->setCallbackListener(CHECKBOX_SAVE_LOAD_FLAT_IMAGE_EXT_DATA, this);
    configTabs->tabs[0].wxs.addDrawable(cb7);
    posInTab.y += 35;

    UICheckbox* cb8 = new UICheckbox("Discord Rich Presence", g_config.useDiscordRPC);
    cb8->position = posInTab;
    cb8->checkbox->tooltip = "When enabled, your activity will be shared as your Discord status.\nSupported only on Windows.";
    cb8->setCallbackListener(CHECKBOX_DISCORD_RPC, this);
    configTabs->tabs[0].wxs.addDrawable(cb8);
    posInTab.y += 35;

    UILabel* lbl4 = new UILabel("Renderer");
    lbl4->position = posInTab;
    configTabs->tabs[0].wxs.addDrawable(lbl4);
    UIDropdown* dd2 = new UIDropdown(g_availableRenderersNow);
    dd2->position = xyAdd(posInTab, {100, 0});
    dd2->wxWidth = 180;
    dd2->setCallbackListener(CHECKBOX_RENDERER, this);
    dd2->setTextToSelectedItem = true;
    dd2->text = g_config.preferredRenderer != "" ? g_config.preferredRenderer : "<auto>";
    configTabs->tabs[0].wxs.addDrawable(dd2);
    posInTab.y += 35;


    /*
        -------------------------
        EDITOR TAB
        -------------------------
    */
    posInTab = { 0,10 };

    UICheckbox* cb1 = new UICheckbox("Open saved file location", g_config.openSavedPath);
    cb1->position = posInTab;
    cb1->setCallbackListener(CHECKBOX_OPEN_SAVED_PATH, this);
    configTabs->tabs[1].wxs.addDrawable(cb1);
    posInTab.y += 35;

    UILabel* lbl3 = new UILabel("Animated background");
    lbl3->position = posInTab;
    configTabs->tabs[1].wxs.addDrawable(lbl3);
    UIDropdown* dd1 = new UIDropdown({ "Off", "Sharp", "Smooth", "Sharp (static)", "Smooth (static)" });
    dd1->position = xyAdd(posInTab, { 200, 0 });
    dd1->wxWidth = 180;
    dd1->setCallbackListener(CHECKBOX_ANIMATED_BACKGROUND, this);
    dd1->setTextToSelectedItem = true;
    dd1->text = g_config.animatedBackground < dd1->items.size() ? dd1->items[g_config.animatedBackground] : "--";
    configTabs->tabs[1].wxs.addDrawable(dd1);
    posInTab.y += 35;

    UICheckbox* cb3 = new UICheckbox("Pan canvas with touchpad", g_config.scrollWithTouchpad);
    cb3->position = posInTab;
    cb3->setCallbackListener(CHECKBOX_SCROLL_WITH_TOUCHPAD, this);
    configTabs->tabs[1].wxs.addDrawable(cb3);
    posInTab.y += 35;

    UILabel* lbl2 = new UILabel("Max undo history");
    lbl2->position = posInTab;
    configTabs->tabs[1].wxs.addDrawable(lbl2);
    UITextField* tf2 = new UITextField();
    tf2->isNumericField = true;
    tf2->position = XY{ posInTab.x + 10 + g_fnt->StatStringDimensions(lbl2->text).x, posInTab.y};
    tf2->wxWidth = 80;
    tf2->setText(std::to_string(g_config.maxUndoHistory));
    tf2->setCallbackListener(TEXTFIELD_MAX_UNDO_HISTORY_SIZE, this);
    configTabs->tabs[1].wxs.addDrawable(tf2);
    posInTab.y += 35;

    UICheckbox* cb4 = new UICheckbox("Isolate rect on locking tile", g_config.isolateRectOnLockTile);
    cb4->position = posInTab;
    cb4->checkbox->tooltip = "When locking a tile loop preview (CTRL+Q), Isolate Rect will be activated on the tile's area.";
    cb4->setCallbackListener(CHECKBOX_ISOLATE_RECT_ON_LOCK_TILE, this);
    configTabs->tabs[1].wxs.addDrawable(cb4);
    posInTab.y += 35;

    UICheckbox* cb5 = new UICheckbox("Lock Fill tool to tile size", g_config.fillToolTileBound);
    cb5->position = posInTab;
    cb5->checkbox->tooltip = "When enabled, the Fill tool will not flow past the current tile if a tile size is set.";
    cb5->setCallbackListener(CHECKBOX_FILL_TOOL_TILE_BOUND, this);
    configTabs->tabs[1].wxs.addDrawable(cb5);
    posInTab.y += 35;

    /*
        -------------------------
        KEYBINDS TAB
        -------------------------
    */
    posInTab = { 0,10 };
    ScrollingPanel* keybindsPanel = new ScrollingPanel();
    keybindsPanel->position = posInTab;
    keybindsPanel->wxWidth = wxWidth - 20;
    keybindsPanel->wxHeight = wxHeight - 140;
    keybindsPanel->scrollVertically = true;
    keybindsPanel->scrollHorizontally = false;
    configTabs->tabs[2].wxs.addDrawable(keybindsPanel);


    std::vector<KeybindConf> keybindsWithIcons = {
    };
    for (BaseBrush* b : g_brushes) {
        keybindsWithIcons.push_back({ b->getName(), &b->keybind, b->cachedIcon });
    }

    XY scrollPanelPos = { 0,0 };
    int x = 1000;
    for (KeybindConf& kk : keybindsWithIcons) {
        UIButton* b = new UIButton();
        b->wxWidth = keybindsPanel->wxWidth - 20;
        b->wxHeight = 30;
        //b->text = std::format("{} ({})", kk.name, SDL_GetKeyName(*kk.target));
        b->icon = kk.icon;
        b->position = scrollPanelPos;
        b->setCallbackListener(x++, this);

        keybindButtons.push_back({kk, b});
        updateKeybindButtonText(keybindButtons.back());
        scrollPanelPos.y += 30;
        keybindsPanel->subWidgets.addDrawable(b);
    }


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
    configTabs->tabs[3].wxs.addDrawable(btn);
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

void PopupGlobalConfig::takeInput(SDL_Event evt)
{
    if (bindingKeyIndex != -1) {
        if (evt.type == SDL_KEYDOWN) {
            int targetKey = evt.key.scancode == SDL_SCANCODE_LSHIFT ? SDL_SCANCODE_UNKNOWN : evt.key.scancode;
            if (targetKey != SDL_SCANCODE_ESCAPE) {
                //find any other keybinds that use this key and reset them
                bool keyIsReserved = (targetKey != SDL_SCANCODE_UNKNOWN) && (std::find(reservedKeys.begin(), reservedKeys.end(), targetKey) != reservedKeys.end());
                if (!keyIsReserved) {
                    for (auto& kb : keybindButtons) {
                        if (kb.first.target != keybindButtons[bindingKeyIndex].first.target && *kb.first.target == targetKey) {
                            *kb.first.target = SDL_SCANCODE_UNKNOWN;
                            updateKeybindButtonText(kb);
                        }
                    }
                    *keybindButtons[bindingKeyIndex].first.target = (SDL_Scancode)targetKey;
                }
                else {
                    g_addNotification(ErrorNotification("Error", std::format("{} is a reserved key.", std::string(SDL_GetScancodeName((SDL_Scancode)targetKey)))));
                }
            }
            updateKeybindButtonText(keybindButtons[bindingKeyIndex]);
            bindingKeyIndex = -1;
        }
    }
    else {
        BasePopup::takeInput(evt);
    }
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
            g_initOrDeinitRPCBasedOnConfig();
            closePopup();
        }
        else {
            g_addNotification(ErrorNotification("Error", "Failed to save preferences"));
        }
    }
    else if (evt_id == BUTTON_OPEN_CONFIG_DIR) {
        platformOpenFileLocation(platformEnsureDirAndGetConfigFilePath());
    }
    else if (evt_id >= 1000) {
        bindingKey = true;
        bindingKeyIndex = evt_id - 1000;
        keybindButtons[bindingKeyIndex].second->text = std::format("{} [SET KEY... (ESC: cancel, LShift: clear)]", keybindButtons[bindingKeyIndex].first.name);
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
        case CHECKBOX_FILL_TOOL_TILE_BOUND:
            g_config.fillToolTileBound = checked;
            break;
        case CHECKBOX_VSYNC:
            g_config.vsync = checked;
            break;
        case CHECKBOX_SAVE_LOAD_FLAT_IMAGE_EXT_DATA:
            g_config.saveLoadFlatImageExtData = checked;
            break;
        case CHECKBOX_DISCORD_RPC:
            g_config.useDiscordRPC = checked;
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
    else if (evt_id == CHECKBOX_RENDERER) {
        g_config.preferredRenderer = name;
    }
}

void PopupGlobalConfig::updateKeybindButtonText(std::pair<KeybindConf, UIButton*> t)
{
    t.second->text = std::format("{}    ({})", t.first.name, SDL_GetScancodeName(*t.first.target));
}
