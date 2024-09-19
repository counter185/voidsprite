#include <lcf/lmu/reader.h>

#include "RPG2KTilemapPreviewScreen.h"
#include "ScreenWideNavBar.h"
#include "maineditor.h"
#include "Notification.h"
#include "FontRenderer.h"

RPG2KTilemapPreviewScreen::RPG2KTilemapPreviewScreen(MainEditor* parent)
{
    caller = parent;

    navbar = new ScreenWideNavBar<RPG2KTilemapPreviewScreen*>(this,
        {
            {
                SDLK_f,
                {
                    "File",
                    {},
                    {
                        {SDLK_o, { "Load layout from file",
                                [](RPG2KTilemapPreviewScreen* screen) {
                                    platformTryLoadOtherFile(screen, {{".lmu", "RPGM2000/2003 Map"}}, "Load tile layout", EVENT_OTHERFILE_OPENFILE);
                                }
                            }
                        },
                        /*{SDLK_s, {"Save layout to file",
                                [](TilemapPreviewScreen* screen) {
                                    platformTrySaveOtherFile(screen, { {".voidtile", "voidtile layout"} }, "Save tile layout", EVENT_OTHERFILE_SAVEFILE);
                                }
                            }
                        },*/
                    },
                    g_iconNavbarTabFile
                }
            }
        }, { SDLK_f });
    wxsManager.addDrawable(navbar);

    lowerLayerData = new uint16_t[dimensions.x * dimensions.y];
    upperLayerData = new uint16_t[dimensions.x * dimensions.y];
    memset(lowerLayerData, 0, dimensions.x * dimensions.y * sizeof(uint16_t));
    memset(upperLayerData, 0, dimensions.x * dimensions.y * sizeof(uint16_t));

    //resizeTilemap(32, 32);
}

RPG2KTilemapPreviewScreen::~RPG2KTilemapPreviewScreen()
{
    wxsManager.freeAllDrawables();
    if (lowerLayerData != NULL) {
        delete[] lowerLayerData;
    }
    if (upperLayerData != NULL) {
        delete[] upperLayerData;
    }
}

void RPG2KTilemapPreviewScreen::render()
{
    TilemapPreviewScreen::drawBackground();

    for (int y = 0; y < dimensions.y; y++) {
        for (int x = 0; x < dimensions.x; x++) {
            SDL_Rect dst = { canvasDrawPoint.x + x * 16 * scale, canvasDrawPoint.y + y * 16 * scale, 16 * scale, 16 * scale };
            uint16_t lowerTile = lowerLayerData[y * dimensions.x + x];
            uint16_t upperTile = upperLayerData[y * dimensions.x + x];
            RenderRPG2KTile(lowerTile, {x, y}, dst);
            RenderRPG2KTile(upperTile, {x, y}, dst);
        }
    }
    SDL_Rect overallRect = { canvasDrawPoint.x-1, canvasDrawPoint.y-1, dimensions.x * 16 * scale + 2, dimensions.y * 16 * scale + 2 };
    SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
    SDL_RenderDrawRect(g_rd, &overallRect);

    wxsManager.renderAll();
}

void RPG2KTilemapPreviewScreen::tick()
{
}

void RPG2KTilemapPreviewScreen::takeInput(SDL_Event evt)
{
    if (evt.type == SDL_QUIT) {
        g_closeScreen(this);
        return;
    }
    if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.state) {
        wxsManager.tryFocusOnPoint(XY{ evt.button.x, evt.button.y });
    }
    if (wxsManager.anyFocused()) {
        wxsManager.passInputToFocused(evt);
    }
    else {
        if (evt.type == SDL_MOUSEWHEEL) {
            if (evt.wheel.y > 0) {
                scale++;
            }
            else {
                if (scale-- <= 1) {
                    scale = 1;
                }
            }
        }
    }
}

