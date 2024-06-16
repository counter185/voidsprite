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
	drawBackground();

	SDL_Rect canvasRenderRect = { canvasDrawOrigin.x, canvasDrawOrigin.y, caller->texW * canvasZoom, caller->texH * canvasZoom };
	for (Layer*& l : caller->layers) {
		SDL_RenderCopy(g_rd, l->tex, NULL, &canvasRenderRect);
	}


	int dx = canvasRenderRect.x;
	while (dx < g_windowW && dx < canvasRenderRect.x + canvasRenderRect.w) {
		dx += caller->tileDimensions.x * canvasZoom;
		if (dx >= 0) {
			SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
			SDL_RenderDrawLine(g_rd, dx, canvasRenderRect.y, dx, canvasRenderRect.y + canvasRenderRect.h);
		}
	}
	

	int dy = canvasRenderRect.y;
	while (dy < g_windowH && dy < canvasRenderRect.y + canvasRenderRect.h) {
		dy += caller->tileDimensions.y * canvasZoom;
		if (dy >= 0) {
			SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
			SDL_RenderDrawLine(g_rd, canvasRenderRect.x, dy, canvasRenderRect.x + canvasRenderRect.w, dy);
		}
	}
	
	SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x80);
	SDL_RenderDrawRect(g_rd, &canvasRenderRect);

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

	int rightPanelWidth = ixmax(300, canvasZoom * tileSize.x);

	SDL_Rect rightPanelRect = { g_windowW - rightPanelWidth, 0, rightPanelWidth, g_windowH };
	SDL_SetRenderDrawColor(g_rd, 0x00, 0x00, 0x00, 0xe0);
	SDL_RenderFillRect(g_rd, &rightPanelRect);

	g_fnt->RenderString("Preview sprites", rightPanelRect.x + 3, 4);

	drawPreview(XY{ rightPanelRect.x + 10, 100 });

	wxsManager.renderAll();

	XY spriteListOrigin = xyAdd(spriteView->position, spriteView->scrollOffset);

	if (sprites.size() > 0) {
		for (int x = 0; x < sprites.size(); x++) {
			XY spritePos = xyAdd(spriteListOrigin, XY{ x * caller->tileDimensions.x * canvasZoom + x * 5 , 50 });
			drawPreview(spritePos, x);
			SDL_Rect spriteArea = {
				spritePos.x,
				spritePos.y,
				caller->tileDimensions.x * canvasZoom,
				caller->tileDimensions.y * canvasZoom
			};
			SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x30);
			SDL_RenderDrawRect(g_rd, &spriteArea);
			g_fnt->RenderString(std::to_string(x), spritePos.x, spritePos.y - 24);
		}
	}
	else {
		g_fnt->RenderString("Click on sprites to add them to the timeline...", 20, spriteView->position.y + 60);
	}

	g_fnt->RenderString("Timeline", 2, spriteView->position.y + 2);

}

void SpritesheetPreviewScreen::tick()
{
	int rightPanelWidth = ixmax(300, canvasZoom * caller->tileDimensions.x);
	XY origin = { g_windowW - rightPanelWidth, 20 };
	msPerSpriteLabel->position = { origin.x + 5, origin.y + 20 };
	textfieldMSPerSprite->position = { origin.x + 130, origin.y + 20 };

	spriteView->wxWidth = g_windowW;
	spriteView->wxHeight = 30 + caller->tileDimensions.y * canvasZoom + 60;
	spriteView->position = { 0, g_windowH - spriteView->wxHeight };
}

void SpritesheetPreviewScreen::takeInput(SDL_Event evt)
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

void SpritesheetPreviewScreen::drawPreview(XY at, int which)
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
		XY currentSprite = sprites[which == -1 ? spritesProgress : which];

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

void SpritesheetPreviewScreen::drawBackground()
{
	uint64_t now = SDL_GetTicks64();
	uint64_t progress = now % 120000;
	for (int y = -(1.0 - progress/120000.0)*g_windowH; y < g_windowH; y += 50) {
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
