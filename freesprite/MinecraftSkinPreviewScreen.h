#pragma once
#include "globals.h"
#include "BaseScreen.h"
#include "DrawableManager.h"

class MinecraftSkinPreviewScreen :
    public BaseScreen
{
public:
    MainEditor* caller;

    MinecraftSkinPreviewScreen(MainEditor* parent) {
        caller = parent;
    }

    void render() override;
    void tick() override;
    void takeInput(SDL_Event evt) override;
    BaseScreen* isSubscreenOf() override;

    std::string getName() override { return "Preview MC skin"; }

    void renderBox(XY positionOnScreen, XY wholeSkinDimensions, SDL_Rect textureRegion, SDL_Rect textureSpaceRegion, XY targetRegion, int texW, int texH, bool horizontalFlip = false);
};

