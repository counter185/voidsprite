#include "MinecraftBlockPreviewScreen.h"
#include "maineditor.h"
#include "PanelMCBlockPreview.h"
#include "FontRenderer.h"
#include "PopupTileGeneric.h"

void MinecraftBlockPreviewScreen::mapRectToVerts(SDL_Vertex* verts, std::vector<int> indices, SDL_Rect r)
{
    verts[indices[0]].tex_coord = { (float)r.x, (float)r.y };   //top left
    verts[indices[1]].tex_coord = { (float)r.x + r.w, (float)r.y }; //top right
    verts[indices[2]].tex_coord = { (float)r.x + r.w, (float)r.y + r.h }; //bottom right
    verts[indices[3]].tex_coord = { (float)r.x, (float)r.y + r.h }; //bottom left
}

MinecraftBlockPreviewScreen::MinecraftBlockPreviewScreen(MainEditor* parent)
{
    caller = parent;
    canvas.scale = parent->canvas.scale;
    canvas.dimensions = parent->canvas.dimensions;
    canvas.recenter();

    panelBig = new PanelMCBlockPreview(this);
    panelBig->position = { 15, 40 };
    wxsManager.addDrawable(panelBig);

    panelSmalm = new PanelMCBlockPreview(this, true);
    panelSmalm->position = { 430, 60 };
    parent->addWidget(panelSmalm);

    navbar = new ScreenWideNavBar(this,
        {
            {
                SDL_SCANCODE_F,
                {
                    "File",
                    {},
                    {
                        {SDL_SCANCODE_R, { "Render to separate workspace",
                                [this]() {
                                    g_addPopup(new PopupTileGeneric(this, "Render to workspace", "Image dimensions:", {512,512}, 3));
                                }
                            }
                        },
                        {SDL_SCANCODE_C, { "Close",
                                [this]() {
                                    this->closeNextTick = true;
                                }
                            }
                        },
                    },
                    g_iconNavbarTabFile
                }
            },
        }, { SDL_SCANCODE_F });
    wxsManager.addDrawable(navbar);
}

MinecraftBlockPreviewScreen::~MinecraftBlockPreviewScreen()
{
    caller->removeWidget(panelSmalm);
}

void MinecraftBlockPreviewScreen::render()
{
    drawBackground();

    canvas.dimensions = caller->canvas.dimensions;
    SDL_Rect canvasRenderRect = canvas.getCanvasOnScreenRect();// { canvasDrawOrigin.x, canvasDrawOrigin.y, caller->canvas.dimensions.x* canvasZoom, caller->canvas.dimensions.y* canvasZoom };
    for (Layer*& l : caller->layers) {
        l->render(canvasRenderRect, l->layerAlpha);
        //SDL_RenderCopy(g_rd, l->tex, NULL, &canvasRenderRect);
    }

    // lines between tiles
    SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
    canvas.drawTileGrid(caller->tileDimensions);

    //canvas border
    canvas.drawCanvasOutline(5, SDL_Color{ 255,255,255,0x80 });

    XY tiles[] = { tileTop, tileSideLeft, tileSideRight };
    SDL_Color tileColors[] = { 
        {0x3c,0x40,0xff,0xff}, 
        {0x46,0xff,0x3c,0xff}, 
        {0xff,0x3c,0x3c,0xff} 
    };
    for (int i = 0; i < 3; i++) {
        if (!xyEqual(tiles[i], {-1,-1})) {
            SDL_Rect p = canvas.getTileScreenRectAt(tiles[i], caller->tileDimensions);
            SDL_SetRenderDrawColor(g_rd, tileColors[i].r, tileColors[i].g, tileColors[i].b, tileColors[i].a);
            SDL_RenderDrawRect(g_rd, &p);
            SDL_SetRenderDrawColor(g_rd, tileColors[i].r, tileColors[i].g, tileColors[i].b, tileColors[i].a / 2);
            SDL_RenderFillRect(g_rd, &p);
        }
    }

    drawBottomBar();

    g_fnt->RenderString(frmt("Select the tile to use for: {}", 
        choosingSide == 0 ? "top side"
        : choosingSide == 1 ? "left side"
        : "right side"),
        2, g_windowH - 28, SDL_Color{ 255,255,255,0xa0 });

    BaseScreen::render();
}

