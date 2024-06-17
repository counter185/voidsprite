#include "TilemapPreviewScreen.h"
#include "maineditor.h"
#include "FontRenderer.h"

void TilemapPreviewScreen::render()
{
    SDL_Rect tilemapDrawRect = { tilemapDrawPoint.x, tilemapDrawPoint.y,
        tilemapDimensions.x * caller->tileDimensions.x * tilemapScale,
        tilemapDimensions.y * caller->tileDimensions.y * tilemapScale };

    for (int y = 0; y < tilemapDimensions.y; y++) {
        for (int x = 0; x < tilemapDimensions.x; x++) {

            XY tilePos = tilemap[y][x];
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

            for (Layer* l : caller->layers) {
                SDL_RenderCopy(g_rd, l->tex, &tileClip, &tileDraw);
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
    SDL_Rect panelRect = { 0,0, ixmax(400, 30 + caller->tileDimensions.x * tilemapScale * 2), ixmax(200, 110 + caller->tileDimensions.y * tilemapScale * 2)};
    SDL_RenderFillRect(g_rd, &panelRect);

    g_fnt->RenderString("Tileset preview", 5, 5);
    g_fnt->RenderString("Selected tile: [TAB] to switch", 15, 60);
    SDL_Rect tileDraw = {
        15,
        90,
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
}

void TilemapPreviewScreen::tick()
{
    if (mouseLeftingTilemap) {
        if (caller->tileDimensions.x != 0 && caller->tileDimensions.y != 0) {
            if (hoveredTilePosition.x >= 0 && hoveredTilePosition.y >= 0 && hoveredTilePosition.x < tilemapDimensions.x && hoveredTilePosition.y < tilemapDimensions.y) {
                tilemap[hoveredTilePosition.y][hoveredTilePosition.x] = pickedTile;
            }
        }
    }
}

void TilemapPreviewScreen::takeInput(SDL_Event evt)
{
    if (evt.type == SDL_QUIT) {
        g_closeScreen(this);
        return;
    }
    if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == 1 && evt.button.state) {
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

void TilemapPreviewScreen::resizeTilemap(int w, int h)
{
    XY** newTilemap = (XY**)malloc(h * sizeof(XY*));
    for (int y = 0; y < h; y++) {
        newTilemap[y] = (XY*)malloc(w * sizeof(XY));
        for (int x = 0; x < w; x++) {
            newTilemap[y][x] = XY{ -1,-1 };
        }
    }

    if (tilemap != NULL) {
        for (int y = 0; y < tilemapDimensions.y; y++) {
            if (y < h) {
                memcpy(newTilemap[y], tilemap[y], sizeof(XY) * ixmin(w, tilemapDimensions.x));
            }
            free(tilemap[y]);
        }
        free(tilemap);
    }

    tilemap = newTilemap;
    tilemapDimensions = { w,h };
}
