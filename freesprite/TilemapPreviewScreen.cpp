#include "TilemapPreviewScreen.h"
#include "maineditor.h"
#include "FontRenderer.h"
#include "ScreenWideNavBar.h"
#include "Notification.h"
#include "TilemapEditorLayerPicker.h"
#include "PanelTilemapPreview.h"
#include "FileIO.h"
#include "PopupTileGeneric.h"

TilemapPreviewScreen::TilemapPreviewScreen(MainEditor* parent) {
    caller = parent;

    navbar = new ScreenWideNavBar<TilemapPreviewScreen*>(this,
        {
            {
                SDLK_f,
                {
                    "File",
                    {SDLK_o, SDLK_s, SDLK_z, SDLK_x, SDLK_c},
                    {
                        {SDLK_o, { "Load layout from file",
                                [](TilemapPreviewScreen* screen) {
                                    platformTryLoadOtherFile(screen, {{".voidtile", "voidtile layout"}, {".pxm", "Cave Story Map PXM"}}, "load tile layout", EVENT_TILEMAP_LOADLAYOUT);
                                }
                            }
                        },
                        {SDLK_s, { "Save layout to file",
                                [](TilemapPreviewScreen* screen) {
                                    platformTrySaveOtherFile(screen, { {".voidtile", "voidtile layout"}, {".pxm", "Cave Story Map PXM"}}, "save tile layout", EVENT_TILEMAP_SAVELAYOUT);
                                }
                            }
                        },
                        {SDLK_z, { "Render all layers to image",
                                [](TilemapPreviewScreen* screen) {
                                    screen->promptRenderMap(EVENT_TILEMAP_RENDERALLLTOIMAGE);
                                }
                            }
                        },
                        {SDLK_x, { "Render all layers to separate images",
                                [](TilemapPreviewScreen* screen) {
                                    screen->promptRenderMap(EVENT_TILEMAP_RENDERALLLTOIMAGES);
                                }
                            }
                        },
                        {SDLK_c, { "Render current layer to image",
                                [](TilemapPreviewScreen* screen) {
                                    screen->promptRenderMap(EVENT_TILEMAP_RENDERCURRENTLTOIMAGE);
                                }
                            }
                        },
                    },
                    g_iconNavbarTabFile
                }
            }, 
            {
                SDLK_e,
                {
                    "Edit",
                    {},
                    {
                        {SDLK_r, { "Resize tilemap",
                                [](TilemapPreviewScreen* screen) {
                                    g_addPopup(new PopupTileGeneric(screen, "Resize tilemap", "Enter the new size of the tilemap (in tiles)", screen->tilemapDimensions, EVENT_TILEMAP_RESIZE));
                                }
                            }
                        }
                    },
                    g_iconNavbarTabEdit
                }
            }
        }, { SDLK_f, SDLK_e });
    wxsManager.addDrawable(navbar);

    PanelTilemapPreview* panel = new PanelTilemapPreview(this);
    wxsManager.addDrawable(panel);

    resizeTilemap(32, 32);

    layerPicker = new TilemapEditorLayerPicker(this);
    wxsManager.addDrawable(layerPicker);

    tileSelectScale = caller->canvas.scale;
    recenterTilePicker();
    recenterTilemap();
}

TilemapPreviewScreen::~TilemapPreviewScreen()
{
    for (int l = 0; l < tilemap.size(); l++) {
        XY**& tilemapLayer = tilemap.at(l);
        if (tilemapLayer != NULL) {
            for (int y = 0; y < tilemapDimensions.y; y++) {
                tracked_free(tilemapLayer[y]);
            }
            tracked_free(tilemapLayer);
        }
    }
    tilemap.clear();
}

