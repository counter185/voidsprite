#pragma once
#include "BaseScreen.h"
#include "Canvas.h"
#include "splitsession.h"

class SplitSessionEditor :
    public BaseScreen
{
protected:
    Canvas c;
    SplitSessionData ssn;
public:
    SplitSessionEditor();

    void render() override;
    void takeInput(SDL_Event evt) override;

    void drawBackground();

    void tryAddFile(std::string path);
};

