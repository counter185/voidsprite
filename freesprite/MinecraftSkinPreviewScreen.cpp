#include "MinecraftSkinPreviewScreen.h"
#include "maineditor.h"
#include "FontRenderer.h"
#include "ScreenWideNavBar.h"
#include "Notification.h"
#include "background_operation.h"
#include "UIStackPanel.h"
#include "UICheckbox.h"
#include "UISlider.h"
#include "UILabel.h"

BaseScreen* MinecraftSkinPreviewScreen::isSubscreenOf()
{
    return caller;
}

void MinecraftSkinPreviewScreen::debugRenderAxes()
{
    XY origin = screen00;

    XY center = xyAdd(origin, xydToXy(worldSpaceToScreenSpace({ 0,0,0 }, rotAlpha * M_PI / 180, rotBeta * M_PI / 180) * 20));
    XY xpoint = xyAdd(origin, xydToXy(worldSpaceToScreenSpace({ 10,0,0 }, rotAlpha * M_PI / 180, rotBeta * M_PI / 180) * 20));
    XY zpoint = xyAdd(origin, xydToXy(worldSpaceToScreenSpace({ 0,0,10 }, rotAlpha * M_PI / 180, rotBeta * M_PI / 180) * 20));
    XY ypoint = xyAdd(origin, xydToXy(worldSpaceToScreenSpace({ 0,10,0 }, rotAlpha * M_PI / 180, rotBeta * M_PI / 180) * 20));

    SDL_SetRenderDrawColor(g_rd, 255, 0, 0, 255);
    drawLine(center, xpoint);

    SDL_SetRenderDrawColor(g_rd, 0, 255, 0, 255);
    drawLine(center, ypoint);

    SDL_SetRenderDrawColor(g_rd, 0, 0, 255, 255);
    drawLine(center, zpoint);
}

void MinecraftSkinPreviewScreen::renderModel(XY origin00, double scale)
{
    struct BoxRenderListObj {
        XYZd pos;
        double sx, sz, sy;
        XY texturePos;
        double offs = 0.0;
        bool flipX = false;
        bool render = true;
    };

    double handSizeX = slimModel ? 3 : 4;

    XY leftArmTexturePos = xyAdd({ 40,16 }, { 4,0 });
    XY leftLegTexturePos = { 4,16 };

    std::vector<std::vector<std::vector<BoxRenderListObj>>> renderList = {
        //legs layer
        {
            {
                BoxRenderListObj{{ -4,0,0 }, 4, 4, 12, leftLegTexturePos, 0.0, false, renderRLeg},  //left leg
                BoxRenderListObj{{ -4,0,0 }, 4, 4, 12, XY{ 4, 32 }, overlayOffsetSize, false, !twoByOneSkin && renderRLegOverlay},   //left leg overlay
            },   
            {
                BoxRenderListObj{{ 0,0,0 }, 4, 4, 12, twoByOneSkin ? leftLegTexturePos : XY{ 20, 48 }, 0.0, twoByOneSkin, renderLLeg},   //right leg
                BoxRenderListObj{{ 0,0,0 }, 4, 4, 12, XY{ 4, 48 }, overlayOffsetSize, false, !twoByOneSkin && renderLLegOverlay},   //right leg overlay
            }    
        },

        //body layer
        {
            {
                BoxRenderListObj{{ -4.0 - handSizeX,12,0 }, handSizeX, 4, 12, leftArmTexturePos, 0.0, false, renderRArm},     //left arm
                BoxRenderListObj{{ -4.0 - handSizeX,12,0 }, handSizeX, 4, 12, {44,32}, overlayOffsetSize, false, !twoByOneSkin && renderRArmOverlay},     //left arm overlay
            },   
            {
                BoxRenderListObj{{ -4,12,0 }, 8, 4, 12, { 20, 16 }, 0.0, false, renderBody},        //body
                BoxRenderListObj{{ -4,12,0 }, 8, 4, 12, { 20, 32 }, overlayOffsetSize, false, !twoByOneSkin && renderBodyOverlay}    //body overlay
            },
            {
                BoxRenderListObj{{ 4,12,0 }, handSizeX, 4, 12, twoByOneSkin ? leftArmTexturePos : xyAdd({ 32,48 }, { 4,0 }), 0.0, twoByOneSkin, renderLArm}, //right arm
                BoxRenderListObj{{ 4,12,0 }, handSizeX, 4, 12, {52, 48}, overlayOffsetSize, false, !twoByOneSkin && renderLArmOverlay}   //right arm overlay
            }   
        },

        //head layer
        {
            {
                BoxRenderListObj{{ -4,24,-2 }, 8, 8, 8, { 8, 0 }, 0.0, false, renderHead},  //head
                BoxRenderListObj{{ -4,24,-2 }, 8, 8, 8, { 40, 0 }, overlayOffsetSize, false, renderHeadOverlay} //head overlay
            }
        }
    };

    if (rotAlpha < 0) {
        std::reverse(renderList.begin(), renderList.end());
    }

    for (auto& x : renderList) {
        auto nextLayer = x;
        if (rotBeta >= 180) {
            std::reverse(nextLayer.begin(), nextLayer.end());
        }
        for (auto& z : nextLayer) {
            for (auto& y : z) {
                if (y.render) {
                    renderBoxFromEditorContent(caller, origin00, scale, y.pos, y.sx, y.sz, y.sy, y.texturePos, y.offs, y.flipX);
                }
            }
        }
    }
}