void MinecraftBlockPreviewScreen::takeInput(SDL_Event evt)
{
    DrawableManager::processHoverEventInMultiple({ wxsManager }, evt);

    if (evt.type == SDL_QUIT) {
        g_closeScreen(this);
        return;
    }

    LALT_TO_SUMMON_NAVBAR;

    if (!DrawableManager::processInputEventInMultiple({ wxsManager }, evt)) {
        switch (evt.type) {
        case SDL_MOUSEBUTTONDOWN:
            if (evt.button.button == SDL_BUTTON_MIDDLE) {
                scrollingCanvas = true;
            }
            else if (evt.button.button == SDL_BUTTON_LEFT) {
                if (caller->tileDimensions.x != 0 && caller->tileDimensions.y != 0
                    && canvas.pointInCanvasBounds(canvas.screenPointToCanvasPoint({ (int)evt.button.x, (int)evt.button.y })))
                {
                    XY tile = canvas.getTilePosAt(XY{ (int)evt.button.x, (int)evt.button.y }, caller->tileDimensions);
                    //do thing here
                    XY* ws[] = { &tileTop, &tileSideLeft, &tileSideRight };
                    *ws[choosingSide++] = tile;
                    choosingSide %= 3;
                }
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (evt.button.button == SDL_BUTTON_MIDDLE) {
                scrollingCanvas = false;
            }
            break;
        case SDL_MOUSEMOTION:
            if (scrollingCanvas) {
                canvas.panCanvas(XY{ (int)(evt.motion.xrel), (int)(evt.motion.yrel) });
            }
            break;
        case SDL_MOUSEWHEEL:
            canvas.zoomFromWheelInput(evt.wheel.y);
            break;
        }
    }
}

void MinecraftBlockPreviewScreen::tick()
{
    if (closeNextTick || caller->tileDimensions.x == 0 || caller->tileDimensions.y == 0) {
        g_closeScreen(this);
        return;
    }

    canvas.lockToScreenBounds(0, 0, 0, 0);

    panelBig->tryMoveOutOfOOB();
    panelSmalm->tryMoveOutOfOOB();
}

BaseScreen* MinecraftBlockPreviewScreen::isSubscreenOf()
{
    return caller;
}

void MinecraftBlockPreviewScreen::eventPopupClosed(int evt_id, BasePopup* target)
{
    if (evt_id == 3) {
        renderToWorkspace(((PopupTileGeneric*)target)->result);
    }
}

void MinecraftBlockPreviewScreen::renderToWorkspace(XY wh)
{
    SDL_Texture* renderTarget = tracked_createTexture(g_rd, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, wh.x, wh.y);
    g_pushRenderTarget(renderTarget);

    SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0);
    SDL_RenderClear(g_rd);
    drawIsometricBlockV2({0, 0, wh.x, wh.y});
    Layer* l = new Layer(wh.x, wh.y);
    SDL_Surface* nsrf = SDL_RenderReadPixels(g_rd, NULL);
    SDL_ConvertPixels(wh.x, wh.y, nsrf->format, nsrf->pixels, nsrf->pitch, SDL_PIXELFORMAT_ARGB8888, l->pixels32(), wh.x*4);
    SDL_FreeSurface(nsrf);

    g_popRenderTarget();

    MainEditor *newSession = new MainEditor(l);
    g_addScreen(newSession);
}

