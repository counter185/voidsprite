#include "TilemapPreviewScreen.h"
#include "maineditor.h"
#include "FontRenderer.h"
#include "ScreenWideNavBar.h"
#include "Notification.h"
#include "TilemapEditorLayerPicker.h"
#include <lcf/lmu/reader.h>
#include <lcf/rpg/map.h>
#include <lcf/reader_lcf.h>

TilemapPreviewScreen::TilemapPreviewScreen(MainEditor* parent) {
    caller = parent;

    navbar = new ScreenWideNavBar<TilemapPreviewScreen*>(this,
        {
            {
                SDLK_f,
                {
                    "File",
                    {},
                    {
                        {SDLK_o, { "Load layout from file",
                                [](TilemapPreviewScreen* screen) {
                                    platformTryLoadOtherFile(screen, {{".voidtile", "voidtile layout"}, {".lmu", "RPGM2000/2003 Map"}}, "Load tile layout", EVENT_OTHERFILE_OPENFILE);
                                }
                            }
                        },
                        {SDLK_s, { "Save layout to file",
                                [](TilemapPreviewScreen* screen) {
                                    platformTrySaveOtherFile(screen, { {".voidtile", "voidtile layout"} }, "Save tile layout", EVENT_OTHERFILE_SAVEFILE);
                                }
                            }
                        },
                    },
                    g_iconNavbarTabFile
                }
            }
        }, { SDLK_f });
    wxsManager.addDrawable(navbar);

    resizeTilemap(32, 32);

    layerPicker = new TilemapEditorLayerPicker(this);
    wxsManager.addDrawable(layerPicker);

    tileSelectScale = caller->scale;
    recenterTilePicker();
    recenterTilemap();
}

TilemapPreviewScreen::~TilemapPreviewScreen()
{
    for (int l = 0; l < tilemap.size(); l++) {
		XY**& tilemapLayer = tilemap.at(l);
		if (tilemapLayer != NULL) {
			for (int y = 0; y < tilemapDimensions.y; y++) {
				free(tilemapLayer[y]);
			}
			free(tilemapLayer);
		}
	}
	tilemap.clear();
}