void TilemapPreviewScreen::render()
{
    drawBackground();

    XY callerTileSize = caller->getPaddedTileDimensions();

    canvas.dimensions = { tilemapDimensions.x * callerTileSize.x, tilemapDimensions.y * callerTileSize.y };

    SDL_Rect tilemapDrawRect = canvas.getCanvasOnScreenRect();

    for (int l = 0; l < tilemap.size(); l++) {
        for (int y = 0; y < tilemapDimensions.y; y++) {
            for (int x = 0; x < tilemapDimensions.x; x++) {

                XY tilePos = tilemap[l][y][x];
                if (tilePos.x < 0 || tilePos.y < 0) {
                    continue;
                }

                SDL_Rect tileDraw = canvas.getTileScreenRectAt({ x,y }, callerTileSize);/* {
                    tilemapDrawRect.x + callerTileSize.x * tilemapScale * x,
                    tilemapDrawRect.y + callerTileSize.y * tilemapScale * y,
                    callerTileSize.x * tilemapScale,
                    callerTileSize.y * tilemapScale
                };*/

                SDL_Rect tileClip = caller->getPaddedTilePosAndDimensions(tilePos);

                double alpha = (layerSelectTimer.started && l == activeLayerIndex()) ? XM1PW3P1(layerSelectTimer.percentElapsedTime(1300)) : 1.0;
                for (Layer* l : caller->layers) {
                    l->render(tileDraw, tileClip, l->layerAlpha * alpha);
                }
            }
        }
    }
    canvas.drawTileGrid(callerTileSize);

    //tilemap border
    canvas.drawCanvasOutline(5, { 255,255,255,0xa0 });

    //tilemap current hovered tile
    SDL_Rect tilemapSelectedTile = canvas.getTileScreenRectAt(hoveredTilePosition, callerTileSize);
    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xd0);
    SDL_RenderDrawRect(g_rd, &tilemapSelectedTile);


    //render tile select menu
    if (tileSelectOpen) {

        SDL_Rect tsbgRect = { 0,0, g_windowW * XM1PW3P1(tileSelectTimer.percentElapsedTime(300)), g_windowH};
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, (uint8_t)(0xd0 * XM1PW3P1(tileSelectTimer.percentElapsedTime(300))));
        SDL_RenderFillRect(g_rd, &tsbgRect);

        int xOffset = -g_windowW * (1.0 - XM1PW3P1(tileSelectTimer.percentElapsedTime(300)));

        SDL_Rect tileSelectRect = { tileSelectOffset.x + xOffset, tileSelectOffset.y,
            caller->canvas.dimensions.x * tileSelectScale,
            caller->canvas.dimensions.y * tileSelectScale
        };

        for (Layer* l : caller->layers) {
            l->render(tileSelectRect, l->layerAlpha);
        }

        SDL_Rect pickedTileRect = {
            tileSelectRect.x + pickedTile.x * caller->tileDimensions.x * tileSelectScale,
            tileSelectRect.y + pickedTile.y * caller->tileDimensions.y * tileSelectScale,
            caller->tileDimensions.x * tileSelectScale,
            caller->tileDimensions.y * tileSelectScale
        };
        SDL_Rect hoveredTileRect = {
            tileSelectRect.x + tileSelectHoveredTile.x * caller->tileDimensions.x * tileSelectScale,
            tileSelectRect.y + tileSelectHoveredTile.y * caller->tileDimensions.y * tileSelectScale,
            caller->tileDimensions.x * tileSelectScale,
            caller->tileDimensions.y * tileSelectScale
        };
        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xe0);
        SDL_RenderDrawRect(g_rd, &pickedTileRect);

        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x80);
        SDL_RenderDrawRect(g_rd, &hoveredTileRect);

        g_fnt->RenderString("Select tile...", 10, 10);
    }

    if (navbar->focused) {
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
        SDL_RenderFillRect(g_rd, NULL);
    }

    wxsManager.renderAll();
}

