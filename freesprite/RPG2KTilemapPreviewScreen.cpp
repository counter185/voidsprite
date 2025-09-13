#include <fstream>

#include <lcf/lmu/reader.h>
#include <lcf/writer_lcf.h>

#include "RPG2KTilemapPreviewScreen.h"
#include "ScreenWideNavBar.h"
#include "maineditor.h"
#include "Notification.h"
#include "FontRenderer.h"
#include "FileIO.h"
#include "LayerPalettized.h"
#include "PanelRPG2KTilemapPreview.h"
#include "PopupGlobalConfig.h"
#include "PanelReference.h"
#include "CollapsableDraggablePanel.h"

RPG2KTilemapPreviewScreen::RPG2KTilemapPreviewScreen(MainEditor* parent)
{
    caller = parent;

    PanelRPG2KTilemapPreview* layersPanel = new PanelRPG2KTilemapPreview(this);
    layersPanel->position = { 20, 80 };
    wxsManager.addDrawable(layersPanel);

    navbar = new ScreenWideNavBar(this,
        {
            {
                SDL_SCANCODE_F,
                {
                    "File",
                    {},
                    {
                        {SDL_SCANCODE_O, { "Load layout from file",
                                [this]() {
                                    platformTryLoadOtherFile(this, {{".lmu", "RPGM2000/2003 Map"}}, "Load tile layout", EVENT_OTHERFILE_OPENFILE);
                                }
                            }
                        },
                        {SDL_SCANCODE_E, { "Render to image",
                                [this]() {
                                    std::vector<std::pair<std::string, std::string>> formats;
                                    for (auto f : g_fileExporters) {
                                        formats.push_back({ f->extension(), f->name()});
                                    }
                                    platformTrySaveOtherFile(this, formats, "render LMU map to image", EVENT_LMUPREVIEW_RENDERMAP);
                                }
                            }
                        },
                        {SDL_SCANCODE_P, { TL("vsp.maineditor.preference"),
                            [this]() {
                                g_addPopup(new PopupGlobalConfig());
                            }
                        }
                    },
                        /*{SDL_SCANCODE_S, {"Save layout to file",
                                [](TilemapPreviewScreen* screen) {
                                    platformTrySaveOtherFile(screen, { {".voidtile", "voidtile layout"} }, "Save tile layout", EVENT_OTHERFILE_SAVEFILE);
                                }
                            }
                        },*/
                    },
                    g_iconNavbarTabFile
                }
            },
            {
                SDL_SCANCODE_V,
                {
                    "View",
                    {},
                    {
                        {SDL_SCANCODE_E, { "Toggle Event display",
                                [this]() {
                                    this->eventViewMode = (LMUEventViewMode)(((int)this->eventViewMode + 1) % 4);
                                    switch (this->eventViewMode) {
                                        case LMUEVENTS_HIDE_ALL:
                                            g_addNotification(Notification("All Events hidden","", 1500, NULL, COLOR_INFO));
                                            break;
                                        case LMUEVENTS_SHOW_INGAME:
                                            g_addNotification(Notification("Events shown as ingame", "", 1500, NULL, COLOR_INFO));
                                            break;
                                        case LMUEVENTS_SHOW_RECTS:
                                            g_addNotification(Notification("Events shown as in editor", "", 1500, NULL, COLOR_INFO));
                                            break;
                                        case LMUEVENTS_SHOW_INGAME_AND_RECTS:
                                            g_addNotification(Notification("Events shown as ingame and in editor", "", 1500, NULL, COLOR_INFO));
                                            break;
                                    }
                                }
                            }
                        },
                        {SDL_SCANCODE_R, { "Recenter canvas",
                                [this]() {
                                    this->RecenterCanvas();
                                }
                            }
                        }
                    },
                    g_iconNavbarTabView 
                }
            }
        }, { SDL_SCANCODE_F, SDL_SCANCODE_V });
    wxsManager.addDrawable(navbar);

    lowerLayerData = new uint16_t[dimensions.x * dimensions.y];
    upperLayerData = new uint16_t[dimensions.x * dimensions.y];
    memset(lowerLayerData, 0, dimensions.x * dimensions.y * sizeof(uint16_t));
    memset(upperLayerData, 0, dimensions.x * dimensions.y * sizeof(uint16_t));

    RecenterCanvas();

    //resizeTilemap(32, 32);
}

RPG2KTilemapPreviewScreen::~RPG2KTilemapPreviewScreen()
{
    if (lowerLayerData != NULL) {
        delete[] lowerLayerData;
    }
    if (upperLayerData != NULL) {
        delete[] upperLayerData;
    }
    if (callerCanvas != NULL) {
        delete callerCanvas;
    }
    for (auto& tex : texturesLoaded) {
        delete tex.second;
    }
}

void RPG2KTilemapPreviewScreen::render()
{
    canvas.dimensions = { dimensions.x * 16, dimensions.y * 16 };

    TilemapPreviewScreen::drawBackground();
    RenderWholeMap(canvas.currentDrawPoint, canvas.scale, rdLowerLayer, rdUpperLayer, rdEventLayer);

    SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, gridOpacity);
    canvas.drawTileGrid({ 16,16 });

    canvas.drawCanvasOutline(5, { 255,255,255,0x90 });

    wxsManager.renderAll();

    //bottom bar
    SDL_Rect r = { 0, g_windowH - 30, g_windowW, 30 };
    SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0xb0);
    SDL_RenderFillRect(g_rd, &r);

    g_fnt->RenderString(frmt("{}x{} tiles", dimensions.x, dimensions.y), 2, g_windowH - 28, SDL_Color{ 255,255,255,0xa0 });
}

void RPG2KTilemapPreviewScreen::tick()
{
    if (!xyEqual(caller->canvas.dimensions, {480, 256})) {
        g_closeScreen(this);
        return;
    }

    canvas.lockToScreenBounds();
}

void RPG2KTilemapPreviewScreen::takeInput(SDL_Event evt)
{
    DrawableManager::processHoverEventInMultiple({ wxsManager }, evt);

    if (evt.type == SDL_QUIT) {
        g_closeScreen(this);
        return;
    }

    LALT_TO_SUMMON_NAVBAR;

    if (evt.type == SDL_DROPFILE) {
        PlatformNativePathString p = convertStringOnWin32(evt.drop.data);
        if (stringEndsWithIgnoreCase(p, convertStringOnWin32(".lmu"))) {
            LoadLMU(p);
        }
        return;
    }

    if (!DrawableManager::processInputEventInMultiple({wxsManager}, evt)) {
        switch (evt.type) {
        case SDL_MOUSEWHEEL:
            canvas.zoom(evt.wheel.y);
            break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            if (evt.button.button == SDL_BUTTON_MIDDLE) {
                scrollingTilemap = evt.button.down;
            }
            break;
        case SDL_MOUSEMOTION:
            if (scrollingTilemap) {
                canvas.panCanvas(XY{ (int)(evt.motion.xrel), (int)(evt.motion.yrel) });
            }
            break;
        }
    }
}

