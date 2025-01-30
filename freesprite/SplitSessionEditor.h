#pragma once
#include "BaseScreen.h"
#include "Canvas.h"
#include "splitsession.h"
#include "EventCallbackListener.h"

struct tempSplitSessionImage {
    Layer* loadedLayer = NULL;
    XY dims = { 0,0 };
    XY position = { 0,0 };
    std::string fullOriginalPath;
    std::string calcRelativePath;
    Timer64 namePopupTimer;
};

class SplitSessionEditor : public BaseScreen, public EventCallbackListener
{
private:
    bool scrollingCanvas = false;
    XY mousePixelPos = { 0,0 };

    int dragging = -1;
    XY draggingStartPixel = { 0,0 };

    bool closeNextTick = false;

protected:
    Canvas c;
    SplitSessionData ssn;
    UILabel* guideLabel;
    ScrollingPanel* subImagesPanel;
    std::vector<tempSplitSessionImage> loadedImgs;
    std::string outputSPSNFilePath = "";
    ScreenWideNavBar<SplitSessionEditor*>* navbar;
public:
    SplitSessionEditor();
    SplitSessionEditor(PlatformNativePathString path);
    ~SplitSessionEditor();

    void tick() override;
    void render() override;
    void takeInput(SDL_Event evt) override;

    std::string getName() override { return "Split session editor"; }
    void eventFileSaved(int evt_id, PlatformNativePathString name, int exporterIndex) override;
    void eventPopupClosed(int evt_id, BasePopup* popup) override;

    void drawBackground();
    void drawTSSITooltips();

    void setupWidgets();
    int findTSSIAt(XY canvasPosition);
    void recalcCanvasDimensions();
    void recalcRelativePaths();
    void populateSubImagesList();
    XY findNextAvailablePosition();
    void tryAddFile(std::string path);
    void calcMousePixelPos(XY onScreenPos);
    bool trySave(bool openSession = false);
};

