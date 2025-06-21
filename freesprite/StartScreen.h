#pragma once
#include "globals.h"
#include "EventCallbackListener.h"
#include "drawable.h"
#include "DrawableManager.h"
#include "BaseScreen.h"
#include "UITextField.h"
#include "UIButton.h"
#include "maineditor.h"
#include "MainEditorPalettized.h"
#include "UILabel.h"
#include "TabbedView.h"
#include "BaseTemplate.h"
#include "PopupMessageBox.h"
#include "UIColorInputField.h"
#include "Notification.h"
#include "ScreenWideNavBar.h"
#include "PopupQuickConvert.h"
#include "PopupGlobalConfig.h"
#include "SplitSessionEditor.h"
#include "PopupListRecoveryAutosaves.h"
#include "Timer64.h"
#include "Panel.h"
#include "PopupAbout.h"
#include "PopupYesNo.h"

#include "background_operation.h"
#include "updatecheck.h"
#include "main.h"

struct LaunchpadBGStar {
    XY pos;
    int size;
    u8 opacity;
    int blinkOffset;
    Timer64 timer;
};

class StartScreen : public BaseScreen, public EventCallbackListener
{
private:
    Timer64 fileDropTimer;
    bool droppingFile = false;
    XY fileDropXY = { -1,-1 };

    bool waitingForUpdateCheckInfo = true;
    std::vector<LaunchpadBGStar> stars;
    Timer64 updateCheckTimer;
public:
    TabbedView* newImageTabs;

    int newImgW = 0, newImgH = 0;

    UITextField* tab0TextFieldW;
    UITextField* tab0TextFieldH;

    UITextField* tab1TextFieldCW;
    UITextField* tab1TextFieldCH;
    UITextField* tab1TextFieldCWX;
    UITextField* tab1TextFieldCHX;

    ScreenWideNavBar* navbar;

    Panel* lastOpenFilesPanel;

    Timer64 startupAnimTimer;

    bool closeNextTick = false;