BaseScreen* RPG2KTilemapPreviewScreen::isSubscreenOf()
{
    return caller;
}

void RPG2KTilemapPreviewScreen::eventFileOpen(int evt_id, PlatformNativePathString path, int importer_index)
{
    if (evt_id == EVENT_OTHERFILE_OPENFILE && importer_index == 1) {
        LoadLMU(path);
    }
}

void RPG2KTilemapPreviewScreen::eventFileSaved(int evt_id, PlatformNativePathString path, int exporter_index)
{
    Layer* l = RenderWholeMapToTexture();
    if (l != NULL) {
        l->name = "Rendered LMU Map";

        FileExporter* exporter = g_fileExporters[exporter_index - 1];
        bool result = false;
        if (exporter->exportsWholeSession()) {
            MainEditor* session = new MainEditor(l);
            session->tileDimensions = { 16,16 };
            result = exporter->exportData(path, session);
            delete session;
        }
        else {
            result = exporter->exportData(path, l);
            delete l;
        }

        if (result) {
            if (g_config.openSavedPath) {
                platformOpenFileLocation(path);
            }
            g_addNotification(SuccessNotification("Success", "Map rendered to image"));
        }
        else {
            g_addNotification(ErrorNotification("Error", "Failed to save rendered image"));
        }
        
    }
    else {
        g_addNotification(ErrorNotification("Error", "Failed to render map to image"));
    }
}

uint16_t RPG2KTilemapPreviewScreen::lowerLayerTileAt(XY position)
{
    if (position.x > 0 && position.x < dimensions.x && position.y > 0 && position.y < dimensions.y) {
        return lowerLayerData[position.y * dimensions.x + position.x];
    }
    else {
        return -1;
    }
}

bool RPG2KTilemapPreviewScreen::isDeepWaterTileAt(XY position)
{
    uint16_t tile = lowerLayerTileAt(position);
    if (tile < 0x0BB8) {
        int watertile = tile % 50;
        int watertype = tile / 50 / 20;
        return watertype == 2;
    }
    else {
        return false;
    }
}

void RPG2KTilemapPreviewScreen::RecenterCanvas()
{
    canvas.recenter();
}

void RPG2KTilemapPreviewScreen::PrerenderCanvas()
{
    if (callerCanvas != NULL) {
        delete callerCanvas;
    }
    callerCanvas = new ReldTex([this](SDL_Renderer* rd) {
        SDL_Texture* tex = tracked_createTexture(rd, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 480, 256);
        SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_NONE);
        g_pushRenderTarget(tex);
        SDL_SetRenderDrawColor(rd, 0, 0, 0, 0);
        SDL_RenderClear(rd);
        for (Layer*& l : caller->layers) {
            if (!l->hidden) {
                l->render({ 0,0, 480, 256 }, 255);
            }
        }
        g_popRenderTarget();
        SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
        return tex;
    });
}