void TilemapPreviewScreen::tick()
{
    canvas.lockToScreenBounds();

    tileSelectOffset = XY{
        iclamp(-caller->canvas.dimensions.x * tileSelectScale + 4, tileSelectOffset.x, g_windowW - 4),
        iclamp(-caller->canvas.dimensions.y * tileSelectScale + 4, tileSelectOffset.y, g_windowH - 4)
    };

    if (mouseLeftingTilemap) {
        if (caller->tileDimensions.x != 0 && caller->tileDimensions.y != 0) {
            if (hoveredTilePosition.x >= 0 && hoveredTilePosition.y >= 0 && hoveredTilePosition.x < tilemapDimensions.x && hoveredTilePosition.y < tilemapDimensions.y) {
                activeTilemap[hoveredTilePosition.y][hoveredTilePosition.x] = pickedTile;
            }
        }
    }

    if (layerPicker != NULL) {
        layerPicker->position.x = g_windowW - 260;
    }
}

void TilemapPreviewScreen::takeInput(SDL_Event evt)
{
    DrawableManager::processHoverEventInMultiple({ wxsManager }, evt);

    if (evt.type == SDL_QUIT) {
        g_closeScreen(this);
        return;
    }

    LALT_TO_SUMMON_NAVBAR;

    if (!DrawableManager::processInputEventInMultiple({wxsManager}, evt)) {
        switch (evt.type) {
        case SDL_MOUSEBUTTONDOWN:
            if (evt.button.button == SDL_BUTTON_MIDDLE) {
                scrollingTilemap = true;
            }
            else if (evt.button.button == SDL_BUTTON_LEFT) {
                if (tileSelectOpen) {
                    if (tileSelectHoveredTile.x >= 0 && tileSelectHoveredTile.y >= 0) {
                        pickedTile = tileSelectHoveredTile;
                        tileSelectOpen = false;
                        tileSelectTimer.start();
                    }
                }
                else {
                    mouseLeftingTilemap = true;
                }
            }
            else if (evt.button.button == SDL_BUTTON_RIGHT) {
                XY rel = canvas.screenPointToCanvasPoint({ evt.button.x, evt.button.y });
                if (canvas.pointInCanvasBounds(rel)) {
                    XY tile = canvas.getTilePosAt({ evt.button.x, evt.button.y }, caller->getPaddedTileDimensions());
                    pickedTile = activeTilemap[tile.y][tile.x];
                }
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (evt.button.button == SDL_BUTTON_MIDDLE) {
                scrollingTilemap = false;
            }
            else if (evt.button.button == SDL_BUTTON_LEFT) {
                mouseLeftingTilemap = false;
            }
            break;
        case SDL_MOUSEMOTION:
            if (tileSelectOpen) {
                if (scrollingTilemap) {
                    tileSelectOffset = xyAdd(tileSelectOffset, XY{ evt.motion.xrel, evt.motion.yrel });
                }

                XY pos = xySubtract(XY{ evt.button.x, evt.button.y }, tileSelectOffset);

                pos.x /= caller->tileDimensions.x * tileSelectScale;
                pos.y /= caller->tileDimensions.y * tileSelectScale;
                tileSelectHoveredTile = pos;
            }
            else {
                if (scrollingTilemap) {
                    canvas.panCanvas( XY{ evt.motion.xrel, evt.motion.yrel });
                }

                if (caller->tileDimensions.x != 0 && caller->tileDimensions.y != 0) {
                    XY callerTileSize = caller->getPaddedTileDimensions();
                    hoveredTilePosition = canvas.getTilePosAt(XY{ evt.motion.x, evt.motion.y }, callerTileSize);
                }
            }
            break;
        case SDL_MOUSEWHEEL:
            if (tileSelectOpen) {
                tileSelectScale += evt.wheel.y;
                tileSelectScale = ixmax(1, tileSelectScale);
            }
            else {
                canvas.zoom(evt.wheel.y);
            }
            break;
        case SDL_KEYDOWN:
            if (evt.key.keysym.sym == SDLK_TAB) {
                tileSelectOpen = !tileSelectOpen;
                tileSelectTimer.start();
            }
            else if (evt.key.keysym.sym == SDLK_LALT) {
                wxsManager.forceFocusOn(navbar);
            }
            break;
        }
    }
}

BaseScreen* TilemapPreviewScreen::isSubscreenOf()
{
    return caller;
}

void TilemapPreviewScreen::eventFileSaved(int evt_id, PlatformNativePathString name, int exporterIndex)
{
    if (evt_id == EVENT_TILEMAP_RENDERALLLTOIMAGE
        || evt_id == EVENT_TILEMAP_RENDERALLLTOIMAGES
        || evt_id == EVENT_TILEMAP_RENDERCURRENTLTOIMAGE) {
        doRenderMap(name, evt_id, exporterIndex);
    }
    else if (evt_id == EVENT_TILEMAP_SAVELAYOUT) {
        if (exporterIndex == 2) {
            //pxm
            FILE* file = platformOpenFile(name, PlatformFileModeWB);
            if (file == NULL) {
                g_addNotification(ErrorNotification("Error saving file", "Could not open file for writing."));
                return;
            }
            else {
                bool success = false;
                if (tilemap.size() == 1) {
                    u8 header[4] = { 'P', 'X', 'M', 0x10 };
                    fwrite(&header, 1, 4, file);
                    u16 w = tilemapDimensions.x;
                    u16 h = tilemapDimensions.y;
                    fwrite(&w, 2, 1, file);
                    fwrite(&h, 2, 1, file);

                    for (int y = 0; y < h; y++) {
                        for (int x = 0; x < w; x++) {
                            XY tile = tilemap[0][y][x];
                            u8 byte = (tile.x < 0 || tile.y < 0 || tile.x >= 16) ? 0 : (u8)(tile.y * 16 + tile.x);
                            fwrite(&byte, 1, 1, file);
                        }
                    }
                    success = true;
                }
                else if (tilemap.size() == 4) {
                    u8 header[4] = { 'P', 'X', 'M', 0x21 };
                    fwrite(&header, 1, 4, file);
                    u16 w = tilemapDimensions.x;
                    u16 h = tilemapDimensions.y;
                    fwrite(&w, 2, 1, file);
                    fwrite(&h, 2, 1, file);

                    for (XY**& layer : tilemap) {
                        for (int y = 0; y < h; y++) {
                            for (int x = 0; x < w; x++) {
                                XY tile = layer[y][x];
                                u16 byte = (tile.x < 0 || tile.y < 0 || tile.x >= 16) ? 0 : (u16)(tile.y * 16 + tile.x);
                                fwrite(&byte, 2, 1, file);
                            }
                        }
                    }
                    success = true;
                }
                else {
                    g_addNotification(ErrorNotification("Error", "PXM requires either 1 or 4 layers."));
                }

                fclose(file);
                if (success) {
                    g_addNotification(SuccessNotification("File saved", "Save successful!"));
                }
            }
        }
        else {
            FILE* file = platformOpenFile(name, PlatformFileModeWB);
            if (file == NULL) {
                g_addNotification(ErrorNotification("Error saving file", "Could not open file for writing."));
                return;
            }
            else {
                uint8_t version = 2;
                fwrite(&version, 1, 1, file);
                fwrite(&tilemapDimensions.x, 4, 1, file);
                fwrite(&tilemapDimensions.y, 4, 1, file);
                int layerCount = tilemap.size();
                fwrite(&layerCount, 4, 1, file);
                for (XY**& l : tilemap) {
                    for (int y = 0; y < tilemapDimensions.y; y++) {
                        for (int x = 0; x < tilemapDimensions.x; x++) {
                            XY td = l[y][x];
                            fwrite(&td.x, 4, 1, file);
                            fwrite(&td.y, 4, 1, file);
                        }
                    }
                }
                fclose(file);
                g_addNotification(SuccessNotification("File saved", "Save successful!"));
            }
        }
    }
}

void TilemapPreviewScreen::eventFileOpen(int evt_id, PlatformNativePathString name, int importerIndex)
{
    if (evt_id == EVENT_TILEMAP_LOADLAYOUT) {
        //
        //if (name.find(utf8StringToWstring(".lmu")) == name.size() - 4) {
        if (importerIndex == 2) {
            //pxm
            FILE* file = platformOpenFile(name, PlatformFileModeRB);
            if (file == NULL) {
                g_addNotification(ErrorNotification("Error loading file", "Could not open file for writing."));
                return;
            }
            else {
                u8 header[3];
                fread(&header, 1, 3, file);
                if (header[0] == 'P' && header[1] == 'X' && header[2] == 'M') {
                    u8 zerox10Byte;
                    fread(&zerox10Byte, 1, 1, file);
                    u16 mapLength, mapHeight;
                    fread(&mapLength, 2, 1, file);
                    fread(&mapHeight, 2, 1, file);

                    if (zerox10Byte == 0x10) {
                        printf("[PXE] mapLength: %i ; mapHeight: %i\n", mapLength, mapHeight);
                        resizeTilemap(mapLength, mapHeight);
                        freeAllLayers();
                        XY** newTilemap = newLayer();
                        for (u64 tile = 0; tile < mapLength * mapHeight; tile++) {
                            u8 byte;
                            fread(&byte, 1, 1, file);
                            newTilemap[tile / mapLength][tile % mapLength] = XY{ byte % 16, byte / 16 };
                        }
                    }
                    else if (zerox10Byte == 0x21) {
                        printf("[PXE] multi layer, mapLength: %i ; mapHeight: %i\n", mapLength, mapHeight);
                        resizeTilemap(mapLength, mapHeight);
                        freeAllLayers();
                        for (int lidx = 0; lidx < 4; lidx++) {
                            XY** newTilemap = newLayer();
                            for (u64 tile = 0; tile < mapLength * mapHeight; tile++) {
                                u16 byte;
                                fread(&byte, 2, 1, file);
                                newTilemap[tile / mapLength][tile % mapLength] = XY{ byte % 16, byte / 16 };
                            }
                        }
                    }
                    activeTilemap = tilemap[0];
                }
                else {
                    g_addNotification(ErrorNotification("Error loading file", "PXM header invalid."));
                }
            }
            fclose(file);
            layerPicker->updateLayers();
        }
        else {
            FILE* file = platformOpenFile(name, PlatformFileModeRB);
            if (file == NULL) {
                g_addNotification(ErrorNotification("Error loading file", "Could not open file for writing."));
                return;
            }
            else {
                uint8_t version;
                fread(&version, 1, 1, file);
                switch (version) {
                case 1:
                    XY dims;
                    fread(&dims.x, 4, 1, file);
                    fread(&dims.y, 4, 1, file);
                    resizeTilemap(dims.x, dims.y);
                    for (int y = 0; y < tilemapDimensions.y; y++) {
                        for (int x = 0; x < tilemapDimensions.x; x++) {
                            XY td;
                            fread(&td.x, 4, 1, file);
                            fread(&td.y, 4, 1, file);
                            activeTilemap[y][x] = td;
                        }
                    }
                    break;
                case 2:
                    freeAllLayers();
                    fread(&tilemapDimensions.x, 4, 1, file);
                    fread(&tilemapDimensions.y, 4, 1, file);
                    int layerCount;
                    fread(&layerCount, 4, 1, file);
                    for (int l = 0; l < layerCount; l++) {
                        XY** newTilemap = newLayer();
                        for (int y = 0; y < tilemapDimensions.y; y++) {
                            for (int x = 0; x < tilemapDimensions.x; x++) {
                                XY td;
                                fread(&td.x, 4, 1, file);
                                fread(&td.y, 4, 1, file);
                                newTilemap[y][x] = td;
                            }
                        }
                    }
                    activeTilemap = tilemap[0];
                    break;
                default:
                    g_addNotification(ErrorNotification("Error loading file", "File version not supported"));
                    break;
                }
                fclose(file);
                layerPicker->updateLayers();
            }
        }
    }
}

void TilemapPreviewScreen::eventPopupClosed(int evt_id, BasePopup* p)
{
    if (evt_id == EVENT_TILEMAP_RESIZE) {
        resizeTilemap(std::stoi(((PopupTileGeneric*)p)->tboxX->text), std::stoi(((PopupTileGeneric*)p)->tboxY->text));
    }
}

void TilemapPreviewScreen::resizeTilemap(int w, int h)
{
    if (tilemap.size() > 0) {
        for (int l = 0; l < tilemap.size(); l++) {
            XY**& tilemapLayer = tilemap.at(l);

            XY** newTilemap = (XY**)tracked_malloc(h * sizeof(XY*), "Tilemap");
            for (int y = 0; y < h; y++) {
                newTilemap[y] = (XY*)tracked_malloc(w * sizeof(XY), "Tilemap");
                for (int x = 0; x < w; x++) {
                    newTilemap[y][x] = XY{ -1,-1 };
                }
            }

            if (tilemapLayer != NULL) {
                for (int y = 0; y < tilemapDimensions.y; y++) {
                    if (y < h) {
                        memcpy(newTilemap[y], tilemapLayer[y], sizeof(XY) * ixmin(w, tilemapDimensions.x));
                    }
                    tracked_free(tilemapLayer[y]);
                }
                tracked_free(tilemapLayer);
            }

            tilemap[l] = newTilemap;
        }
    }
    else {
        XY** newTilemap = (XY**)tracked_malloc(h * sizeof(XY*), "Tilemap");
        for (int y = 0; y < h; y++) {
            newTilemap[y] = (XY*)tracked_malloc(w * sizeof(XY), "Tilemap");
            for (int x = 0; x < w; x++) {
                newTilemap[y][x] = XY{ -1,-1 };
            }
        }
        tilemap.push_back(newTilemap);
    }
    
    tilemapDimensions = { w,h };
    activeTilemap = tilemap[0];
}

void TilemapPreviewScreen::drawBackground()
{
    uint32_t colorBG1 = 0xFF000000;//| (sdlcolorToUint32(backgroundColor) == 0xFF000000 ? 0x000000 : 0xDFDFDF);
    uint32_t colorBG2 = 0xFF000000 | 0x202020;//| (sdlcolorToUint32(backgroundColor) == 0xFF000000 ? 0x202020 : 0x808080);
    renderGradient({ 0,0, g_windowW, g_windowH }, colorBG1, colorBG1, colorBG1, colorBG2);

    if (g_config.animatedBackground) {
        uint64_t now = g_config.animatedBackground >= 3 ? 0 : SDL_GetTicks64();
        uint64_t progress = now % 120000;
        for (int y = -(1.0 - progress / 120000.0) * g_windowH; y < g_windowH; y += 50) {
            if (y >= 0) {
                SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x13);
                SDL_RenderDrawLine(g_rd, 0, y, g_windowW, y - 50);
            }
        }

        for (int x = -(1.0 - (now % 100000) / 100000.0) * g_windowW; x < g_windowW; x += 30) {
            if (x >= 0) {
                SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x13);
                SDL_RenderDrawLine(g_rd, x, 0, x - 30, g_windowH);
            }
        }
    }
}

