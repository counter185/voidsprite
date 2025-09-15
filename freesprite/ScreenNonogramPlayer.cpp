#include "ScreenNonogramPlayer.h"
#include "FontRenderer.h"

void ScreenNonogramPlayer::StartDebugGame()
{
	u8 puzzle[] = {
		0,0,0,0,0,1,0,0,0,0,
		0,0,0,0,1,1,1,0,0,0,
		0,0,0,1,1,1,0,1,0,0,
		0,0,1,1,1,1,1,0,1,0,
		0,1,1,1,1,1,0,1,0,1,
		0,0,1,1,1,1,0,0,1,0,
		0,0,0,1,1,1,0,1,0,0,
		0,0,0,0,1,1,1,0,0,0,
		0,0,0,0,0,1,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,
	};
    u8* puzzledata2 = (u8*)tracked_malloc(10 * 10, "NonogramPlayer");
	memcpy(puzzledata2, puzzle, 10 * 10);
    ScreenNonogramPlayer* s = new ScreenNonogramPlayer(puzzledata2, 10,10);
	g_addScreen(s);
}

ScreenNonogramPlayer::ScreenNonogramPlayer(u8* puzzleData, int w, int h)
{
	puzzlePlayerInputData = (u8*)tracked_malloc(w * h, "NonogramPlayer");
	memset(puzzlePlayerInputData, 0, w * h);
	c = Canvas({w,h});
	c.minScale = 20;
	c.recenter();
    this->puzzleData = puzzleData;
	GenHints();
}

ScreenNonogramPlayer::~ScreenNonogramPlayer()
{
	if (puzzleData != NULL) {
		tracked_free(puzzleData);
	}
	if (puzzlePlayerInputData != NULL) {
		tracked_free(puzzlePlayerInputData);
	}
}

void ScreenNonogramPlayer::render()
{
	RenderBackground();
	c.lockToScreenBounds();

	SDL_Rect canvasRect = c.getCanvasOnScreenRect();
    SDL_SetRenderDrawColor(g_rd, 255,255,255, 255);
	SDL_Rect boundsRect = offsetRect(canvasRect, 1);
    SDL_RenderDrawRect(g_rd, &boundsRect);
	SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 30);
	c.drawTileGrid({1,1});

	for (int y = 0; y < c.dimensions.y; y++) {
		for (int x = 0; x < c.dimensions.x; x++) {
			u8 cellData = ARRAY2DPOINT(puzzlePlayerInputData, x, y, c.dimensions.x);
			SDL_Rect tileRect = c.getTileScreenRectAt({ x,y }, { 1,1 });
			if (cellData == 1) {
				//filled
				Fill::Solid(0xFFFFFFFF).fill(tileRect);
			}
			else if (cellData == 2) {
				//X
				SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 255);
				drawLine({tileRect.x + 4, tileRect.y + 4}, {tileRect.x + tileRect.w - 4, tileRect.y + tileRect.h - 4}, 1.0);
				drawLine({tileRect.x + 4, tileRect.y + tileRect.h - 4}, {tileRect.x + tileRect.w - 4, tileRect.y + 4}, 1.0);
			}
		}
	}

	//row hints
	for (int y = 0; y < c.dimensions.y; y++) {
		XY origin = c.canvasPointToScreenPoint({0, y});
		origin.x -= 20;
		for (auto& hint : rowHints[y]) {
			g_fnt->RenderString(std::to_string(hint), origin.x, origin.y);
			origin.x -= 15;
		}
	}

	//col hints
	for (int x = 0; x < c.dimensions.x; x++) {
		XY origin = c.canvasPointToScreenPoint({x, 0});
		origin.y -= 25;
		for (auto& hint : colHints[x]) {
			g_fnt->RenderString(std::to_string(hint), origin.x, origin.y);
			origin.y -= 25;
		}
	}

	if (pointInBox(mousePoint, { 0,0,c.dimensions.x, c.dimensions.y })) {
		
        SDL_Rect pointRect = c.canvasRectToScreenRect({(int)mousePoint.x, (int)mousePoint.y, 1, 1});
        Fill::Gradient(0x00FFFFFF, 0x00FFFFFF, 0x80FFFFFF, 0x80FFFFFF).fill(pointRect);
		SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 255);
		SDL_RenderDrawRect(g_rd, &pointRect);
		pointRect = offsetRect(pointRect, -1);
		SDL_SetRenderDrawColor(g_rd, 0,0,0,0x80);
		SDL_RenderDrawRect(g_rd, &pointRect);
	}
}

