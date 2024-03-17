#pragma once
#include "globals.h"
#include "BaseScreen.h"
#include "DrawableManager.h"
#include "EditorColorPicker.h"

class MainEditor : public BaseScreen
{
public:
	SDL_Texture* mainTexture;
	int* lockedPixels = NULL;
	int pitch = 0;
	bool textureLocked = false;
	int texW = -1, texH = -1;
	XY canvasCenterPoint = XY{0,0};
	XY mousePixelTargetPoint;
	int scale = 1;
	XY mouseHoldPosition;
	bool leftMouseHold = false;
	bool middleMouseHold = false;

	uint32_t pickedColor = 0xFFFFFF;

	DrawableManager wxsManager;
	EditorColorPicker* colorPicker;

	MainEditor(XY dimensions);
	MainEditor(std::string file);

	void render() override;
	void tick() override;
	void takeInput(SDL_Event evt) override;
	
	void DrawBackground();
	void DrawForeground();
	void SetUpWidgets();
	void RecalcMousePixelTargetPoint(int x, int y);
	void FillTexture();
	void SetPixel(XY position, uint32_t color);
	void DrawLine(XY from, XY to, uint32_t color);
	void EnsureTextureLocked();
	void EnsureTextureUnlocked();
};

