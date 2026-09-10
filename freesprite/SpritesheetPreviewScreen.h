#pragma once
#include "globals.h"
#include "BaseScreen.h"
#include "EventCallbackListener.h"
#include "DrawableManager.h"
#include "ScreenWideNavBar.h"
#include "Canvas.h"

class SpritesheetPreviewScreen : public BaseScreen, public EventCallbackListener
{
public:
    MainEditor* caller;
    EditorSpritesheetPreview* previewWx;

    PanelSpritesheetPreview* panel;

    std::vector<XY> sprites;
    int spritesProgress = 0;
    int msPerSprite = 128;

    Canvas canvas;
    bool scrollingCanvas = false;
    XY lastTapPosition = { 0,0 };
    Timer64 lastTapTimer;

    bool closeNextTick = false;

    ScrollingPanel* spriteView;
    int timelineSpriteScale = 1;

    SpritesheetPreviewScreen(MainEditor* parent);
    ~SpritesheetPreviewScreen();

    void render() override;
    void RenderCanvas();
    void tick() override;
    void defaultInputAction(SDL_Event evt) override;
    BaseScreen* isSubscreenOf() override;
    bool takesTouchEvents() { return true; }

    void eventButtonPressed(int evt_id) override;

    std::string getName() override { return TL("vsp.spritesheetpreview"); }

    void drawPreview(XY at, int scale, int which = -1);
    void drawBackground();
    void genTimelineButtons();
    void addTimelineButton();
    void popTimelineButton();
    int calcMaxTimelineScale();

    void selectTileAt(XY pos);
};