void TilemapPreviewScreen::recenterTilemap()
{
    canvas.recenter();
}

void TilemapPreviewScreen::recenterTilePicker()
{
    tileSelectOffset = XY{
        (g_windowW / 2) - (caller->canvas.dimensions.x * tileSelectScale) / 2,
        (g_windowH / 2) - (caller->canvas.dimensions.y * tileSelectScale) / 2
    };
}

int TilemapPreviewScreen::activeLayerIndex()
{
    if (activeTilemap == NULL) {
        return -1;
    }
    for (int i = 0; i < tilemap.size(); i++) {
        if (tilemap[i] == activeTilemap) {
            return i;
        }
    }
    return -1;
}

void TilemapPreviewScreen::switchActiveLayer(int layerIndex)
{
    if (layerIndex >= 0 && layerIndex < tilemap.size()) {
        activeTilemap = tilemap[layerIndex];
        layerSelectTimer.start();
    }
}

void TilemapPreviewScreen::freeAllLayers()
{
    for (int l = 0; l < tilemap.size(); l++) {
        XY**& tilemapLayer = tilemap.at(l);
        if (tilemapLayer != NULL) {
            for (int y = 0; y < tilemapDimensions.y; y++) {
                tracked_free(tilemapLayer[y]);
            }
            tracked_free(tilemapLayer);
        }
    }
    tilemap.clear();
    activeTilemap = NULL;
}

