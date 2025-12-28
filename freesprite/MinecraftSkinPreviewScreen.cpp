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

void MinecraftSkinPreviewScreen::renderQuad(XY origin00, double scale, XYZd ul, XYZd ur, XYZd dl, XYZd dr, SDL_Rect textureRegion, double shading)
{
    textureRegion = {
        (int)(textureRegion.x * pointScale),
        (int)(textureRegion.y * pointScale),
        (int)(textureRegion.w * pointScale),
        (int)(textureRegion.h * pointScale)
    };


    double alpha = rotAlpha * M_PI / 180;
    double beta = rotBeta * M_PI / 180;
    XYd ul2d = worldSpaceToScreenSpace(ul, alpha, beta) * scale;
    XYd ur2d = worldSpaceToScreenSpace(ur, alpha, beta) * scale;
    XYd dl2d = worldSpaceToScreenSpace(dl, alpha, beta) * scale;
    XYd dr2d = worldSpaceToScreenSpace(dr, alpha, beta) * scale;

    SDL_Vertex verts[4] =
    {
        {{origin00.x + ul2d.x, origin00.y + ul2d.y}},
        {{origin00.x + ur2d.x, origin00.y + ur2d.y}},
        {{origin00.x + dl2d.x, origin00.y + dl2d.y}},
        {{origin00.x + dr2d.x, origin00.y + dr2d.y}},
    };

    for (auto& v : verts) {
        v.color = SDL_FColor{ (float)(1.0f - shading),(float)(1.0f - shading),(float)(1.0f - shading),1.0f };
    }

    verts[0].tex_coord = { (float)textureRegion.x, (float)textureRegion.y };
    verts[1].tex_coord = { (float)(textureRegion.x + textureRegion.w), (float)textureRegion.y };
    verts[2].tex_coord = { (float)textureRegion.x, (float)(textureRegion.y + textureRegion.h) };
    verts[3].tex_coord = { (float)(textureRegion.x + textureRegion.w), (float)(textureRegion.y + textureRegion.h) };

    for (auto& v : verts) {
        v.tex_coord.x /= (float)caller->canvas.dimensions.x;
        v.tex_coord.y /= (float)caller->canvas.dimensions.y;
    }

    int indices[] = {0,1,2,2,1,3};
    for (Layer* l : caller->getLayerStack()) {
        for (SDL_Vertex& v : verts) {
            v.color.a = l->layerAlpha / 255.0f;
        }
        l->prerender();
        SDL_RenderGeometry(g_rd, l->renderData[g_rd].tex, verts, 4, indices, 6);
    }

    if (drawWireframe) {
        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x90);
        drawLine(xyAdd(origin00, xydToXy(ul2d)), xyAdd(origin00, xydToXy(ur2d)));
        drawLine(xyAdd(origin00, xydToXy(ur2d)), xyAdd(origin00, xydToXy(dr2d)));
        drawLine(xyAdd(origin00, xydToXy(dr2d)), xyAdd(origin00, xydToXy(dl2d)));
        drawLine(xyAdd(origin00, xydToXy(ul2d)), xyAdd(origin00, xydToXy(dl2d)));
    }
}

void MinecraftSkinPreviewScreen::renderBox(XY origin00, double scale, XYZd at, double sizeX, double sizeZ, double sizeY, XY textureBoxOrigin, double offset, bool flipUVX = false)
{
    XYZd l00 = xyzdAdd(at, { -offset,        -offset,  -offset }),
        lx0 = xyzdAdd(at,  { sizeX +offset,  -offset,  -offset }),
        lxz = xyzdAdd(at,  { sizeX + offset, -offset,  sizeZ + offset }),
        l0z = xyzdAdd(at,  { -offset,        -offset,  sizeZ + offset });

    XYZd h00 = xyzdAdd(l00, { 0, sizeY +offset*2, 0}),
        hx0 = xyzdAdd(lx0,  { 0, sizeY +offset*2, 0}),
        hxz = xyzdAdd(lxz,  { 0, sizeY +offset*2, 0}),
        h0z = xyzdAdd(l0z,  { 0, sizeY +offset*2, 0});

    SDL_Rect frontFace = { textureBoxOrigin.x, textureBoxOrigin.y + (int)sizeZ, (int)sizeX, (int)sizeY };
    SDL_Rect rightFace = { textureBoxOrigin.x + (int)sizeX, textureBoxOrigin.y + (int)sizeZ, (int)sizeZ, (int)sizeY };
    SDL_Rect leftFace = { textureBoxOrigin.x - (int)sizeZ, textureBoxOrigin.y + (int)sizeZ, (int)sizeZ, (int)sizeY };
    SDL_Rect backFace = { textureBoxOrigin.x + (int)sizeX + (int)sizeZ, textureBoxOrigin.y + (int)sizeZ, (int)sizeX, (int)sizeY };
    SDL_Rect bottomFace = { textureBoxOrigin.x + (int)sizeX, textureBoxOrigin.y, (int)sizeX, (int)sizeZ };
    SDL_Rect topFace = { textureBoxOrigin.x, textureBoxOrigin.y, (int)sizeX, (int)sizeZ };

    if (flipUVX) {
        frontFace = uvFlipHorizontal(frontFace);
        std::swap(leftFace, rightFace);
        leftFace = uvFlipHorizontal(leftFace);
        rightFace = uvFlipHorizontal(rightFace);
        bottomFace = uvFlipHorizontal(bottomFace);
        topFace = uvFlipHorizontal(topFace);
        backFace = uvFlipHorizontal(backFace);
    }

    //front face
    if (rotBeta <= 90 || rotBeta >= 270) {
        renderQuad(origin00, scale, h0z, hxz, l0z, lxz, frontFace,
            shade ? shadeFront : 0.0
        );
    }

    //right face
    if (rotBeta >= 0 && rotBeta <= 180) {
        renderQuad(origin00, scale, hxz, hx0, lxz, lx0, rightFace,
            shade ? shadeRight : 0.0
        );
    }

    //left face
    if (rotBeta >= 180) {
        renderQuad(origin00, scale, h00, h0z, l00, l0z, leftFace,
            shade ? shadeLeft : 0.0
        );
    }

    //back face
    if (rotBeta >= 90 && rotBeta <= 270) {
        renderQuad(origin00, scale, hx0, h00, lx0, l00, backFace,
            shade ? shadeBack : 0.0
        );
    }

    //bottom face
    if (rotAlpha < 0) {
        renderQuad(origin00, scale, l00, lx0, l0z, lxz, bottomFace,
            shade ? shadeBottom : 0.0
        );
    }

    //top face
    if (rotAlpha >= 0 && rotAlpha <= 180) {
        renderQuad(origin00, scale, h00, hx0, h0z, hxz, topFace,
            shade ? shadeTop : 0.0
        );
    }
}

