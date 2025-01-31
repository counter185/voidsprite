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
#include "Timer64.h"
#include "Panel.h"

class StartScreen : public BaseScreen, public EventCallbackListener
{
public:
    TabbedView* newImageTabs;

    int newImgW = 0, newImgH = 0;

    UITextField* tab0TextFieldW;
    UITextField* tab0TextFieldH;

    UITextField* tab1TextFieldCW;
    UITextField* tab1TextFieldCH;
    UITextField* tab1TextFieldCWX;
    UITextField* tab1TextFieldCHX;

    ScreenWideNavBar<StartScreen*>* navbar;

    Panel* lastOpenFilesPanel;

    Timer64 startupAnimTimer;

    bool closeNextTick = false;

    StartScreen() {
        newImageTabs = new TabbedView({ {"Pixel dimensions", g_iconMenuPxDim}, {"Sprites/Tiles", g_iconMenuSpritesheet}, {"Templates", g_iconMenuTemplates} }, 180);
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

        UILabel* wLabel = new UILabel();
        wLabel->text = "Width";
        wLabel->position = xySubtract(XY{ 10,120 }, newImageTabs->position);
        newImageTabs->tabs[0].wxs.addDrawable(wLabel);

        UILabel* hLabel = new UILabel();
        hLabel->text = "Height";
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
        tab1TextFieldCWX->text = "1";
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
        tab1TextFieldCHX->text = "1";
        tab1TextFieldCHX->setCallbackListener(3, this);
        newImageTabs->tabs[1].wxs.addDrawable(tab1TextFieldCHX);

        UILabel* w2Label = new UILabel();
        w2Label->text = "Cell width";
        w2Label->position = xySubtract(XY{ 10,120 }, newImageTabs->position);
        newImageTabs->tabs[1].wxs.addDrawable(w2Label);

        UILabel* h2Label = new UILabel();
        h2Label->text = "Cell height";
        h2Label->position = xySubtract(XY{ 10,155 }, newImageTabs->position);
        newImageTabs->tabs[1].wxs.addDrawable(h2Label);

        UILabel* w2Label2 = new UILabel();
        w2Label2->text = "x";
        w2Label2->position = xySubtract(XY{ 280,120 }, newImageTabs->position);
        newImageTabs->tabs[1].wxs.addDrawable(w2Label2);

        UILabel* h2Label2 = new UILabel();
        h2Label2->text = "x";
        h2Label2->position = xySubtract(XY{ 280,155 }, newImageTabs->position);
        newImageTabs->tabs[1].wxs.addDrawable(h2Label2);

        std::vector<std::pair<std::string, std::string>> templates;
        for (BaseTemplate*& templ : g_templates) {
            templates.push_back({ templ->getName(), templ->getTooltip() });
        }
        UIDropdown* templatesDropdown = new UIDropdown(templates);
        templatesDropdown->position = XY{ 40, 5 };
        templatesDropdown->text = "Select template...";
        templatesDropdown->setCallbackListener(EVENT_STARTSCREEN_TEMPLATEPICKED, this);
        templatesDropdown->wxWidth = 400;
        templatesDropdown->genButtons();
        newImageTabs->tabs[2].wxs.addDrawable(templatesDropdown);

        for (int x = 0; x < 2; x++) {
            UIButton* buttonNewImageRGB = new UIButton();
            buttonNewImageRGB->setCallbackListener(4, this);
            buttonNewImageRGB->position = XY{ 30,90 };
            buttonNewImageRGB->wxWidth = 120;
            buttonNewImageRGB->text = "Create (RGB)";
            buttonNewImageRGB->tooltip = "Create a new image with full color support.";
            newImageTabs->tabs[x].wxs.addDrawable(buttonNewImageRGB);

            UIButton* buttonNewImagePalettized = new UIButton();
            buttonNewImagePalettized->setCallbackListener(5, this);
            buttonNewImagePalettized->position = XY{ 160,90 };
            buttonNewImagePalettized->wxWidth = 200;
            buttonNewImagePalettized->text = "Create (Palettized)";
            buttonNewImagePalettized->tooltip = "Create a new image with a limited color palette.\nExporting to palette-only formats will keep the color order as it is in the editor.";
            newImageTabs->tabs[x].wxs.addDrawable(buttonNewImagePalettized);
        }

        lastOpenFilesPanel = new Panel();
        lastOpenFilesPanel->wxWidth = 560;
        lastOpenFilesPanel->wxHeight = 520;
        lastOpenFilesPanel->position = {570, 75};
        wxsManager.addDrawable(lastOpenFilesPanel);

        navbar = new ScreenWideNavBar<StartScreen*>(this, 
        {
            {
                SDLK_f,
                {
                    "File",
                    {SDLK_o, SDLK_v, SDLK_e, SDLK_s, SDLK_p},
                    {
                        {SDLK_o, { "Open",
                                [](StartScreen* screen) {
                                    screen->openImageLoadDialog();
                                }
                            }
                        },
                        {SDLK_v, { "Open from clipboard",
                                [](StartScreen* screen) {
                                    screen->tryOpenImageFromClipboard();
                                }
                            }
                        },
                        {SDLK_e, { "Quick Convert",
                                [](StartScreen* screen) {
                                    g_addPopup(new PopupQuickConvert("Quick Convert", "Select the format to export the image to.\nDrag a file into this window to convert to the same directory."));
                                }
                            }
                        },
                        {SDLK_s, { "New split session...",
                                [](StartScreen* screen) {
                                    platformTrySaveOtherFile(screen, {{".voidspsn", "Split session file"}}, "create new split session", 0);
                                }
                            }
                        },
                        {SDLK_p, { "Preferences",
                                [](StartScreen* screen) {
                                    g_addPopup(new PopupGlobalConfig());
                                }
                            }
                        }
                    },
                    g_iconNavbarTabFile
                }
            }
        }, { SDLK_f });
        wxsManager.addDrawable(navbar);


        for (std::string& arg : g_cmdlineArgs) {

            if (arg.substr(0,2) == "--") {
                std::string option = arg.substr(2);
                if (option == "no-launchpad") {
                    this->closeNextTick = true;
                }
            }
            else {
                if (std::filesystem::exists(convertStringOnWin32(arg))) {
                    tryLoadFile(arg);
                }
                else {
                    g_addNotification(ErrorNotification("Error", std::format("Could not find file:\n {}", arg)));
                }
            }
        }

        populateLastOpenFiles();
        startupAnimTimer.start();
    }

    void render() override;
    void tick() override;
    void takeInput(SDL_Event evt) override;

    void onReturnToScreen() override {
        populateLastOpenFiles();
    }

    std::string getName() override { return "voidsprite Launchpad"; }

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
    void renderBackground();
    void openImageLoadDialog();
    void tryLoadFile(std::string path);
    void tryOpenImageFromClipboard();
};