SDL_Rect MinecraftSkinPreviewScreen::getModelRenderArea(XY at)
{
    //todo:fix this
    XY topLeft;
    XY bottomRight;

    bottomRight.y = at.y + size * (sqrt(3) / 2);
    topLeft.y = at.y - size * 32 * (sqrt(3) / 2);
    topLeft.x = at.x - size * 8 * (sqrt(2) / 2);
    bottomRight.x = at.x + size * 8 * (sqrt(2) / 2);

    return {
        topLeft.x, topLeft.y,
        bottomRight.x - topLeft.x,
        bottomRight.y - topLeft.y
    };
}

void MinecraftSkinPreviewScreen::renderToWorkspace()
{
    SDL_Texture* renderTarget = tracked_createTexture(g_rd, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, g_windowW, g_windowH);
    g_pushRenderTarget(renderTarget);

    SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0);
    SDL_RenderClear(g_rd);
    renderModel(screen00, size);
    Layer* l = new Layer(g_windowW, g_windowH);
    SDL_Surface* nsrf = SDL_RenderReadPixels(g_rd, NULL);
    SDL_ConvertPixels(g_windowW, g_windowH, nsrf->format, nsrf->pixels, nsrf->pitch, SDL_PIXELFORMAT_ARGB8888, l->pixels32(), g_windowW * 4);
    SDL_FreeSurface(nsrf);

    g_popRenderTarget();
    tracked_destroyTexture(renderTarget);

    MainEditor* newSession = new MainEditor(l);
    g_addScreen(newSession);
}

MinecraftSkinPreviewScreen::MinecraftSkinPreviewScreen(MainEditor* parent) {
    caller = parent;
    recalcPointScale();
    screen00 = { g_windowW / 2, g_windowH / 6*5 };
    twoByOneSkin = parent->canvas.dimensions.x > parent->canvas.dimensions.y;
    slimModel = (parent->getLayerStack().front()->getVisualPixelAt(scaledPoint({ 51, 16 })) & 0xFF000000) == 0;

    PanelMCSkinPreviewSettings* settingsPanel = new PanelMCSkinPreviewSettings(this);
    settingsPanel->position = { 10, 50 };
    wxsManager.addDrawable(settingsPanel);

    navbar = new ScreenWideNavBar(this,
        {
            {
                SDL_SCANCODE_F,
                {
                    TL("vsp.nav.file"),
                    { SDL_SCANCODE_R, SDL_SCANCODE_C },
                    {
                        { SDL_SCANCODE_C,{ "Close",
                                [this]() {
                                    g_startNewMainThreadOperation([this]() {g_closeScreen(this); });
                                }
                            } 
                        },
                        {SDL_SCANCODE_R, { "Render to separate workspace",
                                [this]() {
                                    renderToWorkspace();
                                }
                            }
                        },
                    }, g_iconNavbarTabFile
                }
            },
            {
                SDL_SCANCODE_V,
                {
                    TL("vsp.nav.view"),
                    { SDL_SCANCODE_T, SDL_SCANCODE_S, SDL_SCANCODE_M, SDL_SCANCODE_W },
                    {
                        { SDL_SCANCODE_T, { "Toggle 2:1 texture mode",
                                [this]() {
                                    this->twoByOneSkin = !this->twoByOneSkin;
                                    g_addNotification(Notification(
                                        (this->twoByOneSkin ? "2:1 texture mode" : "1:1 texture mode"),
                                        "",
                                        1200
                                    ));
                                }
                            }
                        },
                        { SDL_SCANCODE_S, { "Toggle shading",
                                [this]() {
                                    this->shade = !this->shade;
                                    g_addNotification(Notification(
                                        (this->shade ? "Shading enabled" : "Shading disabled"),
                                        "",
                                        1200
                                    ));
                                }
                            }
                        },
                        { SDL_SCANCODE_M,{ "Toggle slim model",
                                [this]() {
                                    this->slimModel = !this->slimModel;
                                    g_addNotification(Notification(
                                        (this->slimModel ? "Using slim model" : "Using standard model"),
                                        "",
                                        1200
                                    ));
                                }
                            } 
                        },
                        { SDL_SCANCODE_W,{ "Toggle wireframe",
                                [this]() {
                                    this->drawWireframe = !this->drawWireframe;
                                    g_addNotification(Notification(
                                        (this->drawWireframe ? "Wireframe enabled" : "Wireframe disabled"),
                                        "",
                                        1200
                                    ));
                                }
                            } 
                        },
                    }, g_iconNavbarTabView
                }
            },
        },
        { SDL_SCANCODE_F, SDL_SCANCODE_V }
    );
    wxsManager.addDrawable(navbar);

    inEditorPanel = new PanelMCSkinPreview(this);
    caller->addWidget(inEditorPanel);
}

