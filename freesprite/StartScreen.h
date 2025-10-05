#pragma once
#include "globals.h"
#include "EventCallbackListener.h"
#include "BaseScreen.h"
#include "UITextField.h"
#include "ScreenWideNavBar.h"
#include "Timer64.h"
#include "Panel.h"

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

    StartScreen();

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
    void promptOpenFromURL();
    void promptConnectToNetworkCanvas(std::string ip = "", std::string port = "");
    void tryLoadURL(std::string url);
    void tryLoadFile(std::string path);
    void tryOpenImageFromClipboard();
    void updateCheckFinished();
    void genBGStars();
    XY bgSpaceTransform(XY p);
};

