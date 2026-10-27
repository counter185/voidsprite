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
#include "UISlider.h"
#include "sdk_pluginloader.h"
#include "UIStackPanel.h"
#include "PopupYesNo.h"
#include "UIColorInputField.h"
#include "thumbnail_loader.h"
#include "main.h"

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
    TEXTFIELD_AUTOSAVE_INTERVAL,
    CHECKBOX_ROWCOLS_START_AT_1,
    DROPDOWN_LANGUAGE,
    DROPDOWN_VISUAL_CONFIG,
};

void KeybindButton::render(XY pos)
{
    UIButton::render(pos);
    g_fnt->RenderString(keybindText, pos.x + wxWidth / 2, pos.y + 2, keybindTextColor, 15);
    g_fnt->RenderString(secondKeybindText, pos.x + wxWidth / 4*3, pos.y + 2, secondKeybindTextColor, 15);
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
        {TL("vsp.config.tab.plugins")}, 
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
    ScrollingPanel* generalSettingsPanel = new ScrollingPanel();
    generalSettingsPanel->position = { 0,0 };
    generalSettingsPanel->wxWidth = wxWidth - 20;
    generalSettingsPanel->wxHeight = wxHeight - 140;
    generalSettingsPanel->scrollVertically = true;
    generalSettingsPanel->scrollHorizontally = false;
    configTabs->tabs[0].add(generalSettingsPanel);
    XY posInTab = { 0,10 };
    
    UIStackPanel* tabGeneralStack = UIStackPanel::Vertical(5, {}, {0, 10});
    generalSettingsPanel->subWidgets.addDrawable(tabGeneralStack);

    tabGeneralStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.pngextdata"), TL("vsp.config.opt.pngextdata.desc"), &g_config.saveLoadFlatImageExtData));
    tabGeneralStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.saverefs"), TL("vsp.config.opt.saverefs.desc"), &g_config.saveLoadReferences));
    if (VSP_DISCORD_RPC){
        tabGeneralStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.discordrpc"), TL("vsp.config.opt.discordrpc.desc"), &g_config.useDiscordRPC));
    }
    tabGeneralStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.usesystemfilepicker"), TL("vsp.config.opt.usesystemfilepicker.desc"), &g_config.useSystemFileDialog));
    tabGeneralStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.opensavelocation"), "", &g_config.openSavedPath));
    if (platformSupportsFeature(VSP_FEATURE_WEB_FETCH)) {
        tabGeneralStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.checkupdates"), TL("vsp.config.opt.checkupdates.desc"), &g_config.checkUpdates));
    }
    if (platformSupportsFeature(VSP_FEATURE_INSTANCE_IPC)) {
        tabGeneralStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.singleinstance"), TL("vsp.config.opt.singleinstance.desc"), &g_config.singleInstance));
    }
    tabGeneralStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.longerrornotifs"), TL("vsp.config.opt.longerrornotifs.desc"), &g_config.longErrorNotifs));
    tabGeneralStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.showsysclock"), "", &g_config.showSystemClock));

    tabGeneralStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.showfeaturedugc"), TL("vsp.config.opt.showfeaturedugc.desc"), &g_config.showFeaturedUGC));

    std::vector<std::string> langNames;
    for (auto& loc : getLocalizations()) {
        langLocNames.push_back(loc.first);
        langNames.push_back(loc.second.langName);
    }
    UIDropdown* dd2 = new UIDropdown(langNames);
    dd2->wxWidth = 180;
    dd2->setCallbackListener(DROPDOWN_LANGUAGE, this);
    dd2->setTextToSelectedItem = true;
    dd2->text = getLocalizations().contains(g_config.language) ? getLocalizations()[g_config.language].langName : "???";

    languageCredit = new UILabel("");
    languageCredit->fontsize = 16;
    languageCredit->color = { 255,255,255,0xa0 };
    tabGeneralStack->addWidgetNoRecalc(UIStackPanel::Vertical(0, {
        UIStackPanel::Horizontal(200, { new UILabel(TL("vsp.config.opt.lang")), dd2} ),
        languageCredit
    }));

    tabGeneralStack->recalculateLayout();
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
    configTabs->tabs[1].add(visualSettingsPanel);

    UIStackPanel* tabVisualStack = UIStackPanel::Vertical(5, {}, { 0, 10 });
    visualSettingsPanel->subWidgets.addDrawable(tabVisualStack);

    HAlign* visualTabHAlign = new HAlign();

    tabVisualStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.vsync"), TL("vsp.config.opt.vsync.desc"), &g_config.vsync));

    uiScaleWhole = (int)(g_config.uiScale * 100);
    tabVisualStack->addWidgetNoRecalc(optionNumberInput("UI scale [BETA] (restart required)", "", &uiScaleWhole, 50, 300, [](int v) {
        g_config.uiScale = v / 100.0;
    }));

    UIDropdown* dd1 = new UIDropdown({ TL("vsp.config.opt.bganim.none"), TL("vsp.config.opt.bganim.sharp"), TL("vsp.config.opt.bganim.smooth"), TL("vsp.config.opt.bganim.sharpstatic"), TL("vsp.config.opt.bganim.smoothstatic") });
    dd1->wxWidth = 180;
    dd1->setCallbackListener(CHECKBOX_ANIMATED_BACKGROUND, this);
    dd1->setTextToSelectedItem = true;
    dd1->text = g_config.animatedBackground < dd1->items.size() ? dd1->items[g_config.animatedBackground] : "--";
    tabVisualStack->addWidgetNoRecalc(UIStackPanel::Horizontal(50, 
        { new UILabel(TL("vsp.config.opt.bganim")), visualTabHAlign->alignPoint(), dd1 }));


    UIDropdown* dd3 = new UIDropdown({
        TL("vsp.config.opt.powersaver.off"),
        TL("vsp.config.opt.powersaver.mid"),
        TL("vsp.config.opt.powersaver.max"),
        TL("vsp.config.opt.powersaver.auto")
    });
    dd3->wxWidth = 180;
    dd3->onDropdownItemSelectedCallback = [&](UIDropdown* dd, int index, std::string) {
        g_config.powerSaverLevel = index;
    };
    dd3->setTextToSelectedItem = true;
    dd3->text = g_config.powerSaverLevel < dd3->items.size() ? dd3->items[g_config.powerSaverLevel] : "--";
    tabVisualStack->addWidgetNoRecalc(UIStackPanel::Horizontal(50, 
        { new UILabel(TL("vsp.config.opt.powersaver")), visualTabHAlign->alignPoint(), dd3 }));


    dd2 = new UIDropdown(joinVectors({ {"<auto>"}, g_availableRenderersNow }));
    dd2->wxWidth = 180;
    dd2->onDropdownItemSelectedCallback = [this](UIDropdown* dd, int index, std::string name) {
        if (name == "vulkan") {
            g_addPopup(PopupYesNo::ConfirmPrompt("Warning",
                "The Vulkan renderer in SDL is currently broken and has heavy visual issues.\n"
                "Expect to see a lot of flashing visuals and broken graphics.\n"
                "\n\nEnable Vulkan anyway?",
                [name, this, dd]() {
                    g_config.preferredRenderer = name;
                    dd->text = g_config.preferredRenderer != "" ? g_config.preferredRenderer : "<auto>";
                }
            ));
        }
        else {
            g_config.preferredRenderer = name == "<auto>" ? "" : name;
        }
    };
    dd2->setTextToSelectedItem = true;
    dd2->text = g_config.preferredRenderer != "" ? g_config.preferredRenderer : "<auto>";
    tabVisualStack->addWidgetNoRecalc(UIStackPanel::Horizontal(50, 
        { new UILabel(TL("vsp.config.opt.renderer:v2")), visualTabHAlign->alignPoint(), dd2 }));


    tabVisualStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.vfx"), TL("vsp.config.opt.vfx.desc"), &g_config.vfxEnabled));
    tabVisualStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.showfps"), TL("vsp.config.opt.showfps.desc"), &g_config.showFPS));
    tabVisualStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.cursor"), TL("vsp.config.opt.cursor.desc"), &g_config.overrideCursor));
    tabVisualStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.autoscale"), TL("vsp.config.opt.autoscale.desc"), &g_config.autoViewportScale));
    tabVisualStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.acrylicpanels"), TL("vsp.config.opt.acrylicpanels.desc"), &g_config.acrylicPanels));
#if VSP_PLATFORM != VSP_PLATFORM_ANDROID && VSP_PLATFORM != VSP_PLATFORM_EMSCRIPTEN
    auto framelessChk = optionCheckbox(TL("vsp.config.opt.framelesswindow"), TL("vsp.config.opt.framelesswindow.desc"), &g_config.customWindowFrame);
    framelessChk->onStateChangeCallback = [](...) { main_updateFramelessWindows(); };
    tabVisualStack->addWidgetNoRecalc(framelessChk);

    std::vector<std::string> framelessStyles = {"Windows", "KDE"};
    UIDropdown* framelessStyleDropdown = new UIDropdown(framelessStyles);
    framelessStyleDropdown->setTextToSelectedItem = true;
    framelessStyleDropdown->wxWidth = 120;
    framelessStyleDropdown->text = framelessStyles[framelessStyles.size() > (u32)g_config.framelessWindowButtonStyle ? g_config.framelessWindowButtonStyle : 0];
    framelessStyleDropdown->onDropdownItemSelectedCallback = [](UIDropdown*, int i, std::string) { g_config.framelessWindowButtonStyle = i; }; 
    tabVisualStack->addWidgetNoRecalc(UIStackPanel::Horizontal(20, 
        {Panel::Space(5,1), new UILabel("Frameless window button style"), visualTabHAlign->alignPoint(), framelessStyleDropdown}));
#endif

    auto availableVisualConfs = g_getAvailableVisualConfigs();
    std::vector<std::string> visualConfNames = { getDefaultVisualConf()["meta/name"] };
    for (auto& meta : availableVisualConfs) {
        visualConfNames.push_back(meta.name);
    }
    dd2 = new UIDropdown(visualConfNames);
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
    tabVisualStack->addWidgetNoRecalc(UIStackPanel::Horizontal(20, 
        {new UILabel(TL("vsp.config.opt.visualconfig")), visualTabHAlign->alignPoint(), dd2}));


    UICheckbox* smoothFontCheckbox = optionCheckbox(TL("vsp.config.opt.smoothfonts"), TL("vsp.config.opt.smoothfonts.desc"), &g_config.smoothFonts, &posInTab);
    smoothFontCheckbox->onStateChangeCallback = [this](UICheckbox* cb, bool newState) { g_reloadFonts();};
    tabVisualStack->addWidgetNoRecalc(smoothFontCheckbox);

    tabVisualStack->recalculateLayout();

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
    configTabs->tabs[2].add(editorSettingsPanel);

    UIStackPanel* tabEditorStack = UIStackPanel::Vertical(5, {}, { 0, 10 });
    editorSettingsPanel->subWidgets.addDrawable(tabEditorStack);


    tabEditorStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.startrowcolidxat1"), "", &g_config.rowColIndexesStartAt1));
    tabEditorStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.altscrolling"), "", &g_config.scrollWithTouchpad));

    tabEditorStack->addWidgetNoRecalc(optionNumberInput(TL("vsp.config.opt.maxundocount"), "", &g_config.maxUndoHistory, 1, 150));

    tabEditorStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.selectonlocktile"), TL("vsp.config.opt.selectonlocktile.desc"), &g_config.isolateRectOnLockTile));
    tabEditorStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.lockfilltotiles"), TL("vsp.config.opt.lockfilltotiles.desc"), &g_config.fillToolTileBound));
    tabEditorStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.brushcolorpreview"), TL("vsp.config.opt.brushcolorpreview.desc"), &g_config.brushColorPreview));
    tabEditorStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.compacteditor"), TL("vsp.config.opt.compacteditor.desc"), &g_config.compactEditor));
    tabEditorStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.huewheel"), TL("vsp.config.opt.huewheel.desc"), &g_config.hueWheelInsteadOfSlider));
    tabEditorStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.showgitrepo"), TL("vsp.config.opt.showgitrepo.desc"), &g_config.editorShowGitRepo));
    tabEditorStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.alwaysopentouchpanel"), TL("vsp.config.opt.alwaysopentouchpanel.desc"), &g_config.editorAlwaysOpenTouchPanel));

    UILabel* lbl2 = new UILabel(TL("vsp.config.hint.recoveryautosaves"), {}, 14);
    lbl2->color = { 255,255,255,0xa0 };
    tabEditorStack->addWidgetNoRecalc(UIStackPanel::Vertical(0, {
        optionNumberInput(TL("vsp.config.opt.recoveryautosavetime"), "", &g_config.autosaveInterval, 0, 7200), 
        lbl2
    }));


    tabEditorStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.showpenpressure"), TL("vsp.config.opt.showpenpressure.desc"), &g_config.showPenPressure));

    tabEditorStack->addWidgetNoRecalc(optionNumberInput(TL("vsp.config.opt.canvaszoomsens"), "", &g_config.canvasZoomSensitivity, 1, 10));

    tabEditorStack->addWidgetNoRecalc(optionNumberInput(TL("vsp.config.opt.linesnapmaxaspect"), "", &g_config.lineSnapMaxAspect, 1, 60));

    UIStackPanel* stack = UIStackPanel::Horizontal(4, {
        new UILabel(TL("vsp.config.opt.tracecolormods")),
        Panel::Space(20,2),
        optionColorU32Input("Back", "", &g_config.backtraceColor, false),
        Panel::Space(8,2),
        optionColorU32Input("Next", "", &g_config.fwdtraceColor, false),
    });
    tabEditorStack->addWidgetNoRecalc(stack);


    tabEditorStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.enablecheckerboard"), "", &g_config.enableCheckerboard));
    tabEditorStack->addWidgetNoRecalc(UIStackPanel::Horizontal(0, {
        Panel::Space(20, 1),
        UIStackPanel::Vertical(5, {
            optionNumberInput(TL("vsp.config.opt.checkerboardsize"), "", &g_config.checkerboardSize, 4, 512),
            optionCheckbox(TL("vsp.config.opt.checkerboardscalewithcanvas"), "", &g_config.checkerboardScaleWithCanvas),
            UIStackPanel::Horizontal(4, {
                new UILabel(TL("vsp.config.opt.checkerboardcolors")),
                Panel::Space(20,2),
                optionColorU32Input("Primary", "", &g_config.checkerboardPrimary, true),
                Panel::Space(8,2),
                optionColorU32Input("Secondary", "", &g_config.checkerboardSecondary, true),
            })
        })
    }));

    tabEditorStack->addWidgetNoRecalc(optionCheckbox(TL("vsp.config.opt.reseteraser"), "", &g_config.resetEraserOnToolSwitch, &posInTab));

    tabEditorStack->recalculateLayout();

    /*
        -------------------------
        KEYBINDS TAB
        -------------------------
    */
    posInTab = { 0,10 };
    configTabs->tabs[3].add(new UILabel(TL("vsp.cmn.search"), posInTab));
    keybindSearchField = new UITextField("");
    keybindSearchField->position = xyAdd(posInTab, {120,0});
    keybindSearchField->onTextChangedCallback = [this](UITextField* tf, std::string newText) {
        createKeybindButtons(newText);
        keybindsPanel->scrollOffset.y = 0;
    };
    configTabs->tabs[3].add(keybindSearchField);
    posInTab.y += 40;

    keybindsPanel = new ScrollingPanel();
    keybindsPanel->position = posInTab;
    keybindsPanel->wxWidth = wxWidth - 20;
    keybindsPanel->wxHeight = wxHeight - 130 - posInTab.y;
    keybindsPanel->scrollVertically = true;
    keybindsPanel->scrollHorizontally = false;
    configTabs->tabs[3].add(keybindsPanel);

    keybindsStack = UIStackPanel::Vertical(0, {});
    keybindsStack->position = { 0,0 };
    keybindsStack->passThroughMouse = true;
    keybindsStack->takeMouseWheelEvents = false;
    keybindsPanel->subWidgets.addDrawable(keybindsStack);

    createKeybindButtons(keybindSearchField->getText());

    /*
        -------------------------
        PLUGINS TAB
        -------------------------
    */
    posInTab = { 0,10 };
    ScrollingPanel* pluginsPanel = new ScrollingPanel();
    pluginsPanel->position = posInTab;
    pluginsPanel->wxWidth = wxWidth - 20;
    pluginsPanel->wxHeight = wxHeight - 140;
    pluginsPanel->scrollVertically = true;
    pluginsPanel->scrollHorizontally = false;
    configTabs->tabs[4].add(pluginsPanel);

    XY posInPanel = { 5,5 };
    if (g_loadedPlugins.empty()) {
        pluginsPanel->subWidgets.addDrawable(new UILabel(TL("vsp.config.tab.plugins.noplugins"), posInPanel));
    }
    else {
        for (auto& pluginInfo : g_loadedPlugins) {
            UILabel* lbl = new UILabel(frmt("{} v{}", pluginInfo.name, pluginInfo.version), posInPanel);
            lbl->fontsize = 18;
            pluginsPanel->subWidgets.addDrawable(lbl);
            posInPanel.y += 25;

            UILabel* authors = new UILabel(frmt("by {}", pluginInfo.authors));
            authors->position = xyAdd(lbl->calcEndpoint(), {10, 0});
            authors->fontsize = 16;
            authors->color = { 255,255,255,0x80 };
            pluginsPanel->subWidgets.addDrawable(authors);

            UILabel* desc = new UILabel(pluginInfo.description, posInPanel);
            desc->fontsize = 16;
            desc->color = { 255,255,255,0xa0 };
            pluginsPanel->subWidgets.addDrawable(desc);
            posInPanel.y += 22;

            UILabel* filename = new UILabel(frmt("{}, sdk {}", pluginInfo.fileName, pluginInfo.sdkVersion), posInPanel);
            filename->fontsize = 12;
            filename->color = { 255,255,255,0x80 };
            pluginsPanel->subWidgets.addDrawable(filename);
            posInPanel.y += 18;


            posInPanel.y += 10;
        }
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
    btn->onClickCallback = [this](UIButton*) {
        platformOpenFileLocation(platformEnsureDirAndGetConfigFilePath());
    };
    configTabs->tabs[5].add(btn);
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
        configTabs->tabs[5].add(btn);
        posInTab.y += 35;

        btn = new UIButton();
        btn->text = TL("vsp.config.opt.associatelospec");
        btn->tooltip = TL("vsp.config.opt.associatelospec.desc");
        btn->position = posInTab;
        btn->wxWidth = 270;
        btn->onClickCallback = [this](UIButton*) {
            if (platformRegisterURI("lospec-palette", { "--no-launchpad" })) {
                g_addNotification(SuccessShortNotification(TL("vsp.config.uri.success"), ""));
            }
            else {
                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.config.uri.error")));
                logerr("failed to register lospec-palette:// uri");
            }
        };
        configTabs->tabs[5].add(btn);
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
    configTabs->tabs[5].add(btn);
    posInTab.y += 35;

    btn = new UIButton();
    btn->text = TL("vsp.config.opt.reloadcolorlist");
    btn->position = posInTab;
    btn->wxWidth = 270;
    btn->onClickCallback = [this](UIButton*) {
        g_reloadColorMap();
        g_addNotification(SuccessShortNotification(TL("vsp.config.opt.colorlistreloaded"), ""));
        };
    configTabs->tabs[5].add(btn);
    posInTab.y += 35;

#if VSP_PLATFORM == VSP_PLATFORM_WIN32
    std::filesystem::path dllpath = convertStringOnWin32(g_programDirectory);
    dllpath /= "voidsprite_shellext.dll";

    if (std::filesystem::exists(dllpath)) {
        btn = new UIButton();
        btn->text = TL("vsp.config.opt.installshellex");
        btn->tooltip = TL("vsp.config.opt.installshellex.desc");
        btn->position = posInTab;
        btn->wxWidth = 270;
        btn->onClickCallback = [this, dllpath](UIButton*) {  shellExtensionAction(true); };
        configTabs->tabs[5].add(btn);
        posInTab.y += 35;

        btn = new UIButton();
        btn->text = TL("vsp.config.opt.uninstallshellex");
        btn->position = posInTab;
        btn->wxWidth = 270;
        btn->onClickCallback = [this, dllpath](UIButton*) {  shellExtensionAction(false); };
        configTabs->tabs[5].add(btn);
        posInTab.y += 35;
    }
#endif


#if __ANDROID__
    btn = new UIButton();
    btn->text = TL("vsp.config.opt.setfileaccess");
    btn->position = posInTab;
    btn->wxWidth = 270;
    btn->onClickCallback = [this](UIButton*) {
        platformRequestFileAccessPermissions();
    };
    configTabs->tabs[5].add(btn);
    posInTab.y += 35;
#endif

    /*
        -------------------------
        DEBUG TAB
        -------------------------
    */
#if _DEBUG
    posInTab = { 0,10 };
    configTabs->tabs[6].add(UIStackPanel::Vertical(5, {
        optionCheckbox("Show scroll panel bounds", "", &g_debugConfig.debugShowScrollPanelBounds, &posInTab),
        optionCheckbox("Debug color slider gradient bounds", "", &g_debugConfig.debugColorSliderGradients, &posInTab),
        optionCheckbox("Test localization",
            UTF8_DIAMOND " will appear next to strings that are present in the current localization\n"
            UTF8_EMPTY_DIAMOND " will appear next to strings that are being pulled as fallback from English",
            &g_debugConfig.debugTestLocalization, &posInTab),
        optionCheckbox("Show raw RPG2K tile data", "", &g_debugConfig.debugShowTilesRPG2K, &posInTab),
        optionCheckbox("Debug color wheel geometry", "", &g_debugConfig.debugColorWheel, &posInTab),
        optionCheckbox("Show blur buffer", "", &g_debugConfig.debugBlurBehind, &posInTab),
        optionCheckbox("Always show mobile text field", "", &g_debugConfig.debugAlwaysMobileUITextField, &posInTab),
        optionCheckbox("Debug focused drawables", "", &g_debugConfig.debugDrawableFocus, &posInTab),
        optionCheckbox("Debug text field bytes", "", &g_debugConfig.debugShowTextFieldUTF8, &posInTab),
    }, {0, 10}));

    UIButton* debugWipeThumbnails = new UIButton("Clear thumbnail cache");
    debugWipeThumbnails->onClickCallback = [](UIButton*) { thumbnails_clearCache(); };
    configTabs->tabs[6].add(UIStackPanel::Vertical(5, {
        debugWipeThumbnails
    }, { 400, 10 }));

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
            frmt("{}\n   {}\n\n    {} -> {}{}...\n\n{}", 
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
                    currentBindTarget->unassignGamepad();
                    updateKeybindButtonText(currentBindTarget, currentBindTargetButton);
                }
                else if (std::find(reservedKeysNow.begin(), reservedKeysNow.end(), key) != reservedKeysNow.end()
                    || std::find(g_keybindManager.globalReservedKeys.begin(), g_keybindManager.globalReservedKeys.end(), key) != g_keybindManager.globalReservedKeys.end()) {
                    g_addNotification(ErrorNotification(TL("vsp.cmn.error"), frmt("{} is a reserved key.", std::string(SDL_GetScancodeName(key)))));
                    bindingKey = false;
                    playPopupCloseVFX();
                }
                else {
                    bindingKey = false;
                    playPopupCloseVFX();

                    bool ctrl = g_ctrlModifier;
                    bool shift = g_shiftModifier;

                    std::vector<KeyCombo*> unassignTargets;

                    if (currentBindTargetRegion->regionKey != "global") {
                        unassignTargets = joinVectors({ 
                            unassignTargets,
                            currentBindTargetRegion->findAllKeysWith(key,ctrl,shift),
                            g_keybindManager.regions["global"].findAllKeysWith(key, ctrl, shift)
                        });
                    }
                    else {
                        for (auto& [regionName, keyRegion] : g_keybindManager.regions) {
                            unassignTargets = joinVectors({ 
                                unassignTargets,
                                keyRegion.findAllKeysWith(key, ctrl, shift)
                            });
                        }
                    }

                    bool updateAll = !unassignTargets.empty();
                    for (auto& unassigned : unassignTargets) {
                        g_addNotification(WarningNotification(
                            "Unassigned:",
                            unassigned->displayName
                        ));
                        unassigned->unassign();
                    }

                    currentBindTarget->key = key;
                    currentBindTarget->ctrl = ctrl;
                    currentBindTarget->shift = shift;
                    if (updateAll) {
                        createKeybindButtons(keybindSearchField->getText());
                    }
                    else {
                        updateKeybindButtonText(currentBindTarget, currentBindTargetButton);
                    }
                }
            }
        }
        else if (evt.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN) {
            SDL_GamepadButton btn = (SDL_GamepadButton)evt.gbutton.button;
            bindingKey = false;
            playPopupCloseVFX();

            std::vector<KeyCombo*> unassignTargets;

            if (currentBindTargetRegion->regionKey != "global") {
                unassignTargets = joinVectors({ 
                    unassignTargets,
                    currentBindTargetRegion->findAllKeysWith(btn),
                    g_keybindManager.regions["global"].findAllKeysWith(btn)
                });
            }
            else {
                for (auto& [regionName, keyRegion] : g_keybindManager.regions) {
                    unassignTargets = joinVectors({
                        unassignTargets,
                        keyRegion.findAllKeysWith(btn)
                    });
                }
            }
            bool updateAll = !unassignTargets.empty();
            for (auto& unassigned : unassignTargets) {
                g_addNotification(WarningNotification(
                    "Unassigned:",
                    unassigned->displayName
                ));
                unassigned->unassignGamepad();
            }

            currentBindTarget->gamepadButton = btn;
            if (updateAll) {
                createKeybindButtons(keybindSearchField->getText());
            }
            else {
                updateKeybindButtonText(currentBindTarget, currentBindTargetButton);
            }
        }
    }
    else {
        BasePopup::takeInput(evt);
    }
}

