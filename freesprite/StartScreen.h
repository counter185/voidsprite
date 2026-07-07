#pragma once
#include "globals.h"
#include "EventCallbackListener.h"
#include "BaseScreen.h"
#include "Timer64.h"
#include "Panel.h"
#include "UIButton.h"

struct LaunchpadBGStar {
    XY pos;
    int size;
    u8 opacity;
    int blinkOffset;
    Timer64 timer{};
};

class ButtonLaunchpadLastFile : public UIButton {
private:
    bool fileExists = false;
    std::string fullPath;
    std::string path;
    std::string fileName;
    std::string size = "";
public:
    ButtonLaunchpadLastFile(std::string file);

    void renderText(XY pos) override;
    void renderTooltip(XY pos) override;
};

class PanelNewImage : public Panel {
protected:
    TabbedView* newImageTabs;

    UINumberInputField* tab0TextFieldW;
    UINumberInputField* tab0TextFieldH;

    UINumberInputField* tab1TextFieldCW;
    UINumberInputField* tab1TextFieldCH;
    UINumberInputField* tab1TextFieldCWX;
    UINumberInputField* tab1TextFieldCHX;

    UIButton* rgbTabCreateButtons[2] = { NULL,NULL };
    ScrollingPanel* templatesPanel = NULL;

    XY newImgSizeTab0 = { 256,256 };
    XY newImgCellSizeTab1 = { 16,16 };
    XY newImgCellCountTab1 = { 4,4 };
public:
    std::function<void()> imageCreatedCallback = NULL;

    PanelNewImage();

    void populateTemplatesPanel();
    void runImageCreatedCallback();

    void loadFromTemplate(BaseTemplate* templ);
    void newRGBSession(u32 fill = 0x00000000);
    void newIndexedSession(std::string palette);

};

class StartScreen : public BaseScreen, public EventCallbackListener
{
private:
    Timer64 fileDropTimer;
    bool droppingFile = false;
    XY fileDropXY = { -1,-1 };

    PanelNewImage* newImagePanel = NULL;

    bool waitingForUpdateCheckInfo = true;
    std::vector<LaunchpadBGStar> stars;
    Timer64 updateCheckTimer;
public:

    UIStackPanel* lastOpenFilesPanel;

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
    bool takesTouchEvents() { return true; }

    void eventFileSaved(int evt_id, PlatformNativePathString name, int importerIndex = -1) override;
    void eventFileOpen(int evt_id, PlatformNativePathString name, int importerIndex = -1) override;
    
    void tryLoadURL(std::string url);
    void tryLoadFile(std::string path);
    void tryLoadFileUsingImporter(FileImporter* importer, PlatformNativePathString name);
    void tryOpenImageFromClipboard();

    void promptOpenFromURL();
    void openImageLoadDialog();
    void openImagePreprocessDialog();
    void checkAndPromptCrashSaves();

    void populateLastOpenFiles();

    void renderStartupAnim();
    void renderFileDropAnim();
    void renderBackground();
    void renderBGStars();
    void renderUpdateCheck(SDL_Rect logoRect);
    static void promptConnectToNetworkCanvas(std::string ip = "", std::string port = "");
    void updateCheckFinished();
    void genBGStars();
    XY bgSpaceTransform(XY p);
};

