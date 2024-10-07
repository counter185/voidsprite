#pragma once
#include "BaseBrush.h"

class Brush1x1 : public BaseBrush
{
	std::string getName() override { return "1x1 Pixel"; }
	std::string getIconPath() override { return VOIDSPRITE_ASSETS_PATH "assets/brush_px1x1.png"; }
	void clickPress(MainEditor* editor, XY pos) override;
	void clickDrag(MainEditor* editor, XY from, XY to) override;
	void clickRelease(MainEditor* editor, XY pos) override {}
	void renderOnCanvas(XY canvasDrawPoint, int scale) {
		SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
		drawLocalPoint(canvasDrawPoint, lastMouseMotionPos, scale);
		SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
		drawPointOutline(canvasDrawPoint, lastMouseMotionPos, scale);
	}
};

class Brush1x1PixelPerfect : public BaseBrush
{
	XY lastPoint = { 0,0 };
	XY lastTrailPoint = { 0,0 };
	bool hasTrailPoint = false;
	bool dragging = false;

	std::string getName() override { return "1x1 Pixel (Pixel-Perfect)"; }
	std::string getIconPath() override { return VOIDSPRITE_ASSETS_PATH "assets/brush_px1x1pxpf.png"; }
	void clickPress(MainEditor* editor, XY pos) override;
	void clickDrag(MainEditor* editor, XY from, XY to) override;
	void clickRelease(MainEditor* editor, XY pos) override;
	void renderOnCanvas(XY canvasDrawPoint, int scale) {
		SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
		drawLocalPoint(canvasDrawPoint, lastMouseMotionPos, scale);
		SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
		drawPointOutline(canvasDrawPoint, lastMouseMotionPos, scale);
	}
};