void PopupGlobalConfig::eventDropdownItemSelected(int evt_id, int index, std::string name)
{
    if (evt_id == CHECKBOX_ANIMATED_BACKGROUND) {
        g_config.animatedBackground = index;
    }
    else if (evt_id == DROPDOWN_LANGUAGE) {
        g_config.language = langLocNames[index];
        updateLanguageCredit();
    }
}

void PopupGlobalConfig::createKeybindButtons(std::string filterName)
{
    keybindsStack->subWidgets.freeAllDrawables();

    for (auto& [regionName, keyRegion] : g_keybindManager.regions) {
        UILabel* regionLabel = new UILabel(keyRegion.displayName);
        regionLabel->fontsize = 20;
        keybindsStack->subWidgets.addDrawable(UIStackPanel::Horizontal(0, { Panel::Space(8,0), regionLabel }));
        keybindsStack->subWidgets.addDrawable(Panel::Space(0,4));
        auto reservedKeys = keyRegion.reservedKeys;

        for (std::string& keyIDInOrder : keyRegion.orderInSettings) {
            if (filterName == "" || stringContainsIgnoreCase(keyRegion.keybinds[keyIDInOrder].displayName, filterName)) {
                KeybindButton* btn = new KeybindButton();
                btn->wxWidth = keybindsPanel->wxWidth - 20;
                btn->wxHeight = 30;
                btn->icon = keyRegion.keybinds[keyIDInOrder].icon;
                updateKeybindButtonText(&keyRegion.keybinds[keyIDInOrder], btn);

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
                keybindsStack->subWidgets.addDrawable(btn);
            }
        }
        keybindsStack->addWidget(Panel::Space(5, 18));
    }
    keybindsStack->recalculateLayout();
}