XYd MinecraftSkinPreviewScreen::worldSpaceToScreenSpace(XYZd point, double alpha, double beta)
{
    //so that y+ is up
    point.y *= -1;

    matrix a = {
        {1, 0, 0},
        {0, cos(alpha), sin(alpha)},
        {0, -sin(alpha), cos(alpha)}
    };

    matrix b = {
        {cos(beta), 0, -sin(beta)},
        {0, 1, 0},
        {sin(beta), 0, cos(beta)}
    };

    matrix c = { {point.x}, {point.y}, {point.z} };

    matrix xyId = {
        {1,0,0},
        {0,1,0},
        {0,0,0}
    };

    matrix r = matrixMultiply(xyId, matrixMultiply(matrixMultiply(a, b), c));

    return XYd{ r[0][0], r[1][0] };
}

XY MinecraftSkinPreviewScreen::scaledPoint(XY point)
{
    return { (int)(point.x * pointScale), (int)(point.y * pointScale) };
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
                    renderBox(origin00, scale, y.pos, y.sx, y.sz, y.sy, y.texturePos, y.offs, y.flipX);
                }
            }
        }
    }
}

void MinecraftSkinPreviewScreen::renderFloorGrid()
{
    double lineLength = 60;
    for (int i = -8; i <= 8; i++) {
        double gridSpaced = i * 4;
        XY proj = xyAdd(screen00, xydToXy(worldSpaceToScreenSpace({ (double)gridSpaced,0,-lineLength }, rotAlpha * M_PI / 180, rotBeta * M_PI / 180) * size));
        XY proj2 = xyAdd(screen00, xydToXy(worldSpaceToScreenSpace({ (double)gridSpaced,0,lineLength }, rotAlpha * M_PI / 180, rotBeta * M_PI / 180) * size));

        XY projA = xyAdd(screen00, xydToXy(worldSpaceToScreenSpace({ -lineLength, 0, (double)gridSpaced }, rotAlpha * M_PI / 180, rotBeta * M_PI / 180) * size));
        XY projA2 = xyAdd(screen00, xydToXy(worldSpaceToScreenSpace({ lineLength, 0, (double)gridSpaced }, rotAlpha * M_PI / 180, rotBeta * M_PI / 180) * size));

        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 30 / ixmax(1, abs(i)));
        drawLine(proj, proj2);
        drawLine(projA, projA2);
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

SDL_Rect MinecraftSkinPreviewScreen::uvFlipHorizontal(SDL_Rect x)
{
    return { x.x + x.w, x.y, -x.w, x.h };
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
    renderGradient({ 0,0,g_windowW,g_windowH }, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF202020);

    if (rotAlpha >= 0) {
        renderFloorGrid();
    }

    renderModel(screen00, size);

    if (rotAlpha < 0) {
        renderFloorGrid();
    }

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

void MinecraftSkinPreviewScreen::rotateFromMouseInput(double xrel, double yrel)
{
    rotBeta -= xrel;
    rotAlpha += yrel;
    if (rotBeta < 0) {
        rotBeta += 360;
    }
    if (rotBeta > 360) {
        rotBeta -= 360;
    }
    rotAlpha = dclamp(-90, rotAlpha, 90);
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
