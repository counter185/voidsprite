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
			RenderRPG2KTile(lowerTile, dst);
			RenderRPG2KTile(upperTile, dst);
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

void RPG2KTilemapPreviewScreen::RenderRPG2KTile(uint16_t tile, SDL_Rect dst)
{
    if (tile == 0) {
        return;
    }

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
        g_fnt->RenderString(std::format("a{}\n{}", autotileConnectionType, autotileIndex), dst.x, dst.y);

        SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
        SDL_RenderDrawRect(g_rd, &dst);
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
        type = 'w';
        g_fnt->RenderString(std::format("W{}", watertile), dst.x, dst.y);
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
                /*char type = '-';
                if (index >= 0x2710) {   //upper layer
                    index -= 0x2710;
                    type = 'u';
                }
                else if (index >= 0x1388) { //lower layer
                    index -= 0x1388;
                    type = 'l';
                }
                else if (index >= 0x0FA0) { //autotiles
                    index -= 0x0FA0;
                    int autotileIndex = index / 50;
                    type = 'a';
                }
                else if (index >= 0x0BB8) { //animated tiles
                    int frame = 0;
                    index = 0x0207 + (((index - 0x0BB8) / 50) << 2) + frame;
                    type = 'n';
                }
                else {  //water tiles
                    int frame = 0;
                    int watertile = index % 50;
                    int watertype = index / 50 / 20;
                    //index = watertype * 141 + watertile + frame * 47;
                    type = 'w';
                }

                uint8_t tileDirection = 0;
                uint16_t tileIndex = index;
                if (type == 'a') {
                    tileDirection = index % 50;
                    tileIndex = index / 50;
                }
                else if (type == 'w') {
                    tileDirection = index % 50;
                    tileIndex = index / 50 / 20;
                }

                printf("%c:(%02x;%04x),", type, tileDirection, tileIndex);
                //printf("%c:%04x,", type, index);*/

            }
            //printf("\n");
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

