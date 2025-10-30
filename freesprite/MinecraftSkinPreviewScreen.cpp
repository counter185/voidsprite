#include "MinecraftSkinPreviewScreen.h"
#include "maineditor.h"
#include "FontRenderer.h"
#include "ScreenWideNavBar.h"
#include "Notification.h"
#include "background_operation.h"

BaseScreen* MinecraftSkinPreviewScreen::isSubscreenOf()
{
    return caller;
}

void MinecraftSkinPreviewScreen::renderQuad(XYZd ul, XYZd ur, XYZd dl, XYZd dr, SDL_Rect textureRegion, double shading)
{
    double alpha = rotAlpha * M_PI / 180;
    double beta = rotBeta * M_PI / 180;
    XYd ul2d = worldSpaceToScreenSpace(ul, alpha, beta) * size;
    XYd ur2d = worldSpaceToScreenSpace(ur, alpha, beta) * size;
    XYd dl2d = worldSpaceToScreenSpace(dl, alpha, beta) * size;
    XYd dr2d = worldSpaceToScreenSpace(dr, alpha, beta) * size;

    SDL_Vertex verts[4] =
    {
        {{screen00.x + ul2d.x, screen00.y + ul2d.y}},
        {{screen00.x + ur2d.x, screen00.y + ur2d.y}},
        {{screen00.x + dl2d.x, screen00.y + dl2d.y}},
        {{screen00.x + dr2d.x, screen00.y + dr2d.y}},
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
    for (Layer* l : caller->layers) {
        for (SDL_Vertex& v : verts) {
            v.color.a = l->layerAlpha / 255.0f;
        }
        l->prerender();
        SDL_RenderGeometry(g_rd, l->renderData[g_rd].tex, verts, 4, indices, 6);
    }
}

void MinecraftSkinPreviewScreen::renderBox(XYZd at, double sizeX, double sizeZ, double sizeY, XY textureBoxOrigin)
{
    renderBoxOffset(at, sizeX, sizeZ, sizeY, textureBoxOrigin, 0);
}

void MinecraftSkinPreviewScreen::renderBoxOffset(XYZd at, double sizeX, double sizeZ, double sizeY, XY textureBoxOrigin, double offset)
{
    XYZd l00 = xyzdAdd(at, { -offset,   -offset, -offset }),
        lx0 = xyzdAdd(at,  { sizeX +offset,     -offset, -offset }),
        lxz = xyzdAdd(at,  { sizeX + offset,     -offset, sizeZ + offset }),
        l0z = xyzdAdd(at,  { -offset,   -offset, sizeZ + offset });

    XYZd h00 = xyzdAdd(l00, { -offset,sizeY +offset,-offset }),
        hx0 = xyzdAdd(lx0,  { offset,sizeY +offset,-offset }),
        hxz = xyzdAdd(lxz,  { offset,sizeY +offset,offset }),
        h0z = xyzdAdd(l0z,  { -offset,sizeY +offset,offset });

    //front face
    if (rotBeta <= 90 || rotBeta >= 270) {
        renderQuad(h0z, hxz, l0z, lxz,
            { textureBoxOrigin.x, textureBoxOrigin.y + (int)sizeZ, (int)sizeX, (int)sizeY },
            shade ? shadeFront : 0.0
        );
    }

    //right face
    if (rotBeta >= 0 && rotBeta <= 180) {
        renderQuad(hxz, hx0, lxz, lx0,
            { textureBoxOrigin.x + (int)sizeX, textureBoxOrigin.y + (int)sizeZ, (int)sizeX, (int)sizeY },
            shade ? shadeRight : 0.0
        );
    }

    //left face
    if (rotBeta >= 180) {
        renderQuad(h00, h0z, l00, l0z,
            { textureBoxOrigin.x - (int)sizeZ, textureBoxOrigin.y + (int)sizeZ, (int)sizeZ, (int)sizeY },
            shade ? shadeLeft : 0.0
        );
    }

    //back face
    if (rotBeta >= 90 && rotBeta <= 270) {
        renderQuad(hx0, h00, lx0, l00,
            { textureBoxOrigin.x + (int)sizeX + (int)sizeZ, textureBoxOrigin.y + (int)sizeZ, (int)sizeX, (int)sizeY },
            shade ? shadeBack : 0.0
        );
    }

    //bottom face
    if (rotAlpha < 0) {
        renderQuad(l00, lx0, l0z, lxz,
            { textureBoxOrigin.x + (int)sizeX, textureBoxOrigin.y, (int)sizeX, (int)sizeZ },
            shade ? shadeBottom : 0.0
        );
    }

    //top face
    if (rotAlpha >= 0 && rotAlpha <= 180) {
        renderQuad(h00, hx0, h0z, hxz,
            { textureBoxOrigin.x, textureBoxOrigin.y, (int)sizeX, (int)sizeZ },
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

MinecraftSkinPreviewScreen::MinecraftSkinPreviewScreen(MainEditor* parent) {
    caller = parent;
    screen00 = { g_windowW / 2, g_windowH / 4*3 };
    slimModel = (parent->layers.back()->getPixelAt({ 51, 16 }) & 0xFF000000) == 0;

    navbar = new ScreenWideNavBar(this,
        {
            {
                SDL_SCANCODE_F,
                {
                    TL("vsp.nav.file"),
                    { SDL_SCANCODE_C },
                    {
                        { SDL_SCANCODE_C,{ "Close",
                                [this]() {
                                    g_startNewMainThreadOperation([this]() {g_closeScreen(this); });
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
                    { SDL_SCANCODE_S, SDL_SCANCODE_M },
                    {
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
                    }, g_iconNavbarTabView
                }
            },
        },
        { SDL_SCANCODE_F, SDL_SCANCODE_V }
    );
    wxsManager.addDrawable(navbar);
}

void MinecraftSkinPreviewScreen::render()
{
    renderGradient({ 0,0,g_windowW,g_windowH }, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF202020);

    if (rotAlpha >= 0) {
        renderFloorGrid();
    }

    struct BoxRenderListObj {
        XYZd pos;
        double sx, sz, sy;
        XY texturePos;
        double offs = 0.0;
    };

    double handSizeX = slimModel ? 3 : 4;
    std::vector<std::vector<std::vector<BoxRenderListObj>>> renderList = {
        //legs layer
        {
            {BoxRenderListObj{{ -4,0,0 }, 4, 4, 12, { 4, 16 }}},   //left leg
            {BoxRenderListObj{{ 0,0,0 }, 4, 4, 12, { 20, 48 }}}    //right leg
        },

        //body layer
        {
            {BoxRenderListObj{{ -4.0 - handSizeX,12,0 }, handSizeX, 4, 12, xyAdd({ 40,16 }, { 4,0 })}},    //left arm
            {
                BoxRenderListObj{{ -4,12,0 }, 8, 4, 12, { 20, 16 }},        //body
                BoxRenderListObj{{ -4,12,0 }, 8, 4, 12, { 20, 32 }, 0.2}    //body overlay
            },   
            {BoxRenderListObj{{ 4,12,0 }, handSizeX, 4, 12, xyAdd({ 32,48 }, { 4,0 })}}   //right arm
        },

        //head layer
        {
            {
                BoxRenderListObj{{ -4,24,-2 }, 8, 8, 8, { 8, 0 }},  //head
                BoxRenderListObj{{ -4,24,-2 }, 8, 8, 8, { 40, 0 }, 0.2} //head overlay
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
                renderBoxOffset(y.pos, y.sx, y.sz, y.sy, y.texturePos, y.offs);
            }
        }
    }

    if (rotAlpha < 0) {
        renderFloorGrid();
    }

    //g_fnt->RenderString(frmt("alpha rotation: {}", rotAlpha), 5, 5);
    //g_fnt->RenderString(frmt("beta rotation: {}", rotBeta), 5, 25);

    //debugRenderAxes();

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
                rotBeta -= evt.motion.xrel;
                rotAlpha += evt.motion.yrel;
                if (rotBeta < 0) {
                    rotBeta += 360;
                }
                if (rotBeta > 360) {
                    rotBeta -= 360;
                }
                rotAlpha = dclamp(-90, rotAlpha, 90);
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