void ScreenNonogramPlayer::takeInput(SDL_Event evt)
{
	switch (evt.type) {
		case SDL_EVENT_MOUSE_MOTION:
			{
				if (mmiddleDown) {
					c.panCanvas({ (int)evt.motion.xrel, (int)evt.motion.yrel });
				}
				XY newMousePoint = c.screenPointToCanvasPoint({ (int)evt.motion.x, (int)evt.motion.y });
				if (!xyEqual(mousePoint, newMousePoint)) {
					UpdateMousePoint(newMousePoint);
					if (mleftDown) {
						PlaceFillAt(mousePoint);
					}
					else if (mrightDown) {
						PlaceXAt(mousePoint);
					}
				}
			}
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		case SDL_EVENT_MOUSE_BUTTON_UP:
			if (evt.button.button == SDL_BUTTON_MIDDLE) {
				mmiddleDown = evt.button.down;
			}
			else if (evt.button.button == SDL_BUTTON_LEFT) {
				mleftDown = evt.button.down;
				UpdateMousePoint(c.screenPointToCanvasPoint({ (int)evt.button.x, (int)evt.button.y }));
				if (mleftDown) {
					PlaceFillAt(mousePoint);
				}
			}
			else if (evt.button.button == SDL_BUTTON_RIGHT) {
				mrightDown = evt.button.down;
				UpdateMousePoint(c.screenPointToCanvasPoint({ (int)evt.button.x, (int)evt.button.y }));
				if (mrightDown) {
					PlaceXAt(mousePoint);
				}
			}
			break;
		case SDL_EVENT_MOUSE_WHEEL:
			c.zoomFromWheelInput(evt.wheel.y);
			break;
		case SDL_QUIT:
			g_closeScreen(this);
			break;
	}
}

void ScreenNonogramPlayer::GenHints()
{
	rowHints.clear();
	colHints.clear();
	ScanlineMap rowPointMap, colPointMap;
	for (int y = 0; y < c.dimensions.y; y++) {
		for (int x = 0; x < c.dimensions.x; x++) {
			if (ARRAY2DPOINT(puzzleData, x, y, c.dimensions.x) == 1) {
				rowPointMap.addPoint({ x,y });
				colPointMap.addPoint({ y,x });
			}
		}
	}

	rowHints.resize(c.dimensions.y);
	colHints.resize(c.dimensions.x);

	rowPointMap.forEachScanline([&](ScanlineMapElement sme) {
		rowHints[sme.origin.y].push_back(sme.size.x);
	});
	colPointMap.forEachScanline([&](ScanlineMapElement sme) {
		colHints[sme.origin.y].push_back(sme.size.x);
	});

	for (auto& rowh : rowHints) {
		if (rowh.size() == 0) {
			rowh.push_back(0);
		}
		std::reverse(rowh.begin(), rowh.end());
	}
	for (auto& colh : colHints) {
		if (colh.size() == 0) {
			colh.push_back(0);
		}
		std::reverse(colh.begin(), colh.end());
	}
}

void ScreenNonogramPlayer::RenderBackground()
{
    Fill::Gradient(0xFF000000, 0xFF000000, 0xFF000000, 0xFF303030).fill({0, 0, g_windowW, g_windowH});
}

void ScreenNonogramPlayer::PlaceFillAt(XY pos)
{
    if (pointInBox(pos, {0, 0, c.dimensions.x, c.dimensions.y})) {
		ARRAY2DPOINT(puzzlePlayerInputData, pos.x, pos.y, c.dimensions.x) = 1;
    }
}

void ScreenNonogramPlayer::PlaceXAt(XY pos)
{
	if (pointInBox(pos, { 0, 0, c.dimensions.x, c.dimensions.y })) {
		ARRAY2DPOINT(puzzlePlayerInputData, pos.x, pos.y, c.dimensions.x) = 2;
	}
}
