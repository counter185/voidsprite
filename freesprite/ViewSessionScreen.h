#pragma once
#include "BaseScreen.h"
#include "Canvas.h"

class ViewSessionScreen :
    public BaseScreen
{
protected:
    MainEditor* caller = NULL;
    Canvas c;
    bool scrollingCanvas = false;
    Fill background = Fill::Solid(0xFF000000);
    Timer64 lastInputTimer;

    virtual void setupNavbar();
    void resetInputTimerOnInput(SDL_Event evt);

    ViewSessionScreen() {}
public:
    ViewSessionScreen(MainEditor* parent);

    std::string getName() override { return TL("vsp.fullscreenpreview"); }
    BaseScreen* isSubscreenOf() override;
    void takeInput(SDL_Event evt) {
        this->resetInputTimerOnInput(evt);
        BaseScreen::takeInput(evt);
    }
    void render() override;
    void defaultInputAction(SDL_Event evt) override;;

};

class TransparentViewSessionScreen : public ViewSessionScreen {
protected:
    SingleScreenTransparentWindow* parentTransparentWindow = NULL;

    void setupNavbar() override;
public:
    TransparentViewSessionScreen(MainEditor* parent);

    void render() override;
    void defaultInputAction(SDL_Event evt) override;

    void assignWindow(SingleScreenTransparentWindow* w) { parentTransparentWindow = w; }

    void promptEnableOverlayMode();
    void enableOverlayMode();
};
