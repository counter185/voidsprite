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
#include "PopupChooseExtsToAssoc.h"

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
    TEXTFIELD_AUTOSAVE_INTERVAL,
    CHECKBOX_ROWCOLS_START_AT_1,
    DROPDOWN_LANGUAGE,
    DROPDOWN_VISUAL_CONFIG,
};

PopupGlobalConfig::PopupGlobalConfig()
{
    //do not do   previousConfig = g_config  it will crash 
    previousConfig = GlobalConfig(g_config);

    wxHeight = 400;
    wxWidth = 850;

    TabbedView* configTabs = new TabbedView({ 
        {TL("vsp.config.tab.general")}, 
        {TL("vsp.config.tab.visual")}, 
        {TL("vsp.config.tab.editor")}, 
        {TL("vsp.config.tab.keybinds")}, 
        {TL("vsp.config.tab.misc")}
    }, 90);
    configTabs->position = XY{ 10,50 };
    wxsManager.addDrawable(configTabs);

    /*
        -------------------------
        GENERAL TAB
        -------------------------
    */
    XY posInTab = { 0,10 };
    
    configTabs->tabs[0].wxs.addDrawable(optionCheckbox(TL("vsp.config.opt.pngextdata"), TL("vsp.config.opt.pngextdata.desc"), &g_config.saveLoadFlatImageExtData, &posInTab));
    configTabs->tabs[0].wxs.addDrawable(optionCheckbox(TL("vsp.config.opt.discordrpc"), TL("vsp.config.opt.discordrpc.desc"), &g_config.useDiscordRPC, &posInTab));
    configTabs->tabs[0].wxs.addDrawable(optionCheckbox(TL("vsp.config.opt.usesystemfilepicker"), TL("vsp.config.opt.usesystemfilepicker.desc"), &g_config.useSystemFileDialog, &posInTab));

    std::vector<std::string> langNames;
    for (auto& loc : getLocalizations()) {
        langLocNames.push_back(loc.first);
        langNames.push_back(loc.second.langName);
    }
    UILabel* lbl4 = new UILabel(TL("vsp.config.opt.lang"));
    lbl4->position = posInTab;
    configTabs->tabs[0].wxs.addDrawable(lbl4);
    UIDropdown* dd2 = new UIDropdown(langNames);
    dd2->position = xyAdd(posInTab, { 300, 0 });
    dd2->wxWidth = 180;
    dd2->setCallbackListener(DROPDOWN_LANGUAGE, this);
    dd2->setTextToSelectedItem = true;
    dd2->text = getLocalizations().contains(g_config.language) ? getLocalizations()[g_config.language].langName : "???";
    configTabs->tabs[0].wxs.addDrawable(dd2);
    posInTab.y += 35;

    languageCredit = new UILabel("");
    languageCredit->fontsize = 16;
    languageCredit->position = xyAdd(posInTab, {30, 0});
    languageCredit->color = { 255,255,255,0xa0 };
    configTabs->tabs[0].wxs.addDrawable(languageCredit);
    posInTab.y += 70;
    updateLanguageCredit();

    /*
        -------------------------
        VISUAL TAB
        -------------------------
    */
    posInTab = { 0,10 };

    configTabs->tabs[1].wxs.addDrawable(optionCheckbox(TL("vsp.config.opt.vsync"), TL("vsp.config.opt.vsync.desc"), &g_config.vsync, &posInTab));

    UILabel* lbl3 = new UILabel(TL("vsp.config.opt.bganim"));
    lbl3->position = posInTab;
    configTabs->tabs[1].wxs.addDrawable(lbl3);
    UIDropdown* dd1 = new UIDropdown({ TL("vsp.config.opt.bganim.none"), TL("vsp.config.opt.bganim.sharp"), TL("vsp.config.opt.bganim.smooth"), TL("vsp.config.opt.bganim.sharpstatic"), TL("vsp.config.opt.bganim.smoothstatic") });
    dd1->position = { ixmax(lbl3->calcEndpoint().x + 30, posInTab.x + 200), posInTab.y };
    dd1->wxWidth = 180;
    dd1->setCallbackListener(CHECKBOX_ANIMATED_BACKGROUND, this);
    dd1->setTextToSelectedItem = true;
    dd1->text = g_config.animatedBackground < dd1->items.size() ? dd1->items[g_config.animatedBackground] : "--";
    configTabs->tabs[1].wxs.addDrawable(dd1);
    posInTab.y += 35;

    lbl4 = new UILabel(TL("vsp.config.opt.renderer"));
    lbl4->position = posInTab;
    configTabs->tabs[1].wxs.addDrawable(lbl4);
    dd2 = new UIDropdown(g_availableRenderersNow);
    dd2->position = { ixmax(lbl4->calcEndpoint().x + 30, posInTab.x + 100), posInTab.y };
    dd2->wxWidth = 180;
    dd2->setCallbackListener(CHECKBOX_RENDERER, this);
    dd2->setTextToSelectedItem = true;
    dd2->text = g_config.preferredRenderer != "" ? g_config.preferredRenderer : "<auto>";
    configTabs->tabs[1].wxs.addDrawable(dd2);
    posInTab.y += 35;

    configTabs->tabs[1].wxs.addDrawable(optionCheckbox(TL("vsp.config.opt.vfx"), TL("vsp.config.opt.vfx.desc"), &g_config.vfxEnabled, &posInTab));
    configTabs->tabs[1].wxs.addDrawable(optionCheckbox(TL("vsp.config.opt.cursor"), TL("vsp.config.opt.cursor.desc"), &g_config.overrideCursor, &posInTab));

    auto availableVisualConfs = g_getAvailableVisualConfigs();
    std::vector<std::string> visualConfNames = { getDefaultVisualConf()["meta/name"] };
    for (auto& meta : availableVisualConfs) {
        visualConfNames.push_back(meta.name);
    }
    lbl4 = new UILabel(TL("vsp.config.opt.visualconfig"));
    lbl4->position = posInTab;
    configTabs->tabs[1].wxs.addDrawable(lbl4);
    dd2 = new UIDropdown(visualConfNames);
    dd2->position = { ixmax(lbl4->calcEndpoint().x + 30, posInTab.x + 100), posInTab.y };
    dd2->wxWidth = 240;
    dd2->setTextToSelectedItem = true;
    dd2->onDropdownItemSelectedCallback = [this, availableVisualConfs](UIDropdown* dd, int index, std::string name) {
        if (index == 0) {   //default
            g_config.customVisualConfigPath = "";
        }
        else {
            index--;
            if (index >= 0 && index < availableVisualConfs.size()) {
                g_config.customVisualConfigPath = convertStringToUTF8OnWin32(availableVisualConfs[index].path);
            }
        }
        g_loadVisualConfig(convertStringOnWin32(g_config.customVisualConfigPath));
        g_reloadFonts();
    };
    dd2->text = fileNameFromPath(visualConfigValue("meta/name"));
    configTabs->tabs[1].wxs.addDrawable(dd2);
    posInTab.y += 35;

    /*
        -------------------------
        EDITOR TAB
        -------------------------
    */
    posInTab = { 0,10 };

	configTabs->tabs[2].wxs.addDrawable(optionCheckbox(TL("vsp.config.opt.opensavelocation"), "", &g_config.openSavedPath, &posInTab));
	configTabs->tabs[2].wxs.addDrawable(optionCheckbox(TL("vsp.config.opt.startrowcolidxat1"), "", &g_config.rowColIndexesStartAt1, &posInTab));
	configTabs->tabs[2].wxs.addDrawable(optionCheckbox(TL("vsp.config.opt.altscrolling"), "", &g_config.scrollWithTouchpad, &posInTab));

    UILabel* lbl2 = new UILabel(TL("vsp.config.opt.maxundocount"));
    lbl2->position = posInTab;
    configTabs->tabs[2].wxs.addDrawable(lbl2);
    UITextField* tf2 = new UITextField();
    tf2->isNumericField = true;
    tf2->position = XY{ posInTab.x + 10 + lbl2->statSize().x, posInTab.y};
    tf2->wxWidth = 80;
    tf2->setText(std::to_string(g_config.maxUndoHistory));
    tf2->setCallbackListener(TEXTFIELD_MAX_UNDO_HISTORY_SIZE, this);
    configTabs->tabs[2].wxs.addDrawable(tf2);
    posInTab.y += 35;

	configTabs->tabs[2].wxs.addDrawable(optionCheckbox(TL("vsp.config.opt.selectonlocktile"), TL("vsp.config.opt.selectonlocktile.desc"), &g_config.isolateRectOnLockTile, &posInTab));
	configTabs->tabs[2].wxs.addDrawable(optionCheckbox(TL("vsp.config.opt.lockfilltotiles"), TL("vsp.config.opt.lockfilltotiles.desc"), &g_config.fillToolTileBound, &posInTab));

    lbl2 = new UILabel(TL("vsp.config.opt.recoveryautosavetime"));
    lbl2->position = posInTab;
    configTabs->tabs[2].wxs.addDrawable(lbl2);
    tf2 = new UITextField();
    tf2->isNumericField = true;
    tf2->position = XY{ posInTab.x + 10 + lbl2->statSize().x, posInTab.y };
    tf2->wxWidth = 80;
    tf2->setText(std::to_string(g_config.autosaveInterval));
    tf2->setCallbackListener(TEXTFIELD_AUTOSAVE_INTERVAL, this);
    configTabs->tabs[2].wxs.addDrawable(tf2);
    posInTab.y += 35;
    lbl2 = new UILabel(TL("vsp.config.hint.recoveryautosaves"));
    lbl2->fontsize = 14;
    lbl2->color = { 255,255,255,0xa0 };
    lbl2->position = xySubtract(posInTab, {0,4});
    configTabs->tabs[2].wxs.addDrawable(lbl2);
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
    configTabs->tabs[3].wxs.addDrawable(keybindsPanel);


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
    btn->text = TL("vsp.config.opt.openappdata");
    btn->tooltip = TL("vsp.config.opt.openappdata.desc");
    btn->position = posInTab;
    btn->wxWidth = 270;
    btn->setCallbackListener(BUTTON_OPEN_CONFIG_DIR, this);
    configTabs->tabs[4].wxs.addDrawable(btn);
    posInTab.y += 35;

    btn = new UIButton();
    btn->text = TL("vsp.config.opt.associateexts");
    btn->tooltip = TL("vsp.config.opt.associateexts.desc");
    btn->position = posInTab;
    btn->wxWidth = 270;
    btn->onClickCallback = [this](UIButton*) {
        g_addPopup(new PopupChooseExtsToAssoc());
    };
    configTabs->tabs[4].wxs.addDrawable(btn);
    posInTab.y += 35;

    btn = new UIButton();
    btn->text = TL("vsp.config.opt.reloadfonts");
    btn->position = posInTab;
    btn->wxWidth = 270;
    btn->onClickCallback = [this](UIButton*) {
        g_reloadFonts();
        g_addNotification(SuccessShortNotification(TL("vsp.config.opt.fontsreloaded"), ""));
    };
    configTabs->tabs[4].wxs.addDrawable(btn);
    posInTab.y += 35;

#if __ANDROID__
    btn = new UIButton();
    btn->text = TL("vsp.config.opt.setfileaccess");
    btn->position = posInTab;
    btn->wxWidth = 270;
    btn->onClickCallback = [this](UIButton*) {
        platformRequestFileAccessPermissions();
    };
    configTabs->tabs[4].wxs.addDrawable(btn);
    posInTab.y += 35;
#endif


    

    UIButton* closeButton = actionButton(TL("vsp.cmn.close"), 140);
    closeButton->onClickCallback = [this](UIButton*) {
        bool visualConfigChanged = g_config.customVisualConfigPath != previousConfig.customVisualConfigPath;
        g_config = previousConfig;
        if (visualConfigChanged) {
            g_loadVisualConfig(convertStringOnWin32(g_config.customVisualConfigPath));
            g_reloadFonts();
        }
        closePopup();
    };

    UIButton* saveAndCloseButton = actionButton(TL("vsp.cmn.apply"), 140);
    saveAndCloseButton->onClickCallback = [this](UIButton*) {
        if (g_saveConfig()) {
            g_addNotification(SuccessShortNotification(TL("vsp.config.notif.saved"), ""));
            g_initOrDeinitRPCBasedOnConfig();
            closePopup();
        }
        else {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.config.notif.savefailed")));
        }
    };

    makeTitleAndDesc(TL("vsp.config.title"));
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
                    g_addNotification(ErrorNotification(TL("vsp.cmn.error"), std::format("{} is a reserved key.", std::string(SDL_GetScancodeName((SDL_Scancode)targetKey)))));
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
    if (evt_id == BUTTON_OPEN_CONFIG_DIR) {
        platformOpenFileLocation(platformEnsureDirAndGetConfigFilePath());
    }
    else if (evt_id >= 1000) {
        bindingKey = true;
        bindingKeyIndex = evt_id - 1000;
        keybindButtons[bindingKeyIndex].second->text = std::format("{} [SET KEY... (ESC: cancel, LShift: clear)]", keybindButtons[bindingKeyIndex].first.name);
    }
}