void TilemapPreviewScreen::render()
{
    drawBackground();

    SDL_Rect tilemapDrawRect = { tilemapDrawPoint.x, tilemapDrawPoint.y,
        tilemapDimensions.x * caller->tileDimensions.x * tilemapScale,
        tilemapDimensions.y * caller->tileDimensions.y * tilemapScale };

    for (int l = 0; l < tilemap.size(); l++) {
        for (int y = 0; y < tilemapDimensions.y; y++) {
            for (int x = 0; x < tilemapDimensions.x; x++) {

                XY tilePos = tilemap[l][y][x];
                if (tilePos.x < 0 || tilePos.y < 0) {
                    continue;
                }

                SDL_Rect tileDraw = {
                    tilemapDrawRect.x + caller->tileDimensions.x * tilemapScale * x,
                    tilemapDrawRect.y + caller->tileDimensions.y * tilemapScale * y,
                    caller->tileDimensions.x * tilemapScale,
                    caller->tileDimensions.y * tilemapScale
                };

                SDL_Rect tileClip = {
                    tilePos.x * caller->tileDimensions.x,
                    tilePos.y * caller->tileDimensions.y,
                    caller->tileDimensions.x,
                    caller->tileDimensions.y
                };

                uint8_t alpha = (layerSelectTimer.started && l == activeLayerIndex()) ? (uint8_t)(0xff * XM1PW3P1(layerSelectTimer.percentElapsedTime(1300))) : 0xff;
                for (Layer* l : caller->layers) {
                    SDL_SetTextureAlphaMod(l->tex, alpha);
                    SDL_RenderCopy(g_rd, l->tex, &tileClip, &tileDraw);
                }
            }
        }
    }

    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xa0);
    SDL_RenderDrawRect(g_rd, &tilemapDrawRect);

    SDL_Rect tilemapSelectedTile = {
        tilemapDrawRect.x + caller->tileDimensions.x * tilemapScale * hoveredTilePosition.x,
        tilemapDrawRect.y + caller->tileDimensions.y * tilemapScale * hoveredTilePosition.y,
        caller->tileDimensions.x * tilemapScale,
        caller->tileDimensions.y * tilemapScale
    };

    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xd0);
    SDL_RenderDrawRect(g_rd, &tilemapSelectedTile);

    SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0xa0);
    SDL_Rect panelRect = { 0,40, ixmax(400, 30 + caller->tileDimensions.x * tilemapScale * 2), ixmax(200, 110 + caller->tileDimensions.y * tilemapScale * 2)};
    SDL_RenderFillRect(g_rd, &panelRect);

    g_fnt->RenderString("Tileset preview", panelRect.x + 5, panelRect.y+5);
    g_fnt->RenderString("Selected tile: [TAB] to switch", panelRect.x + 15, panelRect.y + 60);
    SDL_Rect tileDraw = {
        panelRect.x + 15,
        panelRect.y + 90,
        caller->tileDimensions.x * tilemapScale*2,
        caller->tileDimensions.y * tilemapScale*2
    };

    SDL_Rect tileClip = {
        pickedTile.x * caller->tileDimensions.x,
        pickedTile.y * caller->tileDimensions.y,
        caller->tileDimensions.x,
        caller->tileDimensions.y
    };

    for (Layer* l : caller->layers) {
        SDL_RenderCopy(g_rd, l->tex, &tileClip, &tileDraw);
    }
    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x90);
    SDL_RenderDrawRect(g_rd, &tileDraw);


    if (tileSelectOpen) {

        SDL_Rect tsbgRect = { 0,0, g_windowW * XM1PW3P1(tileSelectTimer.percentElapsedTime(300)), g_windowH};
        SDL_SetRenderDrawColor(g_rd, 0, 0, 0, (uint8_t)(0xd0 * XM1PW3P1(tileSelectTimer.percentElapsedTime(300))));
        SDL_RenderFillRect(g_rd, &tsbgRect);

        int xOffset = -g_windowW * (1.0 - XM1PW3P1(tileSelectTimer.percentElapsedTime(300)));

        SDL_Rect tileSelectRect = { tileSelectOffset.x + xOffset, tileSelectOffset.y,
            caller->texW * tileSelectScale,
            caller->texH * tileSelectScale
        };

        for (Layer* l : caller->layers) {
            SDL_RenderCopy(g_rd, l->tex, NULL, &tileSelectRect);
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
    tilemapDrawPoint = XY{
        iclamp(-tilemapDimensions.x * caller->tileDimensions.x * tilemapScale + 4, tilemapDrawPoint.x, g_windowW - 4),
        iclamp(-tilemapDimensions.y * caller->tileDimensions.y * tilemapScale + 4, tilemapDrawPoint.y, g_windowH - 4)
    };
    tileSelectOffset = XY{
        iclamp(-caller->texW * tileSelectScale + 4, tileSelectOffset.x, g_windowW - 4),
        iclamp(-caller->texH * tileSelectScale + 4, tileSelectOffset.y, g_windowH - 4)
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
    if (evt.type == SDL_QUIT) {
        g_closeScreen(this);
        return;
    }
    if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.state) {
        wxsManager.tryFocusOnPoint(XY{ evt.button.x, evt.button.y });
    }

    if (!wxsManager.anyFocused()) {
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
                    tilemapDrawPoint = xyAdd(tilemapDrawPoint, XY{ evt.motion.xrel, evt.motion.yrel });
                }

                if (caller->tileDimensions.x != 0 && caller->tileDimensions.y != 0) {
                    XY pos = xySubtract(XY{ evt.button.x, evt.button.y }, tilemapDrawPoint);

                    pos.x /= caller->tileDimensions.x * tilemapScale;
                    pos.y /= caller->tileDimensions.y * tilemapScale;
                    hoveredTilePosition = pos;
                }
            }
            break;
        case SDL_MOUSEWHEEL:
            if (tileSelectOpen) {
                tileSelectScale += evt.wheel.y;
                tileSelectScale = ixmax(1, tileSelectScale);
            }
            else {
                tilemapScale += evt.wheel.y;
                tilemapScale = ixmax(1, tilemapScale);
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
    else {
        wxsManager.passInputToFocused(evt);
    }
}

BaseScreen* TilemapPreviewScreen::isSubscreenOf()
{
    return caller;
}

void TilemapPreviewScreen::eventFileSaved(int evt_id, PlatformNativePathString name, int exporterIndex)
{
    FILE* file = platformOpenFile(name, PlatformFileModeWB);
    if (file == NULL) {
        g_addNotification(Notification("Error saving file", "Could not open file for writing."));
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
        g_addNotification(Notification("File saved", "Save successful!", 4000));
    }
}

void TilemapPreviewScreen::eventFileOpen(int evt_id, PlatformNativePathString name, int importerIndex)
{
    //
    //if (name.find(utf8StringToWstring(".lmu")) == name.size() - 4) {
    if (importerIndex == 2) {
        
    }
    else {
        FILE* file = platformOpenFile(name, PlatformFileModeRB);
        if (file == NULL) {
            g_addNotification(Notification("Error loading file", "Could not open file for writing."));
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
                g_addNotification(Notification("Error loading file", "File version not supported"));
                break;
            }
            fclose(file);
            layerPicker->updateLayers();
        }
    }
}