XY** TilemapPreviewScreen::newLayer()
{
    XY** newTilemap = (XY**)tracked_malloc(tilemapDimensions.y * sizeof(XY*), "Tilemap");
    for (int y = 0; y < tilemapDimensions.y; y++) {
        newTilemap[y] = (XY*)tracked_malloc(tilemapDimensions.x * sizeof(XY), "Tilemap");
        for (int x = 0; x < tilemapDimensions.x; x++) {
            newTilemap[y][x] = XY{ -1,-1 };
        }
    }
    tilemap.push_back(newTilemap);
    switchActiveLayer(tilemap.size() - 1);
    return newTilemap;
}

void TilemapPreviewScreen::deleteLayer(int index)
{
    if (tilemap.size() > 1) {
        XY** tilemapLayer = tilemap.at(index);
        for (int y = 0; y < tilemapDimensions.y; y++) {
            tracked_free(tilemapLayer[y]);
        }
        tracked_free(tilemapLayer);
        tilemap.erase(tilemap.begin() + index);
        if (activeTilemap == tilemapLayer) {
            activeTilemap = tilemap[index < tilemap.size() ? index : tilemap.size()-1];
        }
    }
}

void TilemapPreviewScreen::moveLayerUp(int index)
{
    if (index != tilemap.size() - 1) {
        XY** tmp = tilemap[index];
        tilemap[index] = tilemap[index + 1];
        tilemap[index + 1] = tmp;
    }
}