void RPG2KTilemapPreviewScreen::RenderWaterTile(uint8_t connection, uint16_t watertileIndex, XY position, SDL_Rect dst, SDL_Texture* tex, int animState)
{
    int waterAnimLength = 500;
    if (animState == -1) {
        int animState = SDL_GetTicks64() % (waterAnimLength * 4) / waterAnimLength;
        if (animState == 3) {
            animState = 1;
        }
    }

    bool dwUp = isDeepWaterTileAt({ position.x, position.y - 1 });
    bool dwRight = isDeepWaterTileAt({ position.x + 1, position.y });
    bool dwLeft = isDeepWaterTileAt({ position.x - 1, position.y });
    bool dwDown = isDeepWaterTileAt({ position.x, position.y + 1 });
    //dw corners
    bool dwUL = isDeepWaterTileAt({ position.x - 1, position.y - 1 });
    bool dwUR = isDeepWaterTileAt({ position.x + 1, position.y - 1 });
    bool dwDL = isDeepWaterTileAt({ position.x - 1, position.y + 1 });
    bool dwDR = isDeepWaterTileAt({ position.x + 1, position.y + 1 });

    XY waterTileOrigin = { 16 * animState, 64 };
    //monkey code time
    if (watertileIndex == 2 /* &&
        (
            (dwUp && dwUL && dwLeft)
            || (dwLeft && dwDL && dwDown)
            || (dwDown && dwDR && dwRight)
            || (dwRight && dwUR && dwUp)
        )*/
       ) 
    {
        waterTileOrigin.y += 48;
    }

    if (watertileIndex <= 2) {
        XY origin = { 16 * 3 * (watertileIndex%2) + 16 * animState, 0 };
        if (connection == 0x00) {
            //x00 - full tile
            SDL_Rect src = { waterTileOrigin.x, waterTileOrigin.y, 16, 16 };
            SDL_RenderCopy(g_rd, tex, &src, &dst);
        }
        else if (connection <= 0b1111) {
            //corners
            SDL_Rect src = { waterTileOrigin.x, waterTileOrigin.y, 16, 16 };
            SDL_RenderCopy(g_rd, tex, &src, &dst);

            if (connection & 0b0001) {
                //top left
                SDL_Rect src2 = { origin.x, origin.y + 48, 8, 8 };
                SDL_Rect dst2 = { dst.x, dst.y, dst.w / 2, dst.h / 2 };
                SDL_RenderCopy(g_rd, tex, &src2, &dst2);
            }
            if (connection & 0b0010) {
                //top right
                SDL_Rect src2 = { origin.x + 8, origin.y + 48, 8, 8 };
                SDL_Rect dst2 = { dst.x + dst.w / 2, dst.y, dst.w / 2, dst.h / 2 };
                SDL_RenderCopy(g_rd, tex, &src2, &dst2);
            }
            if (connection & 0b0100) {
                //bottom right
                SDL_Rect src2 = { origin.x + 8, origin.y + 48 + 8, 8, 8 };
                SDL_Rect dst2 = { dst.x + dst.w / 2, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
                SDL_RenderCopy(g_rd, tex, &src2, &dst2);
            }
            if (connection & 0b1000) {
                //bottom left
                SDL_Rect src2 = { origin.x, origin.y + 48 + 8, 8, 8 };
                SDL_Rect dst2 = { dst.x, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
                SDL_RenderCopy(g_rd, tex, &src2, &dst2);
            }
        }
        else if (connection <= 0b10011) {
            //left side
            SDL_Rect src = { waterTileOrigin.x, waterTileOrigin.y, 16, 16 };
            SDL_RenderCopy(g_rd, tex, &src, &dst);

            SDL_Rect src2 = { origin.x, origin.y + 16, 8, 16 };
            SDL_Rect dst2 = { dst.x, dst.y, dst.w / 2, dst.h };
            SDL_RenderCopy(g_rd, tex, &src2, &dst2);

            if (connection & 0b01) {
                //top right corner
                SDL_Rect src2 = { origin.x + 8, origin.y + 48, 8, 8 };
                SDL_Rect dst2 = { dst.x + dst.w / 2, dst.y, dst.w / 2, dst.h / 2 };
                SDL_RenderCopy(g_rd, tex, &src2, &dst2);
            }
            if (connection & 0b10) {
                //bottom right corner
                SDL_Rect src2 = { origin.x + 8, origin.y + 48 + 8, 8, 8 };
                SDL_Rect dst2 = { dst.x + dst.w / 2, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
                SDL_RenderCopy(g_rd, tex, &src2, &dst2);
            }
        }
        else if (connection <= 0b10111) {
            //border top
            SDL_Rect src = { waterTileOrigin.x, waterTileOrigin.y, 16, 16 };
            SDL_RenderCopy(g_rd, tex, &src, &dst);

            SDL_Rect src2 = { origin.x, origin.y + 32, 16, 8 };
            SDL_Rect dst2 = { dst.x, dst.y, dst.w, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &src2, &dst2);

            if (connection & 0b01) {
                //bottom right corner
                SDL_Rect src2 = { origin.x + 8, origin.y + 48 + 8, 8, 8 };
                SDL_Rect dst2 = { dst.x + dst.w / 2, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
                SDL_RenderCopy(g_rd, tex, &src2, &dst2);
            }
            if (connection & 0b10) {
                //bottom left corner
                SDL_Rect src2 = { origin.x, origin.y + 48 + 8, 8, 8 };
                SDL_Rect dst2 = { dst.x, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
                SDL_RenderCopy(g_rd, tex, &src2, &dst2);
            }
        }
        else if (connection <= 0b11011) {
            //border right
            SDL_Rect src = { waterTileOrigin.x, waterTileOrigin.y, 16, 16 };
            SDL_RenderCopy(g_rd, tex, &src, &dst);
            SDL_Rect src2 = { origin.x + 8, origin.y + 16, 8, 16 };
            SDL_Rect dst2 = { dst.x + dst.w / 2, dst.y, dst.w / 2, dst.h };
            SDL_RenderCopy(g_rd, tex, &src2, &dst2);

            if (connection & 0b01) {
                //bottom left corner
                SDL_Rect src2 = { origin.x, origin.y + 48 + 8, 8, 8 };
                SDL_Rect dst2 = { dst.x, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
                SDL_RenderCopy(g_rd, tex, &src2, &dst2);
            }
            if (connection & 0b10) {
                //top left corner
                SDL_Rect src2 = { origin.x, origin.y + 48, 8, 8 };
                SDL_Rect dst2 = { dst.x, dst.y, dst.w / 2, dst.h / 2 };
                SDL_RenderCopy(g_rd, tex, &src2, &dst2);
            }
        }
        else if (connection <= 0b11111) {
            //border bottom
            SDL_Rect src = { waterTileOrigin.x, waterTileOrigin.y, 16, 16 };
            SDL_RenderCopy(g_rd, tex, &src, &dst);
            SDL_Rect src2 = { origin.x, origin.y + 32 + 8, 16, 8 };
            SDL_Rect dst2 = { dst.x, dst.y + dst.h / 2, dst.w, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &src2, &dst2);

            if (connection & 0b01) {
                //top left corner
                SDL_Rect src2 = { origin.x, origin.y + 48, 8, 8 };
                SDL_Rect dst2 = { dst.x, dst.y, dst.w / 2, dst.h / 2 };
                SDL_RenderCopy(g_rd, tex, &src2, &dst2);
            }
            if (connection & 0b10) {
                //top right corner
                SDL_Rect src2 = { origin.x + 8, origin.y + 48, 8, 8 };
                SDL_Rect dst2 = { dst.x + dst.w / 2, dst.y, dst.w / 2, dst.h / 2 };
                SDL_RenderCopy(g_rd, tex, &src2, &dst2);
            }
        }
        else if (connection == 0b100000) {
            //x20 - vertical tile
            SDL_Rect src = { origin.x, origin.y + 16, 16, 16 };
            SDL_RenderCopy(g_rd, tex, &src, &dst);
        }
        else if (connection == 0b100001) {
            //x21 - horizontal tile
            SDL_Rect src = { origin.x, origin.y + 32, 16, 16 };
            SDL_RenderCopy(g_rd, tex, &src, &dst);
        }
        else if (connection <= 0b100011) {

            SDL_Rect src = { waterTileOrigin.x, waterTileOrigin.y, 16, 16 };
            SDL_RenderCopy(g_rd, tex, &src, &dst);

            //top left border tile
            SDL_Rect chunkUL = { origin.x, origin.y, 8, 8 };
            SDL_Rect dstUL = { dst.x, dst.y, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &chunkUL, &dstUL);
            SDL_Rect chunkUR = { origin.x + 8, origin.y + 32, 8, 8 };
            SDL_Rect dstUR = { dst.x + dst.w / 2, dst.y, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &chunkUR, &dstUR);
            SDL_Rect chunkDL = { origin.x, origin.y + 16, 8, 8 };
            SDL_Rect dstDL = { dst.x, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &chunkDL, &dstDL);

            if (connection & 0b1) {
                //bottom right corner
                SDL_Rect chunkDR = { origin.x + 8, origin.y + 48 + 8, 8, 8 };
                SDL_Rect dstDR = { dst.x + dst.w / 2, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
                SDL_RenderCopy(g_rd, tex, &chunkDR, &dstDR);
            }
        }
        else if (connection <= 0b100101) {
            SDL_Rect src = { waterTileOrigin.x, waterTileOrigin.y, 16, 16 };
            SDL_RenderCopy(g_rd, tex, &src, &dst);
            
            //top right border tile
            SDL_Rect chunkUR = { origin.x + 8, origin.y, 8, 8 };
            SDL_Rect dstUR = { dst.x + dst.w / 2, dst.y, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &chunkUR, &dstUR);
            SDL_Rect chunkDR = { origin.x + 8, origin.y + 16 + 8, 8, 8 };
            SDL_Rect dstDR = { dst.x + dst.w / 2, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &chunkDR, &dstDR);
            SDL_Rect chunkUL = { origin.x, origin.y + 32, 8, 8 };
            SDL_Rect dstUL = { dst.x, dst.y, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &chunkUL, &dstUL);

            if (connection & 0b1) {
                //bottom left corner
                SDL_Rect chunkDL = { origin.x, origin.y + 48 + 8, 8, 8 };
                SDL_Rect dstDL = { dst.x, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
                SDL_RenderCopy(g_rd, tex, &chunkDL, &dstDL);
            }
        }
        else if (connection <= 0b100111) {
            SDL_Rect src = { waterTileOrigin.x, waterTileOrigin.y, 16, 16 };
            SDL_RenderCopy(g_rd, tex, &src, &dst);

            //bottom right border tile
            SDL_Rect chunkDR = { origin.x + 8, origin.y + 8, 8, 8 };
            SDL_Rect dstDR = { dst.x + dst.w / 2, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &chunkDR, &dstDR);
            SDL_Rect chunkDL = { origin.x, origin.y + 32 + 8, 8, 8 };
            SDL_Rect dstDL = { dst.x, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &chunkDL, &dstDL);
            SDL_Rect chunkUR = { origin.x + 8, origin.y + 16, 8, 8 };
            SDL_Rect dstUR = { dst.x + dst.w / 2, dst.y, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &chunkUR, &dstUR);

            if (connection & 0b1) {
                //top left corner
                SDL_Rect chunkUL = { origin.x, origin.y + 48, 8, 8 };
                SDL_Rect dstUL = { dst.x, dst.y, dst.w / 2, dst.h / 2 };
                SDL_RenderCopy(g_rd, tex, &chunkUL, &dstUL);
            }
        }
        else if (connection <= 0b101001) {
            SDL_Rect src = { waterTileOrigin.x, waterTileOrigin.y, 16, 16 };
            SDL_RenderCopy(g_rd, tex, &src, &dst);

            // bottom left border tile
            SDL_Rect chunkDL = { origin.x, origin.y + 8, 8, 8 };
            SDL_Rect dstDL = { dst.x, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &chunkDL, &dstDL);
            SDL_Rect chunkUL = { origin.x, origin.y + 16, 8, 8 };
            SDL_Rect dstUL = { dst.x, dst.y, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &chunkUL, &dstUL);
            SDL_Rect chunkDR = { origin.x + 8, origin.y + 32 + 8, 8, 8 };
            SDL_Rect dstDR = { dst.x + dst.w / 2, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &chunkDR, &dstDR);

            if (connection & 0b1) {
                //top right corner
                SDL_Rect chunkUR = { origin.x + 8, origin.y + 48, 8, 8 };
                SDL_Rect dstUR = { dst.x + dst.w / 2, dst.y, dst.w / 2, dst.h / 2 };
                SDL_RenderCopy(g_rd, tex, &chunkUR, &dstUR);
            }
            
        }
        else if (connection <= 0b101110) {
            switch (connection & 0b111) {
            case 0b010:
            {
                //top left + top right border tile
                SDL_Rect src = { origin.x, origin.y, 16, 8 };
                SDL_Rect dst2 = { dst.x, dst.y, dst.w, dst.h / 2 };
                SDL_RenderCopy(g_rd, tex, &src, &dst2);

                SDL_Rect src2 = { origin.x, origin.y + 16 + 8, 16, 8 };
                dst2 = { dst.x, dst.y + dst.h / 2, dst.w, dst.h / 2 };
                SDL_RenderCopy(g_rd, tex, &src2, &dst2);
            }
                break;
            case 0b011:
            {
                //top left + bottom left border tile
                SDL_Rect src = { origin.x, origin.y, 8, 16 };
                SDL_Rect dst2 = { dst.x, dst.y, dst.w / 2, dst.h };
                SDL_RenderCopy(g_rd, tex, &src, &dst2);

                SDL_Rect src2 = { origin.x + 8, origin.y + 32, 8, 16 };
                dst2 = { dst.x + dst.w / 2, dst.y, dst.w / 2, dst.h };
                SDL_RenderCopy(g_rd, tex, &src2, &dst2);
            }
                break;
            case 0b100:
            {
                //bottom left + bottom right border tile
                SDL_Rect src = { origin.x, origin.y + 8, 16, 8 };
                SDL_Rect dst2 = { dst.x, dst.y + dst.h / 2, dst.w, dst.h / 2 };
                SDL_RenderCopy(g_rd, tex, &src, &dst2);

                SDL_Rect src2 = { origin.x, origin.y + 16, 16, 8 };
                dst2 = { dst.x, dst.y, dst.w, dst.h / 2 };
                SDL_RenderCopy(g_rd, tex, &src2, &dst2);
            }
                break;
            case 0b101:
            {
                // top right + bottom right border tile
                SDL_Rect src = { origin.x + 8, origin.y, 8, 16 };
                SDL_Rect dst2 = { dst.x + dst.w / 2, dst.y, dst.w / 2, dst.h };
                SDL_RenderCopy(g_rd, tex, &src, &dst2);

                SDL_Rect src2 = { origin.x, origin.y + 32, 8, 16 };
                dst2 = { dst.x, dst.y, dst.w / 2, dst.h };
                SDL_RenderCopy(g_rd, tex, &src2, &dst2);
            }
                break;
            case 0b110:
            {
                SDL_Rect src = { origin.x, origin.y, 16, 16 };
                SDL_RenderCopy(g_rd, tex, &src, &dst);
            }
                break;
            }
        }

        if (g_debugConfig.debugShowTilesRPG2K) {
            g_fnt->RenderString(frmt("W{:02X}\n{}", connection, watertileIndex), dst.x, dst.y);

            SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
            SDL_RenderDrawRect(g_rd, &dst);
        }

        if (watertileIndex != 2) {

            SDL_Rect partialDWChunk = { waterTileOrigin.x, waterTileOrigin.y + 16, 8, 8 };
            if (dwLeft && dwUp) {
                SDL_Rect src = { partialDWChunk.x, partialDWChunk.y, 8, 8 };
                SDL_Rect dstPartial = { dst.x, dst.y, dst.w / 2, dst.h / 2 };
                SDL_RenderCopy(g_rd, tex, &src, &dstPartial);
            }
            if (dwRight && dwUp) {
                SDL_Rect src = { partialDWChunk.x + 8, partialDWChunk.y, 8, 8 };
                SDL_Rect dstPartial = { dst.x + dst.w / 2, dst.y, dst.w / 2, dst.h / 2 };
                SDL_RenderCopy(g_rd, tex, &src, &dstPartial);
            }
            if (dwLeft && dwDown) {
                SDL_Rect src = { partialDWChunk.x, partialDWChunk.y + 8, 8, 8 };
                SDL_Rect dstPartial = { dst.x, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
                SDL_RenderCopy(g_rd, tex, &src, &dstPartial);
            }
            if (dwRight && dwDown) {
                SDL_Rect src = { partialDWChunk.x + 8, partialDWChunk.y + 8, 8, 8 };
                SDL_Rect dstPartial = { dst.x + dst.w / 2, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
                SDL_RenderCopy(g_rd, tex, &src, &dstPartial);
            }
        }
        else {
            SDL_Rect partialDWChunk = { waterTileOrigin.x, waterTileOrigin.y - 16, 8, 8 };
            if (connection == 0) {
                if (!dwUL) {
                    SDL_Rect src = { partialDWChunk.x, partialDWChunk.y, 8, 8 };
                    SDL_Rect dstPartial = { dst.x, dst.y, dst.w / 2, dst.h / 2 };
                    SDL_RenderCopy(g_rd, tex, &src, &dstPartial);
                }
                if (!dwUR) {
                    SDL_Rect src = { partialDWChunk.x + 8, partialDWChunk.y, 8, 8 };
                    SDL_Rect dstPartial = { dst.x + dst.w / 2, dst.y, dst.w / 2, dst.h / 2 };
                    SDL_RenderCopy(g_rd, tex, &src, &dstPartial);
                }
                if (!dwDL) {
                    SDL_Rect src = { partialDWChunk.x, partialDWChunk.y + 8, 8, 8 };
                    SDL_Rect dstPartial = { dst.x, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
                    SDL_RenderCopy(g_rd, tex, &src, &dstPartial);
                }
                if (!dwDR) {
                    SDL_Rect src = { partialDWChunk.x + 8, partialDWChunk.y + 8, 8, 8 };
                    SDL_Rect dstPartial = { dst.x + dst.w / 2, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
                    SDL_RenderCopy(g_rd, tex, &src, &dstPartial);
                }
            }
            
        }
    } else {
        g_fnt->RenderString(frmt("W{:02X}\n{}", connection, watertileIndex), dst.x, dst.y);

        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
        SDL_RenderDrawRect(g_rd, &dst);
    }
}

void RPG2KTilemapPreviewScreen::RenderAutoTile(uint8_t connection, uint16_t autotileIndex, SDL_Rect dst, SDL_Texture* tex)
{
    XY autotileOrigin = { 0,0 };
    if (autotileIndex < 4) {
        autotileOrigin.y += 8 * 16;
    }
    else {
        autotileIndex -= 4;
        autotileOrigin.x += 6 * 16;
    }
    autotileOrigin.x += (autotileIndex % 2) * 3 * 16;
    autotileOrigin.y += (autotileIndex / 2) * 4 * 16;

    //caveman code
    if (connection <= 0b1111) {
        SDL_Rect centerTile = { autotileOrigin.x + 16, autotileOrigin.y + 32, 16, 16 };
        SDL_RenderCopy(g_rd, tex, &centerTile, &dst);
        XY cornerPieceOrigin = { autotileOrigin.x + 32, autotileOrigin.y};
        SDL_Rect cornerPiece;
        if (connection & 0b0001) {
            // top left
            cornerPiece = { cornerPieceOrigin.x, cornerPieceOrigin.y, 8, 8 };
            SDL_Rect cornerDst = { dst.x, dst.y, dst.w/2, dst.h/2 };
            SDL_RenderCopy(g_rd, tex, &cornerPiece, &cornerDst);
        } 
        if (connection & 0b0010) {
            // top right
            cornerPiece = { cornerPieceOrigin.x + 8, cornerPieceOrigin.y, 8, 8 };
            SDL_Rect cornerDst = { dst.x + dst.w / 2, dst.y, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &cornerPiece, &cornerDst);
        }
        if (connection & 0b0100) {
            // bottom right
            cornerPiece = { cornerPieceOrigin.x + 8, cornerPieceOrigin.y + 8, 8, 8 };
            SDL_Rect cornerDst = { dst.x + dst.w / 2, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &cornerPiece, &cornerDst);
        }
        if (connection & 0b1000) {
            // bottom left
            cornerPiece = { cornerPieceOrigin.x, cornerPieceOrigin.y + 8, 8, 8 };
            SDL_Rect cornerDst = { dst.x, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &cornerPiece, &cornerDst);
        }
    }
    else if (connection <= 0b010011) {
        SDL_Rect leftBorderTile = { autotileOrigin.x, autotileOrigin.y + 32, 16, 16 };
        SDL_RenderCopy(g_rd, tex, &leftBorderTile, &dst);
        XY cornerPieceOrigin = { autotileOrigin.x + 32, autotileOrigin.y };
        SDL_Rect cornerPiece;
        if (connection & 0b01) {
            // top right corner
            cornerPiece = { cornerPieceOrigin.x + 8, cornerPieceOrigin.y, 8, 8 };
            SDL_Rect cornerDst = { dst.x + dst.w / 2, dst.y, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &cornerPiece, &cornerDst);
        }
        if (connection & 0b10) {
            // bottom right corner
            cornerPiece = { cornerPieceOrigin.x + 8, cornerPieceOrigin.y + 8, 8, 8 };
            SDL_Rect cornerDst = { dst.x + dst.w / 2, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &cornerPiece, &cornerDst);
        }
    }
    else if (connection <= 0b10111) {
        SDL_Rect topBorderTile = { autotileOrigin.x + 16, autotileOrigin.y + 16, 16, 16 };
        SDL_RenderCopy(g_rd, tex, &topBorderTile, &dst);
        XY cornerPieceOrigin = { autotileOrigin.x + 32, autotileOrigin.y };
        SDL_Rect cornerPiece;
        if (connection & 0b01) {
            // bottom right corner
            cornerPiece = { cornerPieceOrigin.x + 8, cornerPieceOrigin.y + 8, 8, 8 };
            SDL_Rect cornerDst = { dst.x + dst.w / 2, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &cornerPiece, &cornerDst);
        }
        if (connection & 0b10) {
            // bottom left corner
            cornerPiece = { cornerPieceOrigin.x, cornerPieceOrigin.y + 8, 8, 8 };
            SDL_Rect cornerDst = { dst.x, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &cornerPiece, &cornerDst);
        }
    }
    else if (connection <= 0b11011) {
        SDL_Rect rightBorderTile = { autotileOrigin.x + 32, autotileOrigin.y + 32, 16, 16 };
        SDL_RenderCopy(g_rd, tex, &rightBorderTile, &dst);
        XY cornerPieceOrigin = { autotileOrigin.x + 32, autotileOrigin.y };
        SDL_Rect cornerPiece;
        if (connection & 0b01) {
            //bottom left corner
            cornerPiece = { cornerPieceOrigin.x, cornerPieceOrigin.y + 8, 8, 8 };
            SDL_Rect cornerDst = { dst.x, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &cornerPiece, &cornerDst);
        }
        if (connection & 0b10) {
            //top left corner
            cornerPiece = { cornerPieceOrigin.x, cornerPieceOrigin.y, 8, 8 };
            SDL_Rect cornerDst = { dst.x, dst.y, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &cornerPiece, &cornerDst);
        }
    }
    else if (connection <= 0b11111) {
        SDL_Rect bottomBorderTile = { autotileOrigin.x + 16, autotileOrigin.y + 48, 16, 16 };
        SDL_RenderCopy(g_rd, tex, &bottomBorderTile, &dst);
        XY cornerPieceOrigin = { autotileOrigin.x + 32, autotileOrigin.y };
        SDL_Rect cornerPiece;
        if (connection & 0b01) {
            //top left corner
            cornerPiece = { cornerPieceOrigin.x, cornerPieceOrigin.y, 8, 8 };
            SDL_Rect cornerDst = { dst.x, dst.y, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &cornerPiece, &cornerDst);
        }
        if (connection & 0b10) {
            //top right corner
            cornerPiece = { cornerPieceOrigin.x + 8, cornerPieceOrigin.y, 8, 8 };
            SDL_Rect cornerDst = { dst.x + dst.w / 2, dst.y, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &cornerPiece, &cornerDst);
        }
    }
    else if (connection == 0b100000) {
        // left + right border tile
        SDL_Rect leftBorderTile = { autotileOrigin.x, autotileOrigin.y + 32, 16, 16 };
        SDL_RenderCopy(g_rd, tex, &leftBorderTile, &dst);
        SDL_Rect rightBorderTileFragment = { autotileOrigin.x + 40, autotileOrigin.y + 32, 8, 16 };
        SDL_Rect rightBorderTileDst = { dst.x + dst.w/2, dst.y, dst.w/2, dst.h };
        SDL_RenderCopy(g_rd, tex, &rightBorderTileFragment, &rightBorderTileDst);
    }
    else if (connection == 0b100001) {
        // top + bottom border tile
        SDL_Rect topBorderTile = { autotileOrigin.x + 16, autotileOrigin.y + 16, 16, 16 };
        SDL_RenderCopy(g_rd, tex, &topBorderTile, &dst);
        SDL_Rect bottomBorderTileFragment = { autotileOrigin.x + 16, autotileOrigin.y + 56, 16, 8 };
        SDL_Rect bottomBorderTileDst = { dst.x, dst.y + dst.h / 2, dst.w, dst.h / 2 };
        SDL_RenderCopy(g_rd, tex, &bottomBorderTileFragment, &bottomBorderTileDst);
    }
    else if (connection <= 0b100011) {
        // top left border tile
        SDL_Rect topLeftBorderTile = { autotileOrigin.x, autotileOrigin.y + 16, 16, 16 };
        SDL_RenderCopy(g_rd, tex, &topLeftBorderTile, &dst);
        XY cornerPieceOrigin = { autotileOrigin.x + 32, autotileOrigin.y };
        SDL_Rect cornerPiece;
        if (connection & 0b1) {
            // bottom right corner
            cornerPiece = { cornerPieceOrigin.x + 8, cornerPieceOrigin.y + 8, 8, 8 };
            SDL_Rect cornerDst = { dst.x + dst.w / 2, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &cornerPiece, &cornerDst);
        }
    }
    else if (connection <= 0b100101) {
        // top right border tile
        SDL_Rect topRightBorderTile = { autotileOrigin.x + 32, autotileOrigin.y + 16, 16, 16 };
        SDL_RenderCopy(g_rd, tex, &topRightBorderTile, &dst);
        XY cornerPieceOrigin = { autotileOrigin.x + 32, autotileOrigin.y };
        SDL_Rect cornerPiece;
        if (connection & 0b1) {
            // bottom left corner
            cornerPiece = { cornerPieceOrigin.x, cornerPieceOrigin.y + 8, 8, 8 };
            SDL_Rect cornerDst = { dst.x, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &cornerPiece, &cornerDst);
        }
    }
    else if (connection <= 0b100111) {
        // bottom right border tile
        SDL_Rect bottomRightBorderTile = { autotileOrigin.x + 32, autotileOrigin.y + 48, 16, 16 };
        SDL_RenderCopy(g_rd, tex, &bottomRightBorderTile, &dst);
        XY cornerPieceOrigin = { autotileOrigin.x + 32, autotileOrigin.y };
        SDL_Rect cornerPiece;
        if (connection & 0b1) {
            // top left corner
            cornerPiece = { cornerPieceOrigin.x, cornerPieceOrigin.y, 8, 8 };
            SDL_Rect cornerDst = { dst.x, dst.y, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &cornerPiece, &cornerDst);
        }
    }
    else if (connection <= 0b101001) {
        // bottom left border tile
        SDL_Rect bottomLeftBorderTile = { autotileOrigin.x, autotileOrigin.y + 48, 16, 16 };
        SDL_RenderCopy(g_rd, tex, &bottomLeftBorderTile, &dst);
        XY cornerPieceOrigin = { autotileOrigin.x + 32, autotileOrigin.y };
        SDL_Rect cornerPiece;
        if (connection & 0b1) {
            // top right corner
            cornerPiece = { cornerPieceOrigin.x + 8, cornerPieceOrigin.y, 8, 8 };
            SDL_Rect cornerDst = { dst.x + dst.w / 2, dst.y, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &cornerPiece, &cornerDst);
        }
    }
    else if (connection <= 0b101110) {
        switch (connection & 0b111) {
        case 0b010:
        {
            //top left + top right border tiles
            SDL_Rect topLeftBorderTile = { autotileOrigin.x, autotileOrigin.y + 16, 16, 16 };
            SDL_RenderCopy(g_rd, tex, &topLeftBorderTile, &dst);
            SDL_Rect topRightBorderPiece = { autotileOrigin.x + 32 + 8, autotileOrigin.y + 16, 8, 16 };
            SDL_Rect topRightBorderDst = { dst.x + dst.w / 2, dst.y, dst.w / 2, dst.h };
            SDL_RenderCopy(g_rd, tex, &topRightBorderPiece, &topRightBorderDst);
        }
            break;
        case 0b011:
        {
            //top left + bottom left border tiles
            SDL_Rect topLeftBorderTile = { autotileOrigin.x, autotileOrigin.y + 16, 16, 16 };
            SDL_RenderCopy(g_rd, tex, &topLeftBorderTile, &dst);
            SDL_Rect bottomLeftBorderPiece = { autotileOrigin.x, autotileOrigin.y + 48 + 8, 16, 8 };
            SDL_Rect bottomLeftBorderDst = { dst.x, dst.y + dst.h / 2, dst.w, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &bottomLeftBorderPiece, &bottomLeftBorderDst);
        }
            break;
        case 0b100:
        {
            //bottom left + bottom right border tiles
            SDL_Rect bottomLeftBorderTile = { autotileOrigin.x, autotileOrigin.y + 48, 16, 16 };
            SDL_RenderCopy(g_rd, tex, &bottomLeftBorderTile, &dst);
            SDL_Rect bottomRightBorderPiece = { autotileOrigin.x + 32 + 8, autotileOrigin.y + 48, 8, 16 };
            SDL_Rect bottomRightBorderDst = { dst.x + dst.w / 2, dst.y, dst.w / 2, dst.h };
            SDL_RenderCopy(g_rd, tex, &bottomRightBorderPiece, &bottomRightBorderDst);
        }
            break;
        case 0b101:
        {
            //top right + bottom right border tiles
            SDL_Rect topRightBorderTile = { autotileOrigin.x + 32, autotileOrigin.y + 16, 16, 16 };
            SDL_RenderCopy(g_rd, tex, &topRightBorderTile, &dst);
            SDL_Rect bottomRightBorderPiece = { autotileOrigin.x + 32, autotileOrigin.y + 48 + 8, 16, 8 };
            SDL_Rect bottomRightBorderDst = { dst.x, dst.y + dst.h / 2, dst.w, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &bottomRightBorderPiece, &bottomRightBorderDst);
        }
            break;
        case 0b110:
        {
            //8x8: top left + top right + bottom left + bottom right border tiles
            SDL_Rect topLeftBorderTile = { autotileOrigin.x, autotileOrigin.y + 16, 16, 16 };
            SDL_RenderCopy(g_rd, tex, &topLeftBorderTile, &dst);
            SDL_Rect topRightBorderPiece = { autotileOrigin.x + 32 + 8, autotileOrigin.y + 16, 8, 8 };
            SDL_Rect topRightBorderDst = { dst.x + dst.w / 2, dst.y, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &topRightBorderPiece, &topRightBorderDst);
            SDL_Rect bottomLeftBorderPiece = { autotileOrigin.x, autotileOrigin.y + 48 + 8, 8, 8 };
            SDL_Rect bottomLeftBorderDst = { dst.x, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &bottomLeftBorderPiece, &bottomLeftBorderDst);
            SDL_Rect bottomRightBorderPiece = { autotileOrigin.x + 32 + 8, autotileOrigin.y + 48 + 8, 8, 8 };
            SDL_Rect bottomRightBorderDst = { dst.x + dst.w / 2, dst.y + dst.h / 2, dst.w / 2, dst.h / 2 };
            SDL_RenderCopy(g_rd, tex, &bottomRightBorderPiece, &bottomRightBorderDst);
        }
            break;
        }
    }
    else if (connection <= 0b110000) {
        //just the center piece
        SDL_Rect centerTile = { autotileOrigin.x + 16, autotileOrigin.y + 32, 16, 16 };
        SDL_RenderCopy(g_rd, tex, &centerTile, &dst);
    }
    else if (connection == 0b110001) {
        //x0 y0 tile
        SDL_Rect centerTile = { autotileOrigin.x, autotileOrigin.y, 16, 16 };
        SDL_RenderCopy(g_rd, tex, &centerTile, &dst);
    }

    if (g_debugConfig.debugShowTilesRPG2K) {
        g_fnt->RenderString(frmt("a{}\n{}", connection, autotileIndex), dst.x, dst.y);

        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
        SDL_RenderDrawRect(g_rd, &dst);
    }
}

void RPG2KTilemapPreviewScreen::RenderRPG2KTile(uint16_t tile, XY position, SDL_Rect dst, int animState)
{
    SDL_Texture* draw = callerCanvas->get();

    char type = 'o';
    uint16_t index = tile;
    if (index >= 0x2710) {   //upper layer
        index -= 0x2710;
        XY startPos = { 18 * 16, 0 };
        if (index <= 48) {
            startPos.y += 8 * 16;
        }
        else {
            startPos.x += 6 * 16;
            index -= 48;
        }
        startPos.x += (index % 6) * 16;
        startPos.y += (index / 6) * 16;
        SDL_Rect src = { startPos.x, startPos.y, 16, 16 };
        SDL_RenderCopy(g_rd, draw, &src, &dst);
        type = 'u';
    }
    else if (index >= 0x1388) { //lower layer
        index -= 0x1388;
        XY startPos = { 12 * 16, 0 };
        if (index >= 96) {
            startPos.x += 6 * 16;
            index -= 96;
        }
        startPos.x += (index % 6) * 16;
        startPos.y += (index / 6) * 16;
        SDL_Rect src = { startPos.x, startPos.y, 16, 16 };
        SDL_RenderCopy(g_rd, draw, &src, &dst);
        type = 'l';
    }
    else if (index >= 0x0FA0) { //autotiles
        index -= 0x0FA0;
        int autotileIndex = index / 50;
        int autotileConnectionType = index % 50;
        RenderAutoTile(autotileConnectionType, autotileIndex, dst, draw);
        type = 'a';
    }
    else if (index >= 0x0BB8) { //animated tiles
        int frame = SDL_GetTicks64() % 1200 / 300;
        index -= 0xBB8;
        index /= 50;
        //index is correct (from 0 to 2)
        XY origin = { 3 * 16, 4 * 16 };
        origin.x += index * 16;
        origin.y += frame * 16;
        SDL_Rect src = { origin.x, origin.y, 16, 16 };
        SDL_RenderCopy(g_rd, draw, &src, &dst);
        
        //g_fnt->RenderString(frmt("an{}", index), dst.x, dst.y);
        //index = 0x0207 + (((index - 0x0BB8) / 50) << 2) + frame;
        type = 'n';
    }
    else {  //water tiles
        int frame = 0;
        int watertile = index % 50;
        int watertype = index / 50 / 20;
        //index = watertype * 141 + watertile + frame * 47;
        RenderWaterTile(watertile, watertype, position, dst, draw);
        type = 'w';
    }
}

void RPG2KTilemapPreviewScreen::RenderEvents(XY originPoint, int canvasScale)
{
    for (LMUEvent& evt : events) {
        if ((eventViewMode == LMUEVENTS_SHOW_INGAME || eventViewMode == LMUEVENTS_SHOW_INGAME_AND_RECTS) && evt.tex != NULL) {
            SDL_Rect dst = { 
                originPoint.x + evt.pos.x * 16 * canvasScale - 4 * canvasScale,
                originPoint.y + evt.pos.y * 16 * canvasScale - 16 * canvasScale,
                24 * canvasScale,
                32 * canvasScale };
            XY srcCharsetOrigin = { (evt.charsetIndex % 4) * (24 * 3) , (evt.charsetIndex / 4) * (32 * 4) };
            SDL_Rect srcCharset = { 
                srcCharsetOrigin.x + (24 * evt.charsetPattern), srcCharsetOrigin.y + (32 * evt.charsetDirection),
                24, 32 
            };
            SDL_RenderCopy(g_rd, evt.tex->get(), &srcCharset, &dst);
        }

        if (eventViewMode == LMUEVENTS_SHOW_RECTS || eventViewMode == LMUEVENTS_SHOW_INGAME_AND_RECTS) {
            SDL_Rect evtRect = { originPoint.x + evt.pos.x * 16 * canvasScale, originPoint.y + evt.pos.y * 16 * canvasScale, 16 * canvasScale, 16 * canvasScale };
            evtRect.x += 4;
            evtRect.y += 4;
            evtRect.w -= 8;
            evtRect.h -= 8;
            SDL_SetRenderDrawColor(g_rd, 255, 255, 255, evt.tex != NULL ? 0x40 : 0x80);
            SDL_RenderDrawRect(g_rd, &evtRect);
        }
    }
}

void RPG2KTilemapPreviewScreen::RenderWholeMap(XY at, int sscale, bool rdLowerLayer, bool rdUpperLayer, bool rdEvents)
{
    PrerenderCanvas();

    //todo: make this work on raw pixel data instead of sdl textures

    for (int y = 0; y < dimensions.y; y++) {
        if (!forceOptimizationsOff && at.y + y * 16 * sscale > g_windowH) {
            break;
        }
        if (!forceOptimizationsOff && at.y + (y + 1) * 16 * sscale < 0) {
            continue;
        }
        for (int x = 0; x < dimensions.x; x++) {
            if (!forceOptimizationsOff && at.x + x * 16 * sscale > g_windowW) {
                break;
            }
            if (!forceOptimizationsOff && at.x + (x + 1) * 16 * sscale < 0) {
                continue;
            }
            SDL_Rect dst = { at.x + x * 16 * sscale, at.y + y * 16 * sscale, 16 * sscale, 16 * sscale };
            uint16_t lowerTile = lowerLayerData[y * dimensions.x + x];
            uint16_t upperTile = upperLayerData[y * dimensions.x + x];
            if (rdLowerLayer) {
                RenderRPG2KTile(lowerTile, { x, y }, dst);
            }
            if (rdUpperLayer) {
                RenderRPG2KTile(upperTile, { x, y }, dst);
            }
        }
    }
    if (rdEvents) {
        RenderEvents(at, sscale);
    }
}

Layer* RPG2KTilemapPreviewScreen::RenderWholeMapToTexture()
{
    Layer* newLayer = new Layer(dimensions.x * 16, dimensions.y * 16);
    forceOptimizationsOff = true;
    bool failed = false;
    //SDL_RenderPresent(g_rd);
    for (int y = 0; y < dimensions.y * 16 && !failed; y += g_windowH) {
        for (int x = 0; x < dimensions.x * 16 && !failed; x += g_windowW) {
            //due to a bug in sdl only the window area gets rendered
            SDL_Texture* newTexture = tracked_createTexture(g_rd, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, g_windowW, g_windowH);
            Layer* blitLayer = new Layer(g_windowW, g_windowH);
            g_pushRenderTarget(newTexture);
            SDL_FlushRenderer(g_rd);
            SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0);
            SDL_SetRenderDrawBlendMode(g_rd, SDL_BLENDMODE_NONE);
            SDL_RenderClear(g_rd);
            SDL_SetRenderDrawBlendMode(g_rd, SDL_BLENDMODE_BLEND);
            RenderWholeMap({ -x,-y }, 1);
            SDL_Surface* newSrf = SDL_RenderReadPixels(g_rd, NULL);
            if (newSrf == NULL) {
                g_addNotification(ErrorNotification("Error", "SDL_RenderReadPixels failed"));
            }
            SDL_ConvertPixels(blitLayer->w, blitLayer->h, newSrf->format, newSrf->pixels, newSrf->pitch,
                              SDL_PIXELFORMAT_ARGB8888, blitLayer->pixels32(), blitLayer->w * 4);
            SDL_FreeSurface(newSrf);
            g_popRenderTarget();
            tracked_destroyTexture(newTexture);

            newLayer->blit(blitLayer, { x,y });
            delete blitLayer;
        }
    }
    forceOptimizationsOff = false;
    if (!failed) {
        newLayer->updateTexture();
        return newLayer;
    }
    else {
        delete newLayer;
        return NULL;
    }
}

bool RPG2KTilemapPreviewScreen::LoadLMU(PlatformNativePathString path)
{
    for (auto& tx : texturesLoaded) {
        delete tx.second;
    }
    texturesLoaded.clear();
    events.clear();

    PlatformNativePathString directoryOfFile = path.substr(0, path.find_last_of({ '/', '\\' }) + 1);
    std::ifstream file(path, std::ios::binary);
    if (file.is_open()) {

        std::unique_ptr<lcf::rpg::Map> map(lcf::LMU_Reader::Load(file));

        //freeAllLayers();
        //tilemapDimensions = { map->width, map->height };
        dimensions = { map->width, map->height };
        canvas.dimensions = { dimensions.x * 16, dimensions.y * 16 };
        RecenterCanvas();
        uint16_t* lowerLayer = new uint16_t[map->width * map->height];
        uint16_t* upperLayer = new uint16_t[map->width * map->height];
        int dataPointer = 0;
        for (int y = 0; y < map->height; y++) {
            for (int x = 0; x < map->width; x++) {
                lowerLayer[y * map->width + x] = map->lower_layer[dataPointer];
                upperLayer[y * map->width + x] = map->upper_layer[dataPointer];
                dataPointer++;
            }
        }

        int charsetLoadFails = 0;
        for (lcf::rpg::Event& evt : map->events) {
            LMUEvent newEvt;
            newEvt.pos = { evt.x, evt.y };
            //todo: convert name from shift-jis to utf8
            newEvt.name = std::string(evt.name);
            if (evt.pages.size() > 0) {
                newEvt.texFileName = shiftJIStoUTF8(std::string(evt.pages[0].character_name));
                newEvt.charsetIndex = evt.pages[0].character_index;
                newEvt.charsetDirection = evt.pages[0].character_direction;
                newEvt.charsetPattern = evt.pages[0].character_pattern;
                //todo: convert texFile from shift-jis to utf8
                //or even better try to find it from the [EasyRPG] section in the ini file
                std::string texFileName = newEvt.texFileName;

                if (texFileName != "") {

                    if (!texturesLoaded.contains(newEvt.texFileName)) {
                        
                        texturesLoaded[newEvt.texFileName] = new ReldTex([directoryOfFile, texFileName](SDL_Renderer* rd) {

                            SDL_Texture* tex = NULL;

                            PlatformNativePathString charsetPath = directoryOfFile + convertStringOnWin32("/CharSet/" + texFileName + ".xyz");
                            if (std::filesystem::exists(charsetPath)) {
                                //xyz has no alpha channel so let's just assume that the first color in the palette is transparency
                                LayerPalettized* nl = (LayerPalettized*)readXYZ(charsetPath);
                                if (nl != NULL) {
                                    nl->palette[0] &= 0xffffff;
                                    nl->updateTexture();
                                    tex = nl->renderToTexture();
                                    delete nl;
                                }
                            }

                            if (tex == NULL) {
                                charsetPath = directoryOfFile + convertStringOnWin32("/CharSet/" + texFileName + ".png");
                                if (std::filesystem::exists(charsetPath)) {
                                    Layer* nl = readPNG(charsetPath);
                                    if (nl != NULL) {
                                        if (nl->isPalettized) {
                                            ((LayerPalettized*)nl)->palette[0] &= 0xffffff;
                                            ((LayerPalettized*)nl)->updateTexture();
                                        }
                                        tex = nl->renderToTexture();
                                        delete nl;
                                    }
                                }
                            }

                            return tex;
                        });
                        
                    }

                    newEvt.tex = texturesLoaded[newEvt.texFileName];

                    if (newEvt.tex == NULL) {
                        charsetLoadFails += 1;
                        loginfo(frmt("charset load failed: {}", newEvt.texFileName));
                    }
                }
                
            }

            events.push_back(newEvt);
            /*std::cout << "-----lmu event\n";
            std::cout << " - position: " << evt.x << ", " << evt.y << "\n";
            std::cout << " - event name: " << evt.name << "\n";
            std::cout << " - pages[0].name: " << newEvt.texFileName << "\n";
            std::cout << " - charset index: " << evt.pages[0].character_index << ", direction: " << evt.pages[0].character_direction << ", pattern: " << evt.pages[0].character_pattern << "\n";*/
        }
        if (charsetLoadFails > 0) {
            g_addNotification(ErrorNotification("Error", frmt("Failed to load charsets for {} Events", charsetLoadFails)));
        }

        if (lowerLayerData != NULL) {
            delete[] lowerLayerData;
        }
        if (upperLayerData != NULL) {
            delete[] upperLayerData;
        }
        lowerLayerData = lowerLayer;
        upperLayerData = upperLayer;

        file.close();
        return true;
    }
    else {
        g_addNotification(Notification("Error loading file", "Could not open file for reading."));
        return false;
    }
}