void TilemapPreviewScreen::resizeTilemap(int w, int h)
{
    if (tilemap.size() > 0) {
        for (int l = 0; l < tilemap.size(); l++) {
            XY**& tilemapLayer = tilemap.at(l);

            XY** newTilemap = (XY**)malloc(h * sizeof(XY*));
            for (int y = 0; y < h; y++) {
                newTilemap[y] = (XY*)malloc(w * sizeof(XY));
                for (int x = 0; x < w; x++) {
                    newTilemap[y][x] = XY{ -1,-1 };
                }
            }

            if (tilemapLayer != NULL) {
                for (int y = 0; y < tilemapDimensions.y; y++) {
                    if (y < h) {
                        memcpy(newTilemap[y], tilemapLayer[y], sizeof(XY) * ixmin(w, tilemapDimensions.x));
                    }
                    free(tilemapLayer[y]);
                }
                free(tilemapLayer);
            }

            tilemap[l] = newTilemap;
        }
    }
    else {
        XY** newTilemap = (XY**)malloc(h * sizeof(XY*));
        for (int y = 0; y < h; y++) {
            newTilemap[y] = (XY*)malloc(w * sizeof(XY));
            for (int x = 0; x < w; x++) {
                newTilemap[y][x] = XY{ -1,-1 };
            }
        }
        tilemap.push_back(newTilemap);
    }
    
    tilemapDimensions = { w,h };
    if (activeTilemap == NULL) {
        activeTilemap = tilemap[0];
    }
}

void TilemapPreviewScreen::drawBackground()
{
    uint32_t colorBG1 = 0xFF000000;//| (sdlcolorToUint32(backgroundColor) == 0xFF000000 ? 0x000000 : 0xDFDFDF);
    uint32_t colorBG2 = 0xFF000000 | 0x202020;//| (sdlcolorToUint32(backgroundColor) == 0xFF000000 ? 0x202020 : 0x808080);
    renderGradient({ 0,0, g_windowW, g_windowH }, colorBG1, colorBG1, colorBG1, colorBG2);

    uint64_t now = SDL_GetTicks64();
    uint64_t progress = now % 120000;
    for (int y = -(1.0 - progress / 120000.0) * g_windowH; y < g_windowH; y += 50) {
        if (y >= 0) {
            SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x22);
            SDL_RenderDrawLine(g_rd, 0, y, g_windowW, y);
        }
    }

    for (int x = -(1.0 - (now % 100000) / 100000.0) * g_windowW; x < g_windowW; x += 30) {
        if (x >= 0) {
            SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x19);
            SDL_RenderDrawLine(g_rd, x, 0, x, g_windowH);
        }
    }
}

void TilemapPreviewScreen::recenterTilemap()
{
    tilemapDrawPoint = XY{
        (g_windowW / 2) - (tilemapDimensions.x * caller->tileDimensions.x * tilemapScale) / 2,
        (g_windowH / 2) - (tilemapDimensions.y * caller->tileDimensions.y * tilemapScale) / 2
    };
}

void TilemapPreviewScreen::recenterTilePicker()
{
    tileSelectOffset = XY{
        (g_windowW / 2) - (caller->texW * tileSelectScale) / 2,
        (g_windowH / 2) - (caller->texH * tileSelectScale) / 2
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
				free(tilemapLayer[y]);
			}
			free(tilemapLayer);
		}
	}
	tilemap.clear();
	activeTilemap = NULL;
}

XY** TilemapPreviewScreen::newLayer()
{
    XY** newTilemap = (XY**)malloc(tilemapDimensions.y * sizeof(XY*));
	for (int y = 0; y < tilemapDimensions.y; y++) {
		newTilemap[y] = (XY*)malloc(tilemapDimensions.x * sizeof(XY));
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
			free(tilemapLayer[y]);
		}
        free(tilemapLayer);
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
	XY** newTilemap = (XY**)malloc(tilemapDimensions.y * sizeof(XY*));
	for (int y = 0; y < tilemapDimensions.y; y++) {
		newTilemap[y] = (XY*)malloc(tilemapDimensions.x * sizeof(XY));
		for (int x = 0; x < tilemapDimensions.x; x++) {
			newTilemap[y][x] = tilemapLayer[y][x];
		}
	}
	tilemap.insert(tilemap.begin() + index, newTilemap);
}