void PopupGlobalConfig::updateKeybindButtonText(KeyCombo* keycombo, KeybindButton* btn)
{
    btn->text = keycombo->displayName;
    btn->keybindText = keycombo->getKeyComboName();
    btn->keybindTextColor = !keycombo->isUnassigned() ? SDL_Color{ 0x97,0xf1,0xff,0xf0 }
                                                      : SDL_Color{ 255,255,255, 0xa0 };

    btn->secondKeybindText = keycombo->getKeyComboNameForGamepad();
    btn->secondKeybindTextColor = !keycombo->isGamepadUnassigned() ? SDL_Color{ 0x97,0xf1,0xff,0xf0 }
                                                                   : SDL_Color{ 255,255,255, 0xa0 };
}

void PopupGlobalConfig::updateLanguageCredit()
{
    if (getLocalizations().contains(g_config.language)) {
        std::string langCredit = getLocalizations()[g_config.language].langCredit;
        if (g_config.language != "en-us") {
            langCredit += frmt("\n[completion: {:.2f}%]", g_getLocCompletionPercentage(g_config.language) * 100);
        }
        languageCredit->setText(langCredit);
    }
    else {
        languageCredit->setText("");
    }
}

void PopupGlobalConfig::shellExtensionAction(bool install)
{
#if VSP_PLATFORM == VSP_PLATFORM_WIN32
    std::filesystem::path dllpath = convertStringOnWin32(g_programDirectory);
    dllpath /= "voidsprite_shellext.dll";

    std::wstring paramString = dllpath.wstring();
    if (!install) {
        paramString = L"/u " + paramString;
    }

    SHELLEXECUTEINFOW shExInfo = { 0 };
    shExInfo.cbSize = sizeof(shExInfo);
    shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    shExInfo.hwnd = 0;
    shExInfo.lpVerb = L"runas";
    shExInfo.lpFile = L"regsvr32";
    shExInfo.lpParameters = paramString.c_str();
    shExInfo.lpDirectory = NULL;
    shExInfo.nShow = SW_SHOW;
    shExInfo.hInstApp = 0;

    if (ShellExecuteExW(&shExInfo))
    {
        WaitForSingleObject(shExInfo.hProcess, INFINITE);
        DWORD exitCode = 0;
        GetExitCodeProcess(shExInfo.hProcess, &exitCode);
        CloseHandle(shExInfo.hProcess);

        if (exitCode == 0) {
            g_addNotification(SuccessNotification("Shell extension registered.", ""));
        }
        else {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), frmt("Failed to register shell extension ({})", exitCode)));
        }
    }
