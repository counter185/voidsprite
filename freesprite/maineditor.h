#pragma once
#include "globals.h"
#include "BaseScreen.h"
#include "DrawableManager.h"
#include "EditorColorPicker.h"
#include "BaseBrush.h"
#include "Brush1x1.h"
#include "Brush3pxCircle.h"
#include "Brush1pxLine.h"
#include "BrushRect.h"
#include "Layer.h"

class MainEditor : public BaseScreen, public EventCallbackListener
{
private:

public:
	Layer* imgLayer;

	int texW = -1, texH = -1;
	XY canvasCenterPoint = XY{0,0};
	XY mousePixelTargetPoint;
	int scale = 1;
	XY mouseHoldPosition;
	BaseBrush* currentBrush = new Brush1x1();
	bool leftMouseHold = false;
	bool middleMouseHold = false;

	bool eraserMode = false;
	uint32_t pickedColor = 0xFFFFFF;

	DrawableManager wxsManager;
	EditorColorPicker* colorPicker;
	EditorBrushPicker* brushPicker;

	MainEditor(XY dimensions);
	MainEditor(SDL_Surface* srf);
	MainEditor(Layer* srf);
	~MainEditor();

	void render() override;
	void tick() override;
	void takeInput(SDL_Event evt) override;

	void eventFileSaved(int evt_id, std::string name) override;
	
	void DrawBackground();
	void DrawForeground();
	void SetUpWidgets();
	void RecalcMousePixelTargetPoint(int x, int y);
	void FillTexture();
	void SetPixel(XY position, uint32_t color);
	void DrawLine(XY from, XY to, uint32_t color);
	void trySaveImage();

	//DEPRECATED DO NOT USE
	void EnsureTextureLocked();
	void EnsureTextureUnlocked();
};

