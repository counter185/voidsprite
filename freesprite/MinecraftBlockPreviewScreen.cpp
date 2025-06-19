#include "MinecraftBlockPreviewScreen.h"
#include "maineditor.h"
#include "PanelMCBlockPreview.h"
#include "FontRenderer.h"
#include "PopupTileGeneric.h"

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
    BaseScreen::~BaseScreen();
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

    g_fnt->RenderString(std::format("Select the tile to use for: {}", 
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
            canvas.zoom(evt.wheel.y);
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
    drawIsometricBlock({0, 0, wh.x, wh.y});
    Layer* l = new Layer(wh.x, wh.y);
    SDL_Surface* nsrf = SDL_RenderReadPixels(g_rd, NULL);
    SDL_ConvertPixels(wh.x, wh.y, nsrf->format, nsrf->pixels, nsrf->pitch, SDL_PIXELFORMAT_ARGB8888, l->pixelData, wh.x*4);
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

void MinecraftBlockPreviewScreen::drawIsometricBlock(SDL_Rect at)
{
    if (caller->tileDimensions.x == 0 || caller->tileDimensions.y == 0) {
        return;
    }

    float divSidePoints = isometricBlockScale;

    XY p1 = { at.x + at.w / 2, at.y };
    XY p3 = { at.x + at.w / 2, at.y + at.h };

    XY p4 = { at.x, at.y + (at.h) / divSidePoints };
    XY p5 = { at.x, at.y + (at.h) / divSidePoints * (divSidePoints - 1)};

    XY p2 = { at.x + at.w / 2, at.y + (p4.y - p1.y) * 2 };

    XY p4p = { at.x + at.w, p4.y };
    XY p5p = { at.x + at.w, p5.y };

    SDL_Vertex vertices[7] = {
        {{p1.x, p1.y}},  //p1
        {{p2.x, p2.y}},  //p2
        {{p3.x, p3.y}},  //p3            
        {{p4.x, p4.y}},  //p4
        {{p5.x, p5.y}},  //p5
        {{p4p.x, p4p.y}}, //p4'             
        {{p5p.x, p5p.y}}  //p5'
    };

    for (int i = 0; i < 7; i++) {
        vertices[i].color = toFColor(SDL_Color{0xff,0xff,0xff,0xff});
    }

    SDL_Rect topTextureRect =
        canvas.getTileRectAt(tileTop, caller->tileDimensions);
    SDL_Rect sideLeftTextureRect = 
        canvas.getTileRectAt(tileSideLeft, caller->tileDimensions);
    SDL_Rect sideRightTextureRect =
        canvas.getTileRectAt(tileSideRight, caller->tileDimensions);

    int indicesTop[] = { 0, 5, 1, 0, 1, 3 };
    int indicesSideLeft[] = { 3, 1, 2, 3, 2, 4 };
    int indicesSideRight[] = { 1, 5, 6, 1, 6, 2 };

    //draw top
    if (!xyEqual(tileTop, { -1,-1 })) {
        vertices[0].tex_coord = { (float)topTextureRect.x, (float)topTextureRect.y };
        vertices[5].tex_coord = { (float)topTextureRect.x + topTextureRect.w,
                                 (float)topTextureRect.y };
        vertices[1].tex_coord = { (float)topTextureRect.x + topTextureRect.w,
                                 (float)topTextureRect.y + topTextureRect.h };
        vertices[3].tex_coord = { (float)topTextureRect.x,
                                 (float)topTextureRect.y + topTextureRect.h };

        for (int i = 0; i < 7; i++) {
            vertices[i].tex_coord.x /= (float)caller->canvas.dimensions.x;
            vertices[i].tex_coord.y /= (float)caller->canvas.dimensions.y;
        }

        for (Layer*& l : caller->layers) {
            //SDL_RenderCopy(g_rd, , NULL, &canvasRenderRect);
            for (int i = 0; i < 7; i++) {
                vertices[i].color.a = l->layerAlpha / 255.0f;
            }
            int r = SDL_RenderGeometry(g_rd, l->tex, vertices, 7, indicesTop, 6);
        }
    }

    //draw left side
    if (!xyEqual(tileSideLeft, { -1,-1 })) {

        if (shadeSides) {
            for (int i = 0; i < 7; i++) {
                vertices[i].color = toFColor(SDL_Color{ 0xff / 4 * 3,0xff / 4 * 3,0xff / 4 * 3, 0xff });
            }
        }

        vertices[3].tex_coord = { (float)sideLeftTextureRect.x, (float)sideLeftTextureRect.y };
        vertices[1].tex_coord = { (float)sideLeftTextureRect.x + sideLeftTextureRect.w,
                                 (float)sideLeftTextureRect.y };
        vertices[2].tex_coord = { (float)sideLeftTextureRect.x + sideLeftTextureRect.w,
                                 (float)sideLeftTextureRect.y + sideLeftTextureRect.h };
        vertices[4].tex_coord = { (float)sideLeftTextureRect.x,
                                 (float)sideLeftTextureRect.y + sideLeftTextureRect.h };

        for (int i = 0; i < 7; i++) {
            vertices[i].tex_coord.x /= (float)caller->canvas.dimensions.x;
            vertices[i].tex_coord.y /= (float)caller->canvas.dimensions.y;
        }

        for (Layer*& l : caller->layers) {
            //SDL_RenderCopy(g_rd, , NULL, &canvasRenderRect);
            for (int i = 0; i < 7; i++) {
                vertices[i].color.a = l->layerAlpha / 255.0f;
            }
            int r = SDL_RenderGeometry(g_rd, l->tex, vertices, 7, indicesSideLeft, 6);
        }
    }

    //draw right side
    if (!xyEqual(tileSideRight, { -1,-1 })) {

        if (shadeSides) {
            for (int i = 0; i < 7; i++) {
                vertices[i].color = toFColor(SDL_Color{ 0xff / 2, 0xff / 2, 0xff / 2, 0xff });
            }
        }

        vertices[1].tex_coord = { (float)sideRightTextureRect.x, (float)sideRightTextureRect.y };
        vertices[5].tex_coord = { (float)sideRightTextureRect.x + sideRightTextureRect.w,
                                 (float)sideRightTextureRect.y };
        vertices[6].tex_coord = { (float)sideRightTextureRect.x + sideRightTextureRect.w,
                                 (float)sideRightTextureRect.y + sideRightTextureRect.h };
        vertices[2].tex_coord = { (float)sideRightTextureRect.x,
                                 (float)sideRightTextureRect.y + sideRightTextureRect.h };

        for (int i = 0; i < 7; i++) {
            vertices[i].tex_coord.x /= (float)caller->canvas.dimensions.x;
            vertices[i].tex_coord.y /= (float)caller->canvas.dimensions.y;
        }

        for (Layer*& l : caller->layers) {
            //SDL_RenderCopy(g_rd, , NULL, &canvasRenderRect);
            for (int i = 0; i < 7; i++) {
                vertices[i].color.a = l->layerAlpha / 255.0f;
            }
            int r = SDL_RenderGeometry(g_rd, l->tex, vertices, 7, indicesSideRight, 6);
        }
    }
    

}
