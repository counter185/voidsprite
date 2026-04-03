#pragma once
#include "ScreenIsomView.h"
#include "PanelUserInteractable.h"

class MinecraftSkinPreviewScreen;

class PanelMCSkinPreview : public PanelUserInteractable {
private:
    bool dragging = false;
public:
    MinecraftSkinPreviewScreen* parent;

    PanelMCSkinPreview(MinecraftSkinPreviewScreen* caller);
    void renderAfterBG(XY at) override;
    SDL_Rect getPreviewAreaRect();
    bool defaultInputAction(SDL_Event evt, XY at) override;
};

class PanelMCSkinPreviewSettings : public PanelUserInteractable {
private:
    MinecraftSkinPreviewScreen* parent;
public:
    PanelMCSkinPreviewSettings(MinecraftSkinPreviewScreen* caller);
};

class MinecraftSkinPreviewScreen :
    public ScreenIsomView
{
private:
    MainEditor* caller;
    PanelMCSkinPreview* inEditorPanel = NULL;

protected:
    int dragging = 0;
    bool slimModel = false;
    bool twoByOneSkin = false;
public:

    double overlayOffsetSize = 0.3;

    bool
        renderHead = true,
        renderHeadOverlay = true,
        renderBody = true,
        renderBodyOverlay = true,
        renderLArm = true,
        renderLArmOverlay = true,
        renderRArm = true,
        renderRArmOverlay = true,
        renderLLeg = true,
        renderLLegOverlay = true,
        renderRLeg = true,
        renderRLegOverlay = true;

    MinecraftSkinPreviewScreen(MainEditor* parent);
    ~MinecraftSkinPreviewScreen();

    void render() override;
    void defaultInputAction(SDL_Event evt) override;
    void tick() override;
    void recalcPointScale();
    bool takesTouchEvents() override { return true; }
    BaseScreen* isSubscreenOf() override;

    std::string getName() override { return "Preview MC skin"; }

    void debugRenderAxes();
    void renderModel(XY origin00, double scale);
    SDL_Rect getModelRenderArea(XY);

    void renderToWorkspace();

    static bool dimensionsValidForPreview(XY size) {
        return size.x > 0 && size.y > 0
            && size.x % 8 == 0  //allowing downscale up to 1/4
            && (size.x == size.y || size.x == (2 * size.y));
    }
};

