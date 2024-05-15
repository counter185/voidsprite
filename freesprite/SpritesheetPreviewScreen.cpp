#include "SpritesheetPreviewScreen.h"
#include "FontRenderer.h"
#include "maineditor.h"

SpritesheetPreviewScreen::~SpritesheetPreviewScreen() {
	wxsManager.freeAllDrawables();
	delete previewWx;
	caller->spritesheetPreview = NULL;
}

void SpritesheetPreviewScreen::render()
{

	SDL_Rect canvasRenderRect = { canvasDrawOrigin.x, canvasDrawOrigin.y, caller->texW * canvasZoom, caller->texH * canvasZoom };
	for (Layer*& l : caller->layers) {
		SDL_RenderCopy(g_rd, l->tex, NULL, &canvasRenderRect);
	}

	SDL_Rect rightPanelRect = { g_windowW / 2, 0, g_windowW / 2, g_windowH };
	SDL_SetRenderDrawColor(g_rd, 0x0b, 0x0b, 0x0b, 0xd0);
	SDL_RenderFillRect(g_rd, &rightPanelRect);

	g_fnt->RenderString("Preview sprites", g_windowW / 2 + 3, 4);

	XY tileSize = caller->tileDimensions;

	int i = 0;
	for (XY& sprite : sprites) {
		SDL_Rect spriteArea = {
			canvasDrawOrigin.x + (sprite.x * tileSize.x * canvasZoom),
			canvasDrawOrigin.y + (sprite.y * tileSize.y * canvasZoom),
			tileSize.x * canvasZoom,
			tileSize.y * canvasZoom
		};
		if (spritesProgress == i) {
			SDL_SetRenderDrawColor(g_rd, 0, 255, 0, 0xa0);
		}
		else {
			SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xa0);
		}
		SDL_RenderDrawRect(g_rd, &spriteArea);
		g_fnt->RenderString(std::to_string(i++), spriteArea.x, spriteArea.y, SDL_Color{255,255,255,0xa0});
	}

	drawPreview(XY{ g_windowW / 4 * 3, g_windowH / 3 });

	wxsManager.renderAll();
}

void SpritesheetPreviewScreen::tick()
{
	XY origin = { g_windowW / 2, 20 };
	msPerSpriteLabel->position = { origin.x + 5, origin.y + 20 };
	textfieldMSPerSprite->position = { origin.x + 130, origin.y + 20 };
}

void SpritesheetPreviewScreen::takeInput(SDL_Event evt)
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
					scrollingCanvas = true;
				}
				else if (evt.button.button == SDL_BUTTON_LEFT) {
					if (caller->tileDimensions.x != 0 && caller->tileDimensions.y != 0) {
						XY pos = xySubtract(XY{ evt.button.x, evt.button.y }, canvasDrawOrigin);
						if (pos.x >= 0 && pos.y >= 0 && pos.x < (caller->texW * canvasZoom) && pos.y < (caller->texH * canvasZoom)) {
							pos.x /= caller->tileDimensions.x * canvasZoom;
							pos.y /= caller->tileDimensions.y * canvasZoom;
							sprites.push_back(pos);
						}
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
					canvasDrawOrigin = xyAdd(canvasDrawOrigin, XY{ evt.motion.xrel, evt.motion.yrel });
				}
				break;
			case SDL_MOUSEWHEEL:
				canvasZoom += evt.wheel.y;
				canvasZoom = ixmax(1, canvasZoom);
				break;
		}
	}
	else {
		wxsManager.passInputToFocused(evt);
	}
}

BaseScreen* SpritesheetPreviewScreen::isSubscreenOf() { 
	return caller; 
}

void SpritesheetPreviewScreen::eventTextInput(int evt_id, std::string data)
{
	if (evt_id == EVENT_SPRITEPREVIEW_SET_SPRITE_TICK) {
		try {
			msPerSprite = std::stoi(data);
			textfieldMSPerSprite->bgColor = { 0,0,0,0xff };
		}
		catch (std::exception) {
			msPerSprite = -1;
			textfieldMSPerSprite->bgColor = { 80,0,0,0xff };
		}
	}
}

void SpritesheetPreviewScreen::drawPreview(XY at)
{
	if (!sprites.empty() && msPerSprite > 0) {
		spritesProgress = (SDL_GetTicks64() / msPerSprite) % sprites.size();
	}

	if (!sprites.empty()) {
		SDL_Rect spriteDrawArea = { at.x, at.y,
			caller->tileDimensions.x * canvasZoom,
			caller->tileDimensions.y * canvasZoom
		};
		if (spritesProgress >= sprites.size()) {
			spritesProgress = 0;
		}
		XY currentSprite = sprites[spritesProgress];

		SDL_Rect layersClipArea = {
			currentSprite.x * caller->tileDimensions.x,
			currentSprite.y * caller->tileDimensions.y,
			caller->tileDimensions.x,
			caller->tileDimensions.y
		};

		for (Layer*& l : caller->layers) {
			SDL_RenderCopy(g_rd, l->tex, &layersClipArea, &spriteDrawArea);
		}
	}
}