void TilemapPreviewScreen::moveLayerDown(int index)
{
    if (index != 0) {
        XY** tmp = tilemap[index];
        tilemap[index] = tilemap[index - 1];
        tilemap[index - 1] = tmp;
    }
}

void TilemapPreviewScreen::mergeLayerDown(int index)
{
    if (index != 0) {
        XY**& tilemapLayer = tilemap.at(index);
        XY**& tilemapLayerBelow = tilemap.at(index - 1);
        for (int y = 0; y < tilemapDimensions.y; y++) {
            for (int x = 0; x < tilemapDimensions.x; x++) {
                if (tilemapLayer[y][x].x >= 0 && tilemapLayer[y][x].y >= 0) {
                    tilemapLayerBelow[y][x] = tilemapLayer[y][x];
                }
            }
        }
        deleteLayer(index);
    }
}

void TilemapPreviewScreen::duplicateLayer(int index)
{
    XY**& tilemapLayer = tilemap.at(index);
    XY** newTilemap = (XY**)tracked_malloc(tilemapDimensions.y * sizeof(XY*), "Tilemap");
    for (int y = 0; y < tilemapDimensions.y; y++) {
        newTilemap[y] = (XY*)tracked_malloc(tilemapDimensions.x * sizeof(XY), "Tilemap");
        for (int x = 0; x < tilemapDimensions.x; x++) {
            newTilemap[y][x] = tilemapLayer[y][x];
        }
    }
    tilemap.insert(tilemap.begin() + index, newTilemap);
}

