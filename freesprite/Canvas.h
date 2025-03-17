#pragma once
#include "globals.h"

class Canvas
{
public:
	XY currentDrawPoint = {0,0};
	XY dimensions = { 0,0 };
	int scale = 1;
	bool middleMouseHold = false;

	//okay actually LET'S NOT USE THIS
	bool takeInput(SDL_Event evt);

	void lockToScreenBounds(int top = 0, int left = 0, int bottom = 0, int right = 0, XY bounds = {-1,-1});
	bool pointInCanvasBounds(XY point);
	void drawCanvasOutline(int shades = 1, SDL_Color c = { 255,255,255,255 });
	SDL_Rect getCanvasOnScreenRect();
	void drawTileGrid(XY tileSize);
	void zoom(int how_much);
	void panCanvas(XY by);
	void recenter(XY windowDimensions = {-1,-1});

	XY canvasPointToScreenPoint(XY canvasPoint);
	/// <summary>
	/// Turns a rect in canvas-space to a rect in screen-space. Scale is taken into consideration.
	/// </summary>
	/// <param name="canvasRect">Rect in canvas-space</param>
	/// <returns>Rect in screen-space</returns>
	SDL_Rect canvasRectToScreenRect(SDL_Rect canvasRect);
	XY screenPointToCanvasPoint(XY screenPoint);
	XY getTilePosAt(XY screenPoint, XY tileSize = {0,0});
	SDL_Rect getTileScreenRectAt(XY canvasTileIndex, XY tileSize);
	SDL_Rect getTileRectAt(XY canvasTileIndex, XY tileSize);
};