    StartScreen() {
        if (g_config.checkUpdates) {
            g_startNewAsyncOperation([]() { runUpdateCheck(); });
        }

        UILabel* title = new UILabel("voidsprite");
        title->position = {10, 40};
        wxsManager.addDrawable(title);
        
        title = new UILabel(TL("vsp.launchpad.newimage"));
        title->fontsize = 22;
        title->position = {10, 75};
        wxsManager.addDrawable(title);

        newImageTabs = new TabbedView({ {TL("vsp.launchpad.tab.pixeldim"), g_iconMenuPxDim}, {TL("vsp.launchpad.tab.spritesheet"), g_iconMenuSpritesheet}, {TL("vsp.launchpad.tab.templates"), g_iconMenuTemplates}}, 180);
        newImageTabs->position = XY{ 10, 110 };
        wxsManager.addDrawable(newImageTabs);

        //tab 0
        tab0TextFieldW = new UITextField();
        tab0TextFieldW->setCallbackListener(2, this);
        tab0TextFieldW->position = xySubtract(XY{80,120}, newImageTabs->position);
        tab0TextFieldW->wxWidth = 200;
        tab0TextFieldW->isNumericField = true;
        newImageTabs->tabs[0].wxs.addDrawable(tab0TextFieldW);

        tab0TextFieldH = new UITextField();
        tab0TextFieldH->setCallbackListener(3, this);
        tab0TextFieldH->position = xySubtract(XY{ 80,155 }, newImageTabs->position);
        tab0TextFieldH->wxWidth = 200;
        tab0TextFieldH->isNumericField = true;
        newImageTabs->tabs[0].wxs.addDrawable(tab0TextFieldH);

        UILabel* wLabel = new UILabel(TL("vsp.cmn.width"));
        wLabel->position = xySubtract(XY{ 10,120 }, newImageTabs->position);
        newImageTabs->tabs[0].wxs.addDrawable(wLabel);

        UILabel* hLabel = new UILabel(TL("vsp.cmn.height"));
        hLabel->position = xySubtract(XY{ 10,155 }, newImageTabs->position);
        newImageTabs->tabs[0].wxs.addDrawable(hLabel);

        //tab 1
        tab1TextFieldCW = new UITextField();
        tab1TextFieldCW->position = xySubtract(XY{ 110,120}, newImageTabs->position);
        tab1TextFieldCW->wxWidth = 160;
        tab1TextFieldCW->isNumericField = true;
        newImageTabs->tabs[1].wxs.addDrawable(tab1TextFieldCW);

        tab1TextFieldCWX = new UITextField();
        tab1TextFieldCWX->position = xySubtract(XY{ 300,120 }, newImageTabs->position);
        tab1TextFieldCWX->wxWidth = 40;
        tab1TextFieldCWX->isNumericField = true;
        tab1TextFieldCWX->setText("1");
        newImageTabs->tabs[1].wxs.addDrawable(tab1TextFieldCWX);

        tab1TextFieldCH = new UITextField();
        tab1TextFieldCH->position = xySubtract(XY{ 110,155 }, newImageTabs->position);
        tab1TextFieldCH->wxWidth = 160;
        tab1TextFieldCH->isNumericField = true;
        tab1TextFieldCH->setCallbackListener(3, this);
        newImageTabs->tabs[1].wxs.addDrawable(tab1TextFieldCH);

        tab1TextFieldCHX = new UITextField();
        tab1TextFieldCHX->position = xySubtract(XY{ 300,155 }, newImageTabs->position);
        tab1TextFieldCHX->wxWidth = 40;
        tab1TextFieldCHX->isNumericField = true;
        tab1TextFieldCHX->setText("1");
        tab1TextFieldCHX->setCallbackListener(3, this);
        newImageTabs->tabs[1].wxs.addDrawable(tab1TextFieldCHX);

        UILabel* w2Label = new UILabel(TL("vsp.launchpad.tab.cellw"));
        w2Label->position = xySubtract(XY{ 10,120 }, newImageTabs->position);
        newImageTabs->tabs[1].wxs.addDrawable(w2Label);

        UILabel* h2Label = new UILabel(TL("vsp.launchpad.tab.cellh"));
        h2Label->position = xySubtract(XY{ 10,155 }, newImageTabs->position);
        newImageTabs->tabs[1].wxs.addDrawable(h2Label);

        UILabel* w2Label2 = new UILabel("x");
        w2Label2->position = xySubtract(XY{ 280,120 }, newImageTabs->position);
        newImageTabs->tabs[1].wxs.addDrawable(w2Label2);

        UILabel* h2Label2 = new UILabel("x");
        h2Label2->position = xySubtract(XY{ 280,155 }, newImageTabs->position);
        newImageTabs->tabs[1].wxs.addDrawable(h2Label2);

        std::vector<std::pair<std::string, std::string>> templates;
        for (BaseTemplate*& templ : g_templates) {
            templates.push_back({ templ->getName(), templ->getTooltip() });
        }
        UIDropdown* templatesDropdown = new UIDropdown(templates);
        templatesDropdown->position = XY{ 40, 5 };
        templatesDropdown->text = TL("vsp.launchpad.tab.picktemplate");
        templatesDropdown->setCallbackListener(EVENT_STARTSCREEN_TEMPLATEPICKED, this);
        templatesDropdown->wxWidth = 400;
        templatesDropdown->genButtons();
        newImageTabs->tabs[2].wxs.addDrawable(templatesDropdown);

        for (int x = 0; x < 2; x++) {
            UIButton* buttonNewImageRGB = new UIButton();
            buttonNewImageRGB->setCallbackListener(4, this);
            buttonNewImageRGB->position = XY{ 30,90 };
            buttonNewImageRGB->wxWidth = 160;
            buttonNewImageRGB->text = TL("vsp.launchpad.tab.creatergb");
            buttonNewImageRGB->tooltip = TL("vsp.launchpad.tab.creatergb.tooltip");
            newImageTabs->tabs[x].wxs.addDrawable(buttonNewImageRGB);

            UIButton* buttonNewImagePalettized = new UIButton();
            buttonNewImagePalettized->setCallbackListener(5, this);
            buttonNewImagePalettized->position = XY{ 200,90 };
            buttonNewImagePalettized->wxWidth = 200;
            buttonNewImagePalettized->text = TL("vsp.launchpad.tab.createindexed");
            buttonNewImagePalettized->tooltip = TL("vsp.launchpad.tab.createindexed.tooltip");
            newImageTabs->tabs[x].wxs.addDrawable(buttonNewImagePalettized);
        }


        int startScreenPanelEndpoint = newImageTabs->getDimensions().x + newImageTabs->position.x;
        startScreenPanelEndpoint = ixmax(560, startScreenPanelEndpoint);

        lastOpenFilesPanel = new Panel();
        lastOpenFilesPanel->wxWidth = startScreenPanelEndpoint;
        lastOpenFilesPanel->wxHeight = 520;
        lastOpenFilesPanel->position = { startScreenPanelEndpoint + 10, 75};
        lastOpenFilesPanel->passThroughMouse = true;
        wxsManager.addDrawable(lastOpenFilesPanel);

        navbar = new ScreenWideNavBar(this, 
        {
            {
                SDL_SCANCODE_F,
                {
                    TL("vsp.nav.file"),
                    {SDL_SCANCODE_O, SDL_SCANCODE_V, SDL_SCANCODE_E, SDL_SCANCODE_S, SDL_SCANCODE_R, SDL_SCANCODE_P},
                    {
                        {SDL_SCANCODE_O, { TL("vsp.nav.open"),
                                [this]() {
                                    this->openImageLoadDialog();
                                }
                            }
                        },
                        {SDL_SCANCODE_V, { TL("vsp.launchpad.nav.openclipboard"),
                                [this]() {
                                    this->tryOpenImageFromClipboard();
                                }
                            }
                        },
                        {SDL_SCANCODE_E, { TL("vsp.launchpad.nav.quickconvert"),
                                [this]() {
                                    g_addPopup(new PopupQuickConvert(TL("vsp.launchpad.nav.quickconvert"), TL("vsp.launchpad.quickconvert.desc")));
                                }
                            }
                        },
                        {SDL_SCANCODE_S, { TL("vsp.launchpad.nav.newsplitsession"),
                                [this]() {
                                    platformTrySaveOtherFile(this, {{".voidspsn", TL("vsp.cmn.filetype.splitsession")}}, TL("vsp.popup.newsplitsession"), 0);
                                }
                            }
                        },
                        {SDL_SCANCODE_P, { TL("vsp.launchpad.nav.preferences"),
                                [this]() {
                                    g_addPopup(new PopupGlobalConfig());
                                }
                            }
                        },
                        {SDL_SCANCODE_R, { TL("vsp.launchpad.nav.recoveryautosaves"),
                                [this]() {
                                    g_addPopup(new PopupListRecoveryAutosaves());
                                }
                            }
                        }
                    },
                    g_iconNavbarTabFile
                }
            },
            {
                SDL_SCANCODE_W,
                {
                    TL("vsp.nav.window"),
                    {},
                    {
                        {SDL_SCANCODE_N, {TL("vsp.launchpad.nav.newwindow"),
                                [this]() { main_newWindow(""); }
                            }
                        },
                    }
                }
            },
            {
                SDL_SCANCODE_I,
                {
                    TL("vsp.nav.help"),
                    {},
                    {
                        {SDL_SCANCODE_U, { TL("vsp.launchpad.nav.opennightlylinkdl"),
                                [this]() { platformOpenWebpageURL("https://nightly.link/counter185/voidsprite/workflows/msbuild/main"); }
                            }
                        },
                        {SDL_SCANCODE_A, { TL("vsp.launchpad.nav.about"),
                                [this]() { g_addPopup(new PopupAbout()); }
                            }
                        },
                    }
                }
            }
        }, { SDL_SCANCODE_F, SDL_SCANCODE_W, SDL_SCANCODE_I });
        wxsManager.addDrawable(navbar);

        if (!platformHasFileAccessPermissions()) {
            PopupYesNo* permissionPopup = new PopupYesNo(TL("vsp.launchpad.filepermcheck.title"), TL("vsp.launchpad.filepermcheck.desc"));
            permissionPopup->onFinishCallback = [](PopupYesNo*, bool yes) {
                if (yes) {
                    platformRequestFileAccessPermissions();
                }
            };

            g_addPopup(permissionPopup);
        }

        populateLastOpenFiles();
        startupAnimTimer.start();
    }

    void render() override;
    void tick() override;
    void takeInput(SDL_Event evt) override;

    void onReturnToScreen() override {
        droppingFile = false;
        populateLastOpenFiles();
    }

    std::string getName() override { return TL("vsp.launchpad"); }

    void eventTextInputConfirm(int evt_id, std::string data) override {
        if (evt_id == 3) {
            eventButtonPressed(4);
        }
    }

    void eventButtonPressed(int evt_id) override;
    void eventFileSaved(int evt_id, PlatformNativePathString name, int importerIndex = -1) override;
    void eventFileOpen(int evt_id, PlatformNativePathString name, int importerIndex = -1) override;
    void eventDropdownItemSelected(int evt_id, int index, std::string name) override;
    
    void populateLastOpenFiles();
    void renderStartupAnim();
    void renderFileDropAnim();
    void renderBackground();
    void renderBGStars();
    void openImageLoadDialog();
    void tryLoadFile(std::string path);
    void tryOpenImageFromClipboard();
    void updateCheckFinished();
    void genBGStars();
    XY bgSpaceTransform(XY p);
};

