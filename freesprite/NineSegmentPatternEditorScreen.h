#pragma once
#include "BaseScreen.h"
#include "EventCallbackListener.h"

class NineSegmentPatternEditorScreen :
    public BaseScreen, public EventCallbackListener
{
protected:
    MainEditor* caller;

    XY canvasDrawOrigin = { 10,10 };
    int canvasZoom = 1;
    bool scrollingCanvas = false;

    XY pointUL;
    XY pointUR;

    ScreenWideNavBar<NineSegmentPatternEditorScreen*>* navbar = NULL;

    bool closeNextTick = false;

public:
    NineSegmentPatternEditorScreen(MainEditor* parent);

    std::string getName() override { return "9S Pattern Editor"; }
    void render() override;
    void tick() override;
    void takeInput(SDL_Event evt) override;
    BaseScreen* isSubscreenOf() override;

    void eventFileSaved(int evt_id, PlatformNativePathString name, int exporterIndex) override;

    void drawBackground();
};