MinecraftSkinPreviewScreen::~MinecraftSkinPreviewScreen()
{
    caller->removeWidget(inEditorPanel);
}

void MinecraftSkinPreviewScreen::render()
{
    renderWithBlurPanelsIfEnabled([this]() {
        renderGradient({ 0,0,g_windowW,g_windowH }, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF202020);

        if (rotAlpha >= 0) {
            renderFloorGrid();
        }

        renderModel(screen00, size);

        if (rotAlpha < 0) {
            renderFloorGrid();
        }
    });

    //g_fnt->RenderString(frmt("alpha rotation: {}", rotAlpha), 5, 5);
    //g_fnt->RenderString(frmt("beta rotation: {}", rotBeta), 5, 25);

    //debugRenderAxes();

    /*SDL_Rect debugDrawBounds = getModelRenderArea(screen00);
    SDL_SetRenderDrawColor(g_rd, 255, 0, 0, 255);
    SDL_RenderDrawRect(g_rd, &debugDrawBounds);*/

    BaseScreen::render();
}

void MinecraftSkinPreviewScreen::takeInput(SDL_Event evt)
{
    if (evt.type == SDL_EVENT_QUIT) {
        g_closeScreen(this);
        return;
    }

    LALT_TO_SUMMON_NAVBAR;

    DrawableManager::processHoverEventInMultiple({ wxsManager }, evt);
    if (!DrawableManager::processInputEventInMultiple({ wxsManager }, evt)) {

        evt = convertTouchToMouseEvent(evt);

        if (evt.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            dragging = evt.button.button;
        }
        else if (evt.type == SDL_EVENT_MOUSE_BUTTON_UP) {
            dragging = -1;
        }
        else if (evt.type == SDL_EVENT_MOUSE_MOTION) {
            if (dragging == SDL_BUTTON_LEFT) {
                rotateFromMouseInput(evt.motion.xrel, evt.motion.yrel);
            }
            else if (dragging == SDL_BUTTON_MIDDLE || dragging == SDL_BUTTON_RIGHT) {
                screen00 = xyAdd(screen00, { (int)evt.motion.xrel, (int)evt.motion.yrel });
            }
        }
        else if (evt.type == SDL_EVENT_MOUSE_WHEEL) {
            size += evt.wheel.y;
            size = dclamp(5, size, 100);
        }
    }
}

void MinecraftSkinPreviewScreen::tick()
{
    if (!dimensionsValidForPreview(caller->canvas.dimensions)) {
        closeThisScreen();
    }
    recalcPointScale();
    BaseScreen::tick();
}

void MinecraftSkinPreviewScreen::recalcPointScale()
{
    pointScale = caller->canvas.dimensions.x / 64.0;
}

PanelMCSkinPreview::PanelMCSkinPreview(MinecraftSkinPreviewScreen* caller) {
    parent = caller;
    wxWidth = 180;
    wxHeight = 240;

    setupDraggable();
    setupCollapsible();
    addTitleText("PREVIEW");
    setupResizable({ 180, 240 });
}

