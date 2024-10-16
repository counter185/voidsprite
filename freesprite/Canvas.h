#pragma once
#include "globals.h"

class Canvas
{
public:
	XY currentDrawPoint = {0,0};
	XY dimensions = { 0,0 };
	int scale = 1;

	bool takeInput(SDL_Event evt);
	void lockToScreenBounds();
	bool pointInCanvasBounds(XY point);
	void drawCanvasOutline(int shades = 1, SDL_Color c = { 255,255,255,255 });
	void drawTileGrid(XY tileSize);

	XY screenPointToCanvasPoint(XY screenPoint);
	XY getTilePosAt(XY screenPoint, XY tileSize = {0,0});
};