void MinecraftBlockPreviewScreen::drawBackground()
{
    renderGradient({ 0,0,g_windowW,g_windowH }, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF202020);

    uint64_t bgtimer = g_config.animatedBackground >= 3 ? 0 : SDL_GetTicks64();
    if (g_config.animatedBackground != 0) {
        int lineY = 400;
        int lineMovementInTime = 60;
        int lineOffset = (bgtimer % 5000 / 5000.0 * 60);
    }
}

void MinecraftBlockPreviewScreen::drawIsometricBlockV2(SDL_Rect at)
{
    double a = isomAlpha * M_PI / 180;
    double b = isomBeta * M_PI / 180;

    XYd scale = { at.w / 2.0, at.h/2.0 };
    //XYd scaleToWholeArea = XYd{ 2.0 / sqrt(2.0), 2.0 / sqrt(3.0) };   //stretch
    XYd scaleToWholeArea = XYd{ 2.0 / sqrt(3.0), 2.0 / sqrt(3.0) };
    
    XY origin00 = { at.x + at.w / 2, at.y + at.h/2 };

    double centerX = 0.5, centerZ = 0.5, centerY = 0.5;

    XY p1 =  xyAdd(origin00, xydToXy(projectPointIsom(0-centerX, 0-centerY, 0-centerZ, a, b) * scaleToWholeArea * scale));
    XY p2 =  xyAdd(origin00, xydToXy(projectPointIsom(1-centerX, 0-centerY, 0-centerZ, a, b) * scaleToWholeArea * scale));
    XY p3 =  xyAdd(origin00, xydToXy(projectPointIsom(0-centerX, 0-centerY, 1-centerZ, a, b) * scaleToWholeArea * scale));
    XY p4 =  xyAdd(origin00, xydToXy(projectPointIsom(1-centerX, 0-centerY, 1-centerZ, a, b) * scaleToWholeArea * scale));

    XY pL1 = xyAdd(origin00, xydToXy(projectPointIsom(0-centerX, 1-centerY, 0-centerZ, a, b) * scaleToWholeArea * scale));
    XY pL2 = xyAdd(origin00, xydToXy(projectPointIsom(1-centerX, 1-centerY, 0-centerZ, a, b) * scaleToWholeArea * scale));
    XY pL3 = xyAdd(origin00, xydToXy(projectPointIsom(0-centerX, 1-centerY, 1-centerZ, a, b) * scaleToWholeArea * scale));
    XY pL4 = xyAdd(origin00, xydToXy(projectPointIsom(1-centerX, 1-centerY, 1-centerZ, a, b) * scaleToWholeArea * scale));

    /*SDL_SetRenderDrawColor(g_rd, 80, 80, 80, 255);
    drawLine(pL1, pL2);
    drawLine(pL2, pL4);
    drawLine(pL4, pL3);
    drawLine(pL3, pL1);

    SDL_SetRenderDrawColor(g_rd, 127, 127, 127, 255);
    drawLine(pL1, p1);
    drawLine(pL2, p2);
    drawLine(pL4, p4);
    drawLine(pL3, p3);

    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 255);
    drawLine(p1, p2);
    drawLine(p2, p4);
    drawLine(p4, p3);
    drawLine(p3, p1);*/

    SDL_Rect topTextureRect =
        canvas.getTileRectAt(tileTop, caller->tileDimensions);
    SDL_Rect sideLeftTextureRect =
        canvas.getTileRectAt(tileSideLeft, caller->tileDimensions);
    SDL_Rect sideRightTextureRect =
        canvas.getTileRectAt(tileSideRight, caller->tileDimensions);

    SDL_Vertex vertices[8] = {
        {{p1.x, p1.y}},
        {{p2.x, p2.y}},
        {{p3.x, p3.y}},            
        {{p4.x, p4.y}},
        {{pL1.x, pL1.y}},
        {{pL2.x, pL2.y}},
        {{pL3.x, pL3.y}},            
        {{pL4.x, pL4.y}}
    };

    

    int indicesTop[] = { 0, 1, 3, 0, 3, 2 };
    int indicesSideLeft[] = { 2, 3, 6, 3, 6, 7 };
    int indicesSideRight[] = { 3,1,7, 1, 7, 5 };

    for (SDL_Vertex& v : vertices) {
        v.color = SDL_FColor{ 1.0f,1.0f,1.0f,1.0f };
    }

    if (!xyEqual(tileTop, { -1,-1 })) {
        mapRectToVerts(vertices, { 0,1,3,2 }, topTextureRect);

        for (SDL_Vertex& v : vertices) {
            v.tex_coord.x /= (float)caller->canvas.dimensions.x;
            v.tex_coord.y /= (float)caller->canvas.dimensions.y;
        }

        for (Layer*& l : caller->layers) {
            for (SDL_Vertex& v : vertices) {
                v.color.a = l->layerAlpha / 255.0f;
            }
            int r = SDL_RenderGeometry(g_rd, l->renderData[g_rd].tex, vertices, 8, indicesTop, 6);
        }
    }

    if (!xyEqual(tileSideLeft, { -1,-1 })) {
        mapRectToVerts(vertices, { 2,3,7,6 }, sideLeftTextureRect);

        if (shadeSides) {
            for (SDL_Vertex& v : vertices) {
                v.color = toFColor(SDL_Color{ 0xff / 4 * 3,0xff / 4 * 3,0xff / 4 * 3, 0xff });
            }
        }

        for (SDL_Vertex& v : vertices) {
            v.tex_coord.x /= (float)caller->canvas.dimensions.x;
            v.tex_coord.y /= (float)caller->canvas.dimensions.y;
        }

        for (Layer*& l : caller->layers) {
            for (SDL_Vertex& v : vertices) {
                v.color.a = l->layerAlpha / 255.0f;
            }
            int r = SDL_RenderGeometry(g_rd, l->renderData[g_rd].tex, vertices, 8, indicesSideLeft, 6);
        }
    }

    if (!xyEqual(tileSideRight, { -1,-1 })) {
        mapRectToVerts(vertices, { 3,1,5,7 }, sideRightTextureRect);

        if (shadeSides) {
            for (SDL_Vertex& v : vertices) {
                v.color = toFColor(SDL_Color{ 0xff / 2, 0xff / 2, 0xff / 2, 0xff });
            }
        }

        for (SDL_Vertex& v : vertices) {
            v.tex_coord.x /= (float)caller->canvas.dimensions.x;
            v.tex_coord.y /= (float)caller->canvas.dimensions.y;
        }

        for (Layer*& l : caller->layers) {
            for (SDL_Vertex& v : vertices) {
                v.color.a = l->layerAlpha / 255.0f;
            }
            int r = SDL_RenderGeometry(g_rd, l->renderData[g_rd].tex, vertices, 8, indicesSideRight, 6);
        }
    }

    /*
    u32 colors[8] = {
        0xFFFFFFFF,
        0xFFFF0000,
        0xFF00FF00,
        0xFF0000FF,
        0xFFFFFF00,
        0xFF00FFFF,
        0xFFFF00FF,
        0xFFA0A0A0,
    };
    
    for (int x = 0; x < 8; x++) {
        SDL_Color ccc = uint32ToSDLColor(colors[x]);
        SDL_SetRenderDrawColor(g_rd, ccc.r, ccc.g, ccc.b, ccc.a);
        SDL_RenderPoint(g_rd, vertices[x].position.x, vertices[x].position.y);
        g_fnt->RenderString(frmt("{}", x), vertices[x].position.x + 2, vertices[x].position.y + 2, ccc);
    }*/
}

XYd MinecraftBlockPreviewScreen::projectPointIsom(double x, double y, double z, double alpha, double beta)
{
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

    matrix c = { {x}, {y}, {z} };

    matrix xyId = {
        {1,0,0},
        {0,1,0},
        {0,0,0}
    };

    matrix r = matrixMultiply(xyId, matrixMultiply(matrixMultiply(a, b), c));

    return XYd{ r[0][0], r[1][0] };

}