void RPG2KTilemapPreviewScreen::eventFileOpen(int evt_id, PlatformNativePathString path, int importer_index)
{
    if (evt_id == EVENT_OTHERFILE_OPENFILE && importer_index == 1) {
        LoadLMU(path);
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

void RPG2KTilemapPreviewScreen::RenderWaterTile(uint8_t connection, uint16_t watertileIndex, XY position, SDL_Rect dst, SDL_Texture* tex)
{
    int waterAnimLength = 500;
    int animState = SDL_GetTicks64() % (waterAnimLength*4) / waterAnimLength;
    if (animState == 3) {
        animState = 1;
    }

    XY waterTileOrigin = { 16 * animState, 64 };

    bool dwUp = isDeepWaterTileAt({ position.x, position.y - 1 });
    bool dwRight = isDeepWaterTileAt({ position.x + 1, position.y });
    bool dwLeft = isDeepWaterTileAt({ position.x - 1, position.y });
    bool dwDown = isDeepWaterTileAt({ position.x, position.y + 1 });

    if (watertileIndex <= 1) {
        XY origin = { 16 * 3 * watertileIndex + 16 * animState, 0 };
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
        else {
            switch (connection) {
            case 0x2E:
            {
                //x2E - center tile
                SDL_Rect src = { origin.x, origin.y, 16, 16 };
                SDL_RenderCopy(g_rd, tex, &src, &dst);
            }
            break;
            case 0x20:
            {
                //x20 - vertical tile
                SDL_Rect src = { origin.x, origin.y + 16, 16, 16 };
                SDL_RenderCopy(g_rd, tex, &src, &dst);
            }
            break;
            case 0x21:
            {
                //x21 - horizontal tile
                SDL_Rect src = { origin.x, origin.y + 32, 16, 16 };
                SDL_RenderCopy(g_rd, tex, &src, &dst);
            }
            break;
            case 0x1c:
            {
                //border bottom
                SDL_Rect src = { waterTileOrigin.x, waterTileOrigin.y, 16, 16 };
                SDL_RenderCopy(g_rd, tex, &src, &dst);
                SDL_Rect src2 = { origin.x, origin.y + 32 + 8, 16, 8 };
                SDL_Rect dst2 = { dst.x, dst.y + dst.h / 2, dst.w, dst.h / 2 };
                SDL_RenderCopy(g_rd, tex, &src2, &dst2);
            }
            break;
            case 0x18:
            {
                //border right
                SDL_Rect src = { waterTileOrigin.x, waterTileOrigin.y, 16, 16 };
                SDL_RenderCopy(g_rd, tex, &src, &dst);
                SDL_Rect src2 = { origin.x + 8, origin.y + 16, 8, 16 };
                SDL_Rect dst2 = { dst.x + dst.w / 2, dst.y, dst.w / 2, dst.h };
                SDL_RenderCopy(g_rd, tex, &src2, &dst2);
            }
            break;
            default:
                g_fnt->RenderString(std::format("W{:02X}\n{}", connection, watertileIndex), dst.x, dst.y);

                SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
                SDL_RenderDrawRect(g_rd, &dst);
                break;
            }
        }

        SDL_Rect partialDWChunk = {waterTileOrigin.x, waterTileOrigin.y + 16, 8, 8};
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
    } else {
        g_fnt->RenderString(std::format("W{:02X}\n{}", connection, watertileIndex), dst.x, dst.y);

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
    else {
        g_fnt->RenderString(std::format("a{}\n{}", connection, autotileIndex), dst.x, dst.y);

        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
        SDL_RenderDrawRect(g_rd, &dst);
    }
}

void RPG2KTilemapPreviewScreen::RenderRPG2KTile(uint16_t tile, XY position, SDL_Rect dst)
{

    //TODO PLEASE CHANGE THIS
    SDL_Texture* draw = caller->getCurrentLayer()->tex;

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
        
        //g_fnt->RenderString(std::format("an{}", index), dst.x, dst.y);
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

void RPG2KTilemapPreviewScreen::LoadLMU(PlatformNativePathString path)
{
    PlatformNativePathString directoryOfFile = path.substr(0, path.find_last_of({ '/', '\\' }) + 1);
    std::ifstream file(path, std::ios::binary);
    if (file.is_open()) {
        std::unique_ptr<lcf::rpg::Map> map(lcf::LMU_Reader::Load(file));

        //freeAllLayers();
        //tilemapDimensions = { map->width, map->height };
        dimensions = { map->width, map->height };
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

        if (lowerLayerData != NULL) {
            delete[] lowerLayerData;
        }
        if (upperLayerData != NULL) {
            delete[] upperLayerData;
        }
        lowerLayerData = lowerLayer;
        upperLayerData = upperLayer;

        file.close();
    }
    else {
        g_addNotification(Notification("Error loading file", "Could not open file for reading."));
    }
}

