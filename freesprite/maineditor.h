#pragma once
#include "globals.h"
#include "BaseScreen.h"
#include "DrawableManager.h"
#include "EditorColorPicker.h"

class MainEditor : public BaseScreen
{
public:
	SDL_Texture* mainTexture;
	int* lockedPixels;
	int pitch;
	bool textureLocked = false;
	int texW, texH;
	XY canvasCenterPoint;
	XY mousePixelTargetPoint;
	int scale = 1;
	XY mouseHoldPosition;
	bool leftMouseHold = false;
	bool middleMouseHold = false;

	uint32_t pickedColor = 0xFFFFFF;

	DrawableManager wxsManager;
	EditorColorPicker* colorPicker;

	MainEditor(XY dimensions);

	void render() override;
	void tick() override;
	void takeInput(SDL_Event evt) override;
	
	void RecalcMousePixelTargetPoint(int x, int y);
	void FillTexture();
	void SetPixel(XY position, uint32_t color);
	void DrawLine(XY from, XY to, uint32_t color);
	void EnsureTextureLocked();
	void EnsureTextureUnlocked();
};