#endif
}

UICheckbox* PopupGlobalConfig::optionCheckbox(std::string name, std::string tooltip, bool* target, XY* position)
{
    UICheckbox* cb7 = new UICheckbox(name, target);
    if (tooltip != "") {
        cb7->checkbox->tooltip = tooltip;
    }
    if (position != NULL) {
        cb7->position = *position;
        position->y += 35;
    }
    return cb7;
}

Panel* PopupGlobalConfig::optionNumberInput(std::string name, std::string tooltip, int* target, int min, int max, std::function<void(int)> onModified)
{

    int valueNow = *target;

    UINumberInputField* tf = new UINumberInputField(target);
    tf->wxWidth = 40;
    tf->wxHeight = 25;
    tf->fontsize = 16;
    tf->validateFunction = [min, max](int v) { return v >= min && (max == -1 || v <= max); };

    UISlider* sld = NULL;

    if (max != -1 && max != min) {
        int range = max - min;
        sld = new UISlider();
        sld->wxWidth = 200;
        sld->wxHeight = 25;
        sld->setValue(min, max, valueNow);
        sld->onChangeValueCallback = [target, tf, range, min, onModified](UISlider* sld, float val) {
            int vv = min + range * val;
            *target = vv;
            if (onModified != NULL) {
                onModified(vv);
            }
        };
    }

    tf->valueUpdatedCallback = [target, min, max, sld](int v) {
        if (sld != NULL) {
            sld->setValue(min, max, v);
        }
    };

    Panel* p = UIStackPanel::Horizontal(10, {
        new UILabel(name), Panel::Space(20, 1), tf, sld
    }, {0,0});

    return p;
}

Panel* PopupGlobalConfig::optionColorU32Input(std::string name, std::string tooltip, u32* target, bool allowAlpha)
{
    UIColorInputField* colorInput = new UIColorInputField(allowAlpha);
    colorInput->button->tooltip = tooltip;
    colorInput->button->wxWidth = 80;
    colorInput->setColor(*target);
    colorInput->onColorChangedCallback = [target](UIColorInputField* uicf, u32 newColor) {
        *target = newColor;
    };

    return UIStackPanel::Horizontal(8, { new UILabel(name), colorInput });
}