void PanelMCSkinPreview::renderAfterBG(XY at)
{
    SDL_Rect previewArea = getPreviewAreaRect();
    previewArea.x += at.x;
    previewArea.y += at.y;
    g_pushClip(previewArea);
    XY modelOrigin = xyAdd(at, { wxWidth / 2, wxHeight / 12 * 11 });
    double scale = wxHeight / 40.0;
    parent->renderModel(modelOrigin, scale);
    g_popClip();

    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 50);
    SDL_RenderDrawRect(g_rd, &previewArea);
}

SDL_Rect PanelMCSkinPreview::getPreviewAreaRect()
{
    return {
        3, 25,
        wxWidth-6,
        wxHeight-28
    };
}

bool PanelMCSkinPreview::defaultInputAction(SDL_Event evt, XY at)
{
    evt = convertTouchToMouseEvent(evt);
    SDL_Rect previewArea = getPreviewAreaRect();
    if (evt.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (pointInBox(xySubtract({ (int)evt.button.x, (int)evt.button.y }, at), previewArea)) {
            dragging = true;
            return true;
        }
    }
    else if (evt.type == SDL_EVENT_MOUSE_BUTTON_UP) {
        dragging = false;
    }
    else if (evt.type == SDL_EVENT_MOUSE_MOTION) {
        if (dragging) {
            parent->rotateFromMouseInput(evt.motion.xrel, evt.motion.yrel);
            return true;
        }
    }
    return false;
}

PanelMCSkinPreviewSettings::PanelMCSkinPreviewSettings(MinecraftSkinPreviewScreen* caller)
{
    wxWidth = 220;
    wxHeight = 500;

    setupDraggable();
    addTitleText("SETTINGS");

    sizeToContent = true;

    UIStackPanel* hStack = new UIStackPanel();
    hStack->orientationVertical = false;
    hStack->position = { 10,40 };
    hStack->manuallyRecalculateLayout = true;
    wxsTarget().addDrawable(hStack);

    UIStackPanel* checkboxesStack = new UIStackPanel();
    checkboxesStack->spacing = 4;
    checkboxesStack->manuallyRecalculateLayout = true;
    hStack->addWidget(checkboxesStack);

    UISlider* sliderOffsetSize = new UISlider();
    sliderOffsetSize->setValue(0.001, 1.0, caller->overlayOffsetSize);
    sliderOffsetSize->wxHeight = 20;
    sliderOffsetSize->wxWidth = 150;
    sliderOffsetSize->onChangeValueCallback = [caller](UISlider* s, double) {
        caller->overlayOffsetSize = s->getValue(0.001, 1.0);
    };

    checkboxesStack->addWidget(
        UIStackPanel::Horizontal(16, {
            new UILabel("Overlay offset"),
            sliderOffsetSize
        })
    );

    checkboxesStack->addWidget(Panel::Space(2, 16));

    std::vector<std::pair<std::string, bool*>> checkboxesS1 = {
        {"Head", &caller->renderHead},
        {"Body", &caller->renderBody},
        {"Left arm", &caller->renderLArm},
        {"Right arm", &caller->renderRArm},
        {"Left leg", &caller->renderLLeg},
        {"Right leg", &caller->renderRLeg}
    };
    std::vector<std::pair<std::string, bool*>> checkboxesS2 = {
        {"Head overlay", &caller->renderHeadOverlay},
        {"Body overlay", &caller->renderBodyOverlay},
        {"Left arm overlay", &caller->renderLArmOverlay},
        {"Right arm overlay", &caller->renderRArmOverlay},
        {"Left leg overlay", &caller->renderLLegOverlay},
        {"Right leg overlay", &caller->renderRLegOverlay},
    };

    for (auto& [name, target] : checkboxesS1) {
        UICheckbox* cb = new UICheckbox(name, target);
        checkboxesStack->addWidget(cb);
    }

    checkboxesStack->addWidget(Panel::Space(2,16));

    for (auto& [name, target] : checkboxesS2) {
        UICheckbox* cb = new UICheckbox(name, target);
        checkboxesStack->addWidget(cb);
    }

    checkboxesStack->addWidget(Panel::Space(2, 4));

    hStack->addWidget(Panel::Space(12, 4));

    checkboxesStack->recalculateLayout();

    hStack->recalculateLayout();
}
