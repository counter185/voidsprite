#include "TilemapPreviewScreen.h"
#include "maineditor.h"

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
                mouseLeftingTilemap = true;
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
            if (scrollingTilemap) {
                tilemapDrawPoint = xyAdd(tilemapDrawPoint, XY{ evt.motion.xrel, evt.motion.yrel });
            }

            if (caller->tileDimensions.x != 0 && caller->tileDimensions.y != 0) {
                XY pos = xySubtract(XY{ evt.button.x, evt.button.y }, tilemapDrawPoint);
                
                pos.x /= caller->tileDimensions.x * tilemapScale;
                pos.y /= caller->tileDimensions.y * tilemapScale;
                hoveredTilePosition = pos;
            }
            break;
        case SDL_MOUSEWHEEL:
            tilemapScale += evt.wheel.y;
            tilemapScale = ixmax(1, tilemapScale);
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