void TilemapPreviewScreen::promptRenderMap(int type)
{
    if (type == EVENT_TILEMAP_RENDERALLLTOIMAGE
        || type == EVENT_TILEMAP_RENDERALLLTOIMAGES
        || type == EVENT_TILEMAP_RENDERCURRENTLTOIMAGE) {
        
        std::vector<std::pair<std::string, std::string>> formats;
        for (auto f : g_fileExporters) {
            formats.push_back({ f->extension(), f->name() });
        }
        platformTrySaveOtherFile(this, formats, "render image", type);
    }
}

void TilemapPreviewScreen::doRenderMap(PlatformNativePathString path, int type, int exporterIndex)
{
    FileExporter* exporter = g_fileExporters[exporterIndex - 1];
    XY tileSize = caller->getPaddedTileDimensions();
    bool success = false;
    if (type == EVENT_TILEMAP_RENDERALLLTOIMAGE) {

        std::vector<Layer*> layers;
        int x = 0;
        for (XY**& tileLayer : tilemap) {
            Layer* ll = renderLayer(tileLayer);
            ll->name += std::format(" {}", x++);
            layers.push_back(ll);
        }
        MainEditor* newSession = new MainEditor(layers);
        newSession->tileDimensions = tileSize;

        if (exporter->exportsWholeSession()) {
            success = exporter->exportData(path, newSession);
            delete newSession;
        }
        else {
            Layer* l = newSession->flattenImage();
            success = exporter->exportData(path, l);
            delete l;
        }
        delete newSession;
    }
    else if (type == EVENT_TILEMAP_RENDERALLLTOIMAGES) {
        int exports = 0;
        int x = 0;
        for (XY**& tileLayer : tilemap) {
            Layer* ll = renderLayer(tileLayer);
            ll->name += std::format(" {}", x+1);
            bool ss = false;
            PlatformNativePathString filename = path + convertStringOnWin32(std::format("-{}{}", x++, exporter->extension()));
            if (exporter->exportsWholeSession()) {
                MainEditor* ssn = new MainEditor({ ll });
                ssn->tileDimensions = tileSize;
                ss = exporter->exportData(filename, ssn);
                delete ssn;
            }
            else {
                ss = exporter->exportData(filename, ll);
                delete ll;
            }
            exports += ss ? 1 : 0;
        }
        success = exports == tilemap.size();
    }
    else if (type == EVENT_TILEMAP_RENDERCURRENTLTOIMAGE) {
        Layer* ll = renderLayer(activeTilemap);
        if (exporter->exportsWholeSession()) {
            MainEditor* ssn = new MainEditor({ ll });
            success = exporter->exportData(path, ssn);
            delete ssn;
        }
        else {
            success = exporter->exportData(path, ll);
            delete ll;
        }
    }
    else {
        g_addNotification(ErrorNotification("Error", "Not implemented"));
    }

    g_addNotification(
        success ? SuccessNotification("Success", "Rendered map to image")
        : ErrorNotification("Error", "Error rendering map to image")
    );
}

Layer* TilemapPreviewScreen::renderLayer(XY** layer)
{
    XY tileSize = caller->getPaddedTileDimensions();
    Layer* l = new Layer(tileSize.x * tilemapDimensions.x, tileSize.y * tilemapDimensions.y);
    l->name = "Tilemap layer";
    for (int y = 0; y < tilemapDimensions.y; y++) {
        for (int x = 0; x < tilemapDimensions.x; x++) {
            XY currentTile = layer[y][x];
            if (currentTile.x >= 0 && currentTile.y >= 0) {
                SDL_Rect clip = caller->getPaddedTilePosAndDimensions(currentTile);
                int lIndex = 0;
                for (Layer*& ll : caller->layers) {
                    l->blit(ll, XY{ x * tileSize.x, y * tileSize.y }, clip, lIndex++ == 0);
                }
            }
        }
    }
    return l;
}
