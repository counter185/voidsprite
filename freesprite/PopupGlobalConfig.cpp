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
#include "discord_rpc.h"
#include "PopupChooseExtsToAssoc.h"
#include "keybinds.h"
#include "vfx.h"

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

void KeybindButton::render(XY pos)
{
    UIButton::render(pos);
    g_fnt->RenderString(keybindText, pos.x + wxWidth / 2, pos.y + 2, keybindTextColor, 15);
}

PopupGlobalConfig::PopupGlobalConfig()
{
    //do not do   previousConfig = g_config  it will crash 
    previousConfig = GlobalConfig(g_config);
    previousKeybinds = g_keybindManager.serializeKeybinds();

    wxHeight = 400;
    wxWidth = 850;

    TabbedView* configTabs = new TabbedView({ 
        {TL("vsp.config.tab.general")}, 
        {TL("vsp.config.tab.visual")}, 
        {TL("vsp.config.tab.editor")}, 
        {TL("vsp.config.tab.keybinds")}, 
        {TL("vsp.config.tab.misc")},
#if _DEBUG
        {"DEBUG"}
#endif
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
    if (platformSupportsFeature(VSP_FEATURE_DISCORD_RPC)){
        configTabs->tabs[0].wxs.addDrawable(optionCheckbox(TL("vsp.config.opt.discordrpc"), TL("vsp.config.opt.discordrpc.desc"), &g_config.useDiscordRPC, &posInTab));
    }
    configTabs->tabs[0].wxs.addDrawable(optionCheckbox(TL("vsp.config.opt.usesystemfilepicker"), TL("vsp.config.opt.usesystemfilepicker.desc"), &g_config.useSystemFileDialog, &posInTab));
    configTabs->tabs[0].wxs.addDrawable(optionCheckbox(TL("vsp.config.opt.opensavelocation"), "", &g_config.openSavedPath, &posInTab));
    if (platformSupportsFeature(VSP_FEATURE_WEB_FETCH)) {
        configTabs->tabs[0].wxs.addDrawable(optionCheckbox(TL("vsp.config.opt.checkupdates"), TL("vsp.config.opt.checkupdates.desc"), &g_config.checkUpdates, &posInTab));
    }
    if (platformSupportsFeature(VSP_FEATURE_INSTANCE_IPC)) {
        configTabs->tabs[0].wxs.addDrawable(optionCheckbox(TL("vsp.config.opt.singleinstance"), TL("vsp.config.opt.singleinstance.desc"), &g_config.singleInstance, &posInTab));
    }

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
    ScrollingPanel* visualSettingsPanel = new ScrollingPanel();
    visualSettingsPanel->position = { 0,0 };
    visualSettingsPanel->wxWidth = wxWidth - 20;
    visualSettingsPanel->wxHeight = wxHeight - 140;
    visualSettingsPanel->scrollVertically = true;
    visualSettingsPanel->scrollHorizontally = false;
    configTabs->tabs[1].wxs.addDrawable(visualSettingsPanel);

    posInTab = { 0,10 };

    visualSettingsPanel->subWidgets.addDrawable(optionCheckbox(TL("vsp.config.opt.vsync"), TL("vsp.config.opt.vsync.desc"), &g_config.vsync, &posInTab));

    UILabel* lbl3 = new UILabel(TL("vsp.config.opt.bganim"));
    lbl3->position = posInTab;
    visualSettingsPanel->subWidgets.addDrawable(lbl3);
    UIDropdown* dd1 = new UIDropdown({ TL("vsp.config.opt.bganim.none"), TL("vsp.config.opt.bganim.sharp"), TL("vsp.config.opt.bganim.smooth"), TL("vsp.config.opt.bganim.sharpstatic"), TL("vsp.config.opt.bganim.smoothstatic") });
    dd1->position = { ixmax(lbl3->calcEndpoint().x + 30, posInTab.x + 200), posInTab.y };
    dd1->wxWidth = 180;
    dd1->setCallbackListener(CHECKBOX_ANIMATED_BACKGROUND, this);
    dd1->setTextToSelectedItem = true;
    dd1->text = g_config.animatedBackground < dd1->items.size() ? dd1->items[g_config.animatedBackground] : "--";
    visualSettingsPanel->subWidgets.addDrawable(dd1);
    posInTab.y += 35;

    UILabel* lbl7 = new UILabel(TL("vsp.config.opt.powersaver"));
    lbl7->position = posInTab;
    visualSettingsPanel->subWidgets.addDrawable(lbl7);
    UIDropdown* dd3 = new UIDropdown({
        TL("vsp.config.opt.powersaver.off"),
        TL("vsp.config.opt.powersaver.mid"),
        TL("vsp.config.opt.powersaver.max"),
        TL("vsp.config.opt.powersaver.auto")
    });
    dd3->position = { ixmax(lbl7->calcEndpoint().x + 30, posInTab.x + 200), posInTab.y };
    dd3->wxWidth = 180;
    dd3->onDropdownItemSelectedCallback = [&](UIDropdown* dd, int index, std::string) {
        g_config.powerSaverLevel = index;
    };
    dd3->setTextToSelectedItem = true;
    dd3->text = g_config.powerSaverLevel < dd3->items.size() ? dd3->items[g_config.powerSaverLevel] : "--";
    visualSettingsPanel->subWidgets.addDrawable(dd3);
    posInTab.y += 35;

    lbl4 = new UILabel(TL("vsp.config.opt.renderer"));
    lbl4->position = posInTab;
    visualSettingsPanel->subWidgets.addDrawable(lbl4);
    dd2 = new UIDropdown(g_availableRenderersNow);
    dd2->position = { ixmax(lbl4->calcEndpoint().x + 30, posInTab.x + 100), posInTab.y };
    dd2->wxWidth = 180;
    dd2->setCallbackListener(CHECKBOX_RENDERER, this);
    dd2->setTextToSelectedItem = true;
    dd2->text = g_config.preferredRenderer != "" ? g_config.preferredRenderer : "<auto>";
    visualSettingsPanel->subWidgets.addDrawable(dd2);
    posInTab.y += 35;

    visualSettingsPanel->subWidgets.addDrawable(optionCheckbox(TL("vsp.config.opt.vfx"), TL("vsp.config.opt.vfx.desc"), &g_config.vfxEnabled, &posInTab));
    visualSettingsPanel->subWidgets.addDrawable(optionCheckbox(TL("vsp.config.opt.showfps"), TL("vsp.config.opt.showfps.desc"), &g_config.showFPS, &posInTab));
    visualSettingsPanel->subWidgets.addDrawable(optionCheckbox(TL("vsp.config.opt.cursor"), TL("vsp.config.opt.cursor.desc"), &g_config.overrideCursor, &posInTab));

    auto availableVisualConfs = g_getAvailableVisualConfigs();
    std::vector<std::string> visualConfNames = { getDefaultVisualConf()["meta/name"] };
    for (auto& meta : availableVisualConfs) {
        visualConfNames.push_back(meta.name);
    }
    lbl4 = new UILabel(TL("vsp.config.opt.visualconfig"));
    lbl4->position = posInTab;
    visualSettingsPanel->subWidgets.addDrawable(lbl4);
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
    visualSettingsPanel->subWidgets.addDrawable(dd2);
    posInTab.y += 35;

    /*
        -------------------------
        EDITOR TAB
        -------------------------
    */

    ScrollingPanel* editorSettingsPanel = new ScrollingPanel();
    editorSettingsPanel->position = { 0,0 };
    editorSettingsPanel->wxWidth = wxWidth - 20;
    editorSettingsPanel->wxHeight = wxHeight - 140;
    editorSettingsPanel->scrollVertically = true;
    editorSettingsPanel->scrollHorizontally = false;
    configTabs->tabs[2].wxs.addDrawable(editorSettingsPanel);

    posInTab = { 0,10 };

    editorSettingsPanel->subWidgets.addDrawable(optionCheckbox(TL("vsp.config.opt.startrowcolidxat1"), "", &g_config.rowColIndexesStartAt1, &posInTab));
    editorSettingsPanel->subWidgets.addDrawable(optionCheckbox(TL("vsp.config.opt.altscrolling"), "", &g_config.scrollWithTouchpad, &posInTab));

    UILabel* lbl2 = new UILabel(TL("vsp.config.opt.maxundocount"));
    lbl2->position = posInTab;
    editorSettingsPanel->subWidgets.addDrawable(lbl2);
    UITextField* tf2 = new UITextField();
    tf2->isNumericField = true;
    tf2->position = XY{ posInTab.x + 10 + lbl2->statSize().x, posInTab.y};
    tf2->wxWidth = 80;
    tf2->setText(std::to_string(g_config.maxUndoHistory));
    tf2->setCallbackListener(TEXTFIELD_MAX_UNDO_HISTORY_SIZE, this);
    editorSettingsPanel->subWidgets.addDrawable(tf2);
    posInTab.y += 35;

    editorSettingsPanel->subWidgets.addDrawable(optionCheckbox(TL("vsp.config.opt.selectonlocktile"), TL("vsp.config.opt.selectonlocktile.desc"), &g_config.isolateRectOnLockTile, &posInTab));
    editorSettingsPanel->subWidgets.addDrawable(optionCheckbox(TL("vsp.config.opt.lockfilltotiles"), TL("vsp.config.opt.lockfilltotiles.desc"), &g_config.fillToolTileBound, &posInTab));
    editorSettingsPanel->subWidgets.addDrawable(optionCheckbox(TL("vsp.config.opt.brushcolorpreview"), TL("vsp.config.opt.brushcolorpreview.desc"), &g_config.brushColorPreview, &posInTab));

    lbl2 = new UILabel(TL("vsp.config.opt.recoveryautosavetime"));
    lbl2->position = posInTab;
    editorSettingsPanel->subWidgets.addDrawable(lbl2);
    tf2 = new UITextField();
    tf2->isNumericField = true;
    tf2->position = XY{ posInTab.x + 10 + lbl2->statSize().x, posInTab.y };
    tf2->wxWidth = 80;
    tf2->setText(std::to_string(g_config.autosaveInterval));
    tf2->setCallbackListener(TEXTFIELD_AUTOSAVE_INTERVAL, this);
    editorSettingsPanel->subWidgets.addDrawable(tf2);
    posInTab.y += 35;
    lbl2 = new UILabel(TL("vsp.config.hint.recoveryautosaves"));
    lbl2->fontsize = 14;
    lbl2->color = { 255,255,255,0xa0 };
    lbl2->position = xySubtract(posInTab, {0,4});
    editorSettingsPanel->subWidgets.addDrawable(lbl2);
    posInTab.y += 35;

    editorSettingsPanel->subWidgets.addDrawable(optionCheckbox(TL("vsp.config.opt.showpenpressure"), TL("vsp.config.opt.showpenpressure.desc"), &g_config.showPenPressure, &posInTab));

    /*
        -------------------------
        KEYBINDS TAB
        -------------------------
    */
    posInTab = { 0,10 };
    keybindsPanel = new ScrollingPanel();
    keybindsPanel->position = posInTab;
    keybindsPanel->wxWidth = wxWidth - 20;
    keybindsPanel->wxHeight = wxHeight - 140;
    keybindsPanel->scrollVertically = true;
    keybindsPanel->scrollHorizontally = false;
    configTabs->tabs[3].wxs.addDrawable(keybindsPanel);

    createKeybindButtons();


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

    if (platformSupportsFeature(VSP_FEATURE_FILE_ASSOC)) {
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
    }

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

    /*
        -------------------------
        DEBUG TAB
        -------------------------
    */
#if _DEBUG
    posInTab = { 0,10 };
    configTabs->tabs[5].wxs.addDrawable(optionCheckbox("Show scroll panel bounds", "", &g_debugConfig.debugShowScrollPanelBounds, &posInTab));
    configTabs->tabs[5].wxs.addDrawable(optionCheckbox("Debug color slider gradient bounds", "", &g_debugConfig.debugColorSliderGradients, &posInTab));
#endif
    

    UIButton* closeButton = actionButton(TL("vsp.cmn.close"), 140);
    closeButton->onClickCallback = [this](UIButton*) {
        bool visualConfigChanged = g_config.customVisualConfigPath != previousConfig.customVisualConfigPath;
        g_config = previousConfig;
        g_keybindManager.deserializeKeybinds(previousKeybinds);
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

void PopupGlobalConfig::render()
{
    BasePopup::render();
    if (bindingKey) {
        XY origin = getPopupOrigin();
        SDL_Rect bgRect = SDL_Rect{ origin.x, origin.y, wxWidth, (int)(wxHeight * XM1PW3P1(startTimer.percentElapsedTime(300))) };
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0xf0 * keyBindingTimer.percentElapsedTime(200));
        SDL_RenderFillRect(g_rd, &bgRect);

        static std::string tlPressAnyKeyToBind = TL("vsp.config.bindkey.pressanykey");
        static std::string tlESCToClear = TL("vsp.config.bindkey.esctoclear");

        g_fnt->RenderString(
            std::format("{}\n   {}\n\n    {} -> {}{}...\n\n{}", 
                tlPressAnyKeyToBind, 
                currentBindTarget->displayName, 
                currentBindTarget->getKeyComboName(), 
                g_ctrlModifier ? "Ctrl + " : "", 
                g_shiftModifier ? "Shift + " : "", 
                tlESCToClear),
            bgRect.x + 10, bgRect.y + 30, {255,255,255,255}, 20
        );

        XY timeoutLineOrigin = { bgRect.x, bgRect.y + bgRect.h - 30 };
        XY timeoutLineEnd = xyAdd(timeoutLineOrigin, { bgRect.w, 0 });

        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 255);
        drawLine(timeoutLineOrigin, timeoutLineEnd, 1.0 - keyBindingTimer.percentElapsedTime(7000));

        if (keyBindingTimer.elapsedTime() > 7000) {
            bindingKey = false;
            playPopupCloseVFX();
        }
    }
}

void PopupGlobalConfig::takeInput(SDL_Event evt)
{
    if (bindingKey) {
        if (evt.type == SDL_EVENT_KEY_DOWN) {
            SDL_Scancode key = evt.key.scancode;
            if (key != SDL_SCANCODE_LSHIFT && key != SDL_SCANCODE_LCTRL) {
                if (key == SDL_SCANCODE_ESCAPE) {
                    bindingKey = false;
                    playPopupCloseVFX();
                    currentBindTarget->unassign();
                    updateKeybindButtonText(currentBindTarget, currentBindTargetButton);
                }
                else if (std::find(reservedKeysNow.begin(), reservedKeysNow.end(), key) != reservedKeysNow.end()
                    || std::find(g_keybindManager.globalReservedKeys.begin(), g_keybindManager.globalReservedKeys.end(), key) != g_keybindManager.globalReservedKeys.end()) {
                    g_addNotification(ErrorNotification(TL("vsp.cmn.error"), std::format("{} is a reserved key.", std::string(SDL_GetScancodeName(key)))));
                    bindingKey = false;
                    playPopupCloseVFX();
                }
                else {
                    bindingKey = false;
                    playPopupCloseVFX();

                    bool ctrl = g_ctrlModifier;
                    bool shift = g_shiftModifier;

                    bool updateAll = false;

                    if (currentBindTargetRegion->regionKey != "global") {
                        updateAll =
                            currentBindTargetRegion->unassignAllWith(key, ctrl, shift)
                            || g_keybindManager.regions["global"].unassignAllWith(key, ctrl, shift);
                    }
                    else {
                        for (auto& [regionName, keyRegion] : g_keybindManager.regions) {
                            updateAll |= keyRegion.unassignAllWith(key, ctrl, shift);
                        }
                    }

                    currentBindTarget->key = key;
                    currentBindTarget->ctrl = ctrl;
                    currentBindTarget->shift = shift;
                    if (updateAll) {
                        createKeybindButtons();
                    }
                    else {
                        updateKeybindButtonText(currentBindTarget, currentBindTargetButton);
                    }
                }
            }
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

void PopupGlobalConfig::createKeybindButtons()
{
    keybindsPanel->subWidgets.freeAllDrawables();

    int y = 0;
    for (auto& [regionName, keyRegion] : g_keybindManager.regions) {
        UILabel* regionLabel = new UILabel(keyRegion.displayName);
        regionLabel->fontsize = 20;
        regionLabel->position = { 10, y };
        y += 30;
        keybindsPanel->subWidgets.addDrawable(regionLabel);
        auto reservedKeys = keyRegion.reservedKeys;

        for (std::string& keyIDInOrder : keyRegion.orderInSettings) {
            KeybindButton* btn = new KeybindButton();
            btn->wxWidth = keybindsPanel->wxWidth - 20;
            btn->wxHeight = 30;
            btn->icon = keyRegion.keybinds[keyIDInOrder].icon;
            updateKeybindButtonText(&keyRegion.keybinds[keyIDInOrder], btn);
            btn->position = { 0, y };
            y += 30;
            KeyCombo* keyComboPtr = &keyRegion.keybinds[keyIDInOrder];
            KeybindRegion* regionPtr = &keyRegion;
            btn->onClickCallback = [this, keyComboPtr, regionPtr, reservedKeys](UIButton* b) {
                if (!bindingKey) {
                    bindingKey = true;
                    this->reservedKeysNow = reservedKeys;
                    this->currentBindTarget = keyComboPtr;
                    this->currentBindTargetRegion = regionPtr;
                    this->currentBindTargetButton = (KeybindButton*)b;
                    keyBindingTimer.start();
                }
            };
            keybindsPanel->subWidgets.addDrawable(btn);
        }
    }
}

void PopupGlobalConfig::updateKeybindButtonText(KeyCombo* keycombo, KeybindButton* btn)
{
    btn->text = keycombo->displayName;
    btn->keybindText = keycombo->getKeyComboName();
    btn->keybindTextColor = !keycombo->isUnassigned() ? SDL_Color{ 0x97,0xf1,0xff,0xf0 }
                                                      : SDL_Color{ 255,255,255, 0xa0 };
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
