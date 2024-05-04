#pragma once
#include "globals.h"
#include "BaseScreen.h"
#include "DrawableManager.h"
#include "EditorColorPicker.h"
#include "EditorLayerPicker.h"
#include "BaseBrush.h"
#include "Brush1x1.h"
#include "Brush3pxCircle.h"
#include "Brush1pxLine.h"
#include "BrushRect.h"
#include "BrushRectFill.h"
#include "ToolRectClone.h"
#include "ToolColorPicker.h"
#include "Layer.h"

class MainEditor : public BaseScreen, public EventCallbackListener
{
public:

	std::vector<Layer*> layers;
	int selLayer = 0;
	
	Layer* flattenImage();

	int maxUndoHistory = 20;
	std::vector<Layer*> undoStack, redoStack;

	int texW = -1, texH = -1;
	XY tileDimensions = XY{ 0,0 };
	uint8_t tileGridAlpha = 0x40;
	XY canvasCenterPoint = XY{0,0};
	XY mousePixelTargetPoint;
	int scale = 1;
	XY mouseHoldPosition;
	bool closeNextTick = false;
	BaseBrush* currentBrush = new Brush1x1();
	bool leftMouseHold = false;
	bool middleMouseHold = false;

	bool changesSinceLastSave = false;
	PlatformNativePathString lastConfirmedSavePath;
	bool lastConfirmedSave = false;

	bool eraserMode = false;
	uint32_t pickedColor = 0xFFFFFF;

	GlobalNavBar* navbar;
	DrawableManager wxsManager;
	EditorColorPicker* colorPicker;
	EditorBrushPicker* brushPicker;
	EditorLayerPicker* layerPicker;

	SDL_Color backgroundColor = SDL_Color{0,0,0,255};

	MainEditor(XY dimensions);
	MainEditor(SDL_Surface* srf);
	MainEditor(Layer* srf);
	MainEditor(std::vector<Layer*> layers);
	~MainEditor();

	void render() override;
	void tick() override;
	void takeInput(SDL_Event evt) override;

	std::string getName() override { return "Editor"; }

	void eventFileSaved(int evt_id, PlatformNativePathString name) override;
	void eventPopupClosed(int evt_id, BasePopup* p) override;
	
	void DrawBackground();
	void DrawForeground();
	void initLayers();
	void SetUpWidgets();
	void RecalcMousePixelTargetPoint(int x, int y);
	void FillTexture();
	void SetPixel(XY position, uint32_t color);
	void DrawLine(XY from, XY to, uint32_t color);
	void trySaveImage();
	void trySaveAsImage();
	void recenterCanvas();
	bool requestSafeClose();

	void commitStateToCurrentLayer();
	void undo();
	void redo();

	void newLayer();
	Layer* getCurrentLayer() {
		return layers[selLayer];
	}
	void layer_flipHorizontally();
	void layer_flipVertically();
	void layer_swapLayerRGBtoBGR();
	uint32_t layer_getPixelAt(XY pos);
};