void PopupGlobalConfig::eventTextInput(int evt_id, std::string text)
{
    switch (evt_id) {
        case TEXTFIELD_MAX_UNDO_HISTORY_SIZE:
            try {
                g_config.maxUndoHistory = std::stoi(text);
            }
            catch (std::exception&) {
                g_config.maxUndoHistory = previousConfig.maxUndoHistory;
            }
            break;
        case TEXTFIELD_AUTOSAVE_INTERVAL:
            try {
                g_config.autosaveInterval = std::stoi(text);
            }
            catch (std::exception&) {
                g_config.autosaveInterval = previousConfig.autosaveInterval;
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
    else if (evt_id == DROPDOWN_LANGUAGE) {
        g_config.language = langLocNames[index];
        updateLanguageCredit();
    }
}

void PopupGlobalConfig::updateKeybindButtonText(std::pair<KeybindConf, UIButton*> t)
{
    t.second->text = std::format("{}    ({})", t.first.name, SDL_GetScancodeName(*t.first.target));
}

void PopupGlobalConfig::updateLanguageCredit()
{
    if (getLocalizations().contains(g_config.language)) {
		std::string langCredit = getLocalizations()[g_config.language].langCredit;
        if (g_config.language != "en-us") {
            langCredit += std::format("\n[completion: {:.2f}%]", g_getLocCompletionPercentage(g_config.language) * 100);
        }
        languageCredit->setText(langCredit);
    }
    else {
        languageCredit->setText("");
    }
}

UICheckbox* PopupGlobalConfig::optionCheckbox(std::string name, std::string tooltip, bool* target, XY* position)
{
    UICheckbox* cb7 = new UICheckbox(name, target);
    cb7->position = *position;
    if (tooltip != "") {
        cb7->checkbox->tooltip = tooltip;
    }
    position->y += 35;
    return cb7;
}
