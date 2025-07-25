#pragma once
#include "BaseScreen.h"
#include "Canvas.h"

class ViewSessionScreen :
    public BaseScreen
{
protected:
    MainEditor* caller;
    Canvas c;
    bool scrollingCanvas = false;
    Fill background = Fill::Solid(0xFF000000);

public:
    ViewSessionScreen(MainEditor* parent);

    std::string getName() override { return TL("vsp.fullscreenpreview"); }
    BaseScreen* isSubscreenOf() override;
    void render() override;
    void takeInput(SDL_Event evt) override;
};

