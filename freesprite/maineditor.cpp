#include "maineditor.h"
#include "FontRenderer.h"
#include "EditorBrushPicker.h"
#include "EditorLayerPicker.h"
#include "ScreenWideNavBar.h"
#include "PopupYesNo.h"
#include "Notification.h"
#include "SpritesheetPreviewScreen.h"
#include "EditorSpritesheetPreview.h"
#include "FileIO.h"
#include "PopupTextBox.h"
#include "PopupSetColorKey.h"
#include "PopupSetEditorPixelGrid.h"
#include "TilemapPreviewScreen.h"
#include "MinecraftSkinPreviewScreen.h"

MainEditor::MainEditor(XY dimensions) {

	texW = dimensions.x;
	texH = dimensions.y;
	//canvasCenterPoint = XY{ texW / 2, texH / 2 };
	layers.push_back(new Layer(texW, texH));
	FillTexture();

	setUpWidgets();
	recenterCanvas();
	initLayers();
}
MainEditor::MainEditor(SDL_Surface* srf) {

	//todo i mean just use MainEditor(Layer*) here
	texW = srf->w;
	texH = srf->h;

	Layer* nlayer = new Layer(texW, texH);
	layers.push_back(nlayer);
	SDL_ConvertPixels(srf->w, srf->h, srf->format->format, srf->pixels, srf->pitch, SDL_PIXELFORMAT_ARGB8888, nlayer->pixelData, texW*4);

	setUpWidgets();
	recenterCanvas();
	initLayers();
}

MainEditor::MainEditor(Layer* layer)
{

	texW = layer->w;
	texH = layer->h;

	layers.push_back(layer);

	setUpWidgets();
	recenterCanvas();
	initLayers();
}

MainEditor::MainEditor(std::vector<Layer*> layers)
{
	texW = layers[0]->w;
	texH = layers[0]->h;
	this->layers = layers;

	setUpWidgets();
	recenterCanvas();
	initLayers();
	
}

MainEditor::~MainEditor() {
	//printf("hello from destructor\n");
	wxsManager.freeAllDrawables();
	for (Layer*& imgLayer : layers) {
		delete imgLayer;
	}
}

void MainEditor::render() {
	SDL_SetRenderDrawColor(g_rd, backgroundColor.r/6*5, backgroundColor.g/6*5, backgroundColor.b/6*5, 255);
	SDL_RenderClear(g_rd);
	DrawBackground();

	SDL_Rect canvasRenderRect;
	canvasRenderRect.w = texW * scale;
	canvasRenderRect.h = texH * scale;
	canvasRenderRect.x = canvasCenterPoint.x;
	canvasRenderRect.y = canvasCenterPoint.y;

	for (int x = 0; x < layers.size(); x++) {
		Layer* imgLayer = layers[x];
		if (!imgLayer->hidden) {
			imgLayer->render(canvasRenderRect, (layerSwitchTimer.started && x == selLayer) ? (uint8_t)(255 * XM1PW3P1(layerSwitchTimer.percentElapsedTime(1300))) : 255);
		}
	}

	//draw a separate 1x1 grid if the scale is >= 1600%
	if (scale >= 10) {

		uint8_t tileGridAlpha = scale < 16 ? 0x10 * ((scale - 9) / 7.0) : 0x10;

		int dx = canvasRenderRect.x;
		while (dx < g_windowW && dx < canvasRenderRect.x + canvasRenderRect.w) {
			dx += scale;
			if (dx >= 0) {
				SDL_SetRenderDrawColor(g_rd, 0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, tileGridAlpha);
				SDL_RenderDrawLine(g_rd, dx, canvasRenderRect.y, dx, canvasRenderRect.y + canvasRenderRect.h);
			}
		}
		int dy = canvasRenderRect.y;
		while (dy < g_windowH && dy < canvasRenderRect.y + canvasRenderRect.h) {
			dy += scale;
			if (dy >= 0) {
				SDL_SetRenderDrawColor(g_rd, 0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, tileGridAlpha);
				SDL_RenderDrawLine(g_rd, canvasRenderRect.x, dy, canvasRenderRect.x + canvasRenderRect.w, dy);
			}
		}
	}

	//draw tile lines
	if (tileDimensions.x != 0) {
		int dx = canvasRenderRect.x;
		while (dx < g_windowW && dx < canvasRenderRect.x + canvasRenderRect.w) {
			dx += tileDimensions.x * scale;
			if (dx >= 0) {
				SDL_SetRenderDrawColor(g_rd, 0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, tileGridAlpha);
				SDL_RenderDrawLine(g_rd, dx, canvasRenderRect.y, dx, canvasRenderRect.y + canvasRenderRect.h);
			}
		}
	}
	if (tileDimensions.y != 0) {
		int dy = canvasRenderRect.y;
		while (dy < g_windowH && dy < canvasRenderRect.y + canvasRenderRect.h) {
			dy += tileDimensions.y * scale;
			if (dy >= 0) {
				SDL_SetRenderDrawColor(g_rd, 0xff-backgroundColor.r, 0xff-backgroundColor.g, 0xff-backgroundColor.b, tileGridAlpha);
				SDL_RenderDrawLine(g_rd, canvasRenderRect.x, dy, canvasRenderRect.x + canvasRenderRect.w, dy);
			}
		}
	}
	drawSymmetryLines();
	renderComments();

	if (currentBrush != NULL) {
		currentBrush->renderOnCanvas(this, scale);
	}

	if (spritesheetPreview != NULL) {
		spritesheetPreview->previewWx->render(XY{ 0,0 });
	}

	//g_fnt->RenderString(std::string("Scale: ") + std::to_string(scale), 0, 20);
	//g_fnt->RenderString(std::string("MousePixelPoint: ") + std::to_string(mousePixelTargetPoint.x) + std::string(":") + std::to_string(mousePixelTargetPoint.y), 0, 50);

	

	if (wxsManager.anyFocused()) {
		SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
		SDL_RenderFillRect(g_rd, NULL);
	}

	DrawForeground();

	wxsManager.renderAll();

	if (eraserMode) {
		SDL_Rect eraserRect = { g_mouseX + 6, g_mouseY - 30, 28, 28 };
		SDL_SetTextureAlphaMod(g_iconEraser, 0xa0);
		SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x60);
		SDL_RenderFillRect(g_rd, &eraserRect);
		SDL_RenderCopy(g_rd, g_iconEraser, NULL, &eraserRect);
	}
}

void MainEditor::tick() {
	canvasCenterPoint = XY{
		iclamp(-texW * scale + 4, canvasCenterPoint.x, g_windowW - 4),
		iclamp(-texH * scale +4, canvasCenterPoint.y, g_windowH - 4)
	};

	//fuck it we ball
	layerPicker->position.x = g_windowW - 260;

	if (closeNextTick) {
		g_closeScreen(this);
	}
}

void MainEditor::DrawBackground()
{
	uint32_t colorBG1 = 0xFF000000 | (sdlcolorToUint32(backgroundColor) == 0xFF000000 ? 0x000000 : 0xDFDFDF);
	uint32_t colorBG2 = 0xFF000000 | (sdlcolorToUint32(backgroundColor) == 0xFF000000 ? 0x202020 : 0x808080);
	renderGradient({ 0,0, g_windowW, g_windowH }, colorBG1, colorBG1, colorBG1, colorBG2);

	int lineX = 400;
	for (int x = 40 + (SDL_GetTicks64()%5000/5000.0 * 60); x < g_windowW + lineX; x += 60) {
		SDL_SetRenderDrawColor(g_rd, 0xff-backgroundColor.r, 0xff-backgroundColor.g, 0xff-backgroundColor.b, 0x40);
		SDL_RenderDrawLine(g_rd, x, 0, x - lineX, g_windowH);
		SDL_SetRenderDrawColor(g_rd, 0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, 0x0d);
		SDL_RenderDrawLine(g_rd, g_windowW - x, 0, g_windowW - x + lineX/4*6, g_windowH);
	}


	int lw = texW * scale + 2;
	int lh = texH * scale + 2;
	SDL_Rect r = { canvasCenterPoint.x - 1, canvasCenterPoint.y - 1, lw, lh };
	uint8_t a = 0xff;
	SDL_SetRenderDrawColor(g_rd, 0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, a);
	SDL_RenderDrawRect(g_rd, &r);
	for (int x = 0; x < 6; x++) {
		r.w += 2;
		r.h += 2;
		r.x -= 1;
		r.y -= 1;
		a /= 2;
		SDL_SetRenderDrawColor(g_rd, 0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, a);
		SDL_RenderDrawRect(g_rd, &r);
	}
	
}

void MainEditor::drawSymmetryLines() {
	if (symmetryEnabled[0]) {
		int symXPos = symmetryPositions.x / 2;
		bool symXMiddle = symmetryPositions.x % 2;
		int lineDrawXPoint = canvasCenterPoint.x + symXPos * scale + (symXMiddle ? scale/2 : 0);

		SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x80);
		SDL_RenderDrawLine(g_rd, lineDrawXPoint, 0, lineDrawXPoint, g_windowH);
	}
	if (symmetryEnabled[1]) {
		int symYPos = symmetryPositions.y / 2;
		bool symYMiddle = symmetryPositions.y % 2;
		int lineDrawYPoint = canvasCenterPoint.y + symYPos * scale + (symYMiddle ? scale/2 : 0);

		SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x80);
		SDL_RenderDrawLine(g_rd,0, lineDrawYPoint, g_windowW, lineDrawYPoint);
	}
}

void MainEditor::DrawForeground()
{
	SDL_Rect r = { 0, g_windowH - 30, g_windowW, 30 };
	SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0xb0);
	SDL_RenderFillRect(g_rd, &r);

	g_fnt->RenderString(std::format("{}x{} ({}%)", texW, texH, scale * 100), 2, g_windowH - 28, SDL_Color{255,255,255,0xa0});

	g_fnt->RenderString(std::format("{}:{}", mousePixelTargetPoint.x, mousePixelTargetPoint.y), 200, g_windowH - 28, SDL_Color{255,255,255,0xd0});

	if (currentBrush != NULL) {
		g_fnt->RenderString(std::format("{} {}", currentBrush->getName(), eraserMode ? "(Erase)" : ""), 350, g_windowH - 28, SDL_Color{ 255,255,255,0xa0 });
	}

	if (currentPattern != NULL) {
		g_fnt->RenderString(std::format("{}", currentPattern->getName()), 600, g_windowH - 28, SDL_Color{ 255,255,255,0xa0 });
	}
}

void MainEditor::renderComments()
{
	XY origin = canvasCenterPoint;
	for (CommentData& c : comments) {
		XY onScreenPosition = xyAdd(origin, { c.position.x * scale, c.position.y * scale });
		SDL_Rect iconRect = { onScreenPosition.x, onScreenPosition.y, 16, 16 };
		SDL_SetTextureAlphaMod(g_iconComment, 0x80);
		SDL_RenderCopy(g_rd, g_iconComment, NULL, &iconRect);
		if (xyDistance(onScreenPosition, XY{ g_mouseX, g_mouseY }) < 32) {
			if (!c.hovered) {
				c.animTimer.start();
				c.hovered = true;
			}
			int yOffset = 16 * (1.0f- XM1PW3P1(c.animTimer.percentElapsedTime(200)));
			g_fnt->RenderString(c.data, onScreenPosition.x + 17, onScreenPosition.y - yOffset, SDL_Color{ 255,255,255, (uint8_t)(0xff * c.animTimer.percentElapsedTime(200))});
		} else {
			c.hovered = false;
		}
	}
}

void MainEditor::initLayers()
{
	for (Layer*& l : layers) {
		l->commitStateToUndoStack();
	}

	layerPicker->updateLayers();
}

void MainEditor::setUpWidgets()
{
	mainEditorKeyActions = {
		{
			SDLK_f,
			{
				"File",
				{SDLK_s, SDLK_d, SDLK_c},
				{
					{SDLK_d, { "Save as",
							[](MainEditor* editor) {
								editor->trySaveAsImage();
							}
						}
					},
					{SDLK_s, { "Save",
							[](MainEditor* editor) {
								editor->trySaveImage();
							}
						}
					},
					{SDLK_c, { "Close",
							[](MainEditor* editor) {
								editor->requestSafeClose();
							}
						}
					},
				},
				g_iconNavbarTabFile
			}
		},
		{
			SDLK_e,
			{
				"Edit",
				{SDLK_z, SDLK_r, SDLK_x, SDLK_y},
				{
					{SDLK_z, { "Undo",
							[](MainEditor* editor) {
								editor->undo();
							}
						}
					},
					{SDLK_r, { "Redo",
							[](MainEditor* editor) {
								editor->redo();
							}
						}
					},
					{SDLK_x, { "Toggle symmetry: X",
							[](MainEditor* editor) {
								editor->symmetryEnabled[0] = !editor->symmetryEnabled[0];
							}
						}
					},
					{SDLK_y, { "Toggle symmetry: Y",
							[](MainEditor* editor) {
								editor->symmetryEnabled[1] = !editor->symmetryEnabled[1];
							}
						}
					},
				},
				g_iconNavbarTabEdit
			}
		},
		{
			SDLK_l,
			{
				"Layer",
				{},
				{
					{SDLK_f, { "Flip current layer: X axis",
							[](MainEditor* editor) {
								editor->layer_flipHorizontally();
							}
						}
					},
					{SDLK_g, { "Flip current layer: Y axis",
							[](MainEditor* editor) {
								editor->layer_flipVertically();
							}
						}
					},
					{SDLK_b, { "Swap channels RGB->BGR",
							[](MainEditor* editor) {
								editor->layer_swapLayerRGBtoBGR();
							}
						}
					},
					{SDLK_x, { "Print number of colors",
							[](MainEditor* editor) {
								g_addNotification(Notification("", std::format("{} colors in current layer", editor->getCurrentLayer()->numUniqueColors(true))));
							}
						}
					},
					{SDLK_r, { "Rename current layer",
							[](MainEditor* editor) {
								editor->layer_promptRename();
							}
						}
					},
					{SDLK_a, { "Remove alpha channel",
							[](MainEditor* editor) {
								editor->layer_setAllAlpha255();
							}
						}
					},
					{SDLK_k, { "Set color key",
							[](MainEditor* editor) {
								g_addPopup(new PopupSetColorKey(editor->getCurrentLayer(), "Set color key", "Set the layer's color key:"));
							}
						}
					},
				},
				g_iconNavbarTabLayer
			}
		},
		{
			SDLK_v,
			{
				"View",
				{},
				{
					{SDLK_r, { "Recenter canvas",
							[](MainEditor* editor) {
								editor->recenterCanvas();
							}
						}
					},
					{SDLK_b, { "Toggle background color",
							[](MainEditor* editor) {
								editor->backgroundColor.r = ~editor->backgroundColor.r;
								editor->backgroundColor.g = ~editor->backgroundColor.g;
								editor->backgroundColor.b = ~editor->backgroundColor.b;
							}
						}
					},
					{SDLK_g, { "Set pixel grid...",
							[](MainEditor* editor) {
								g_addPopup(new PopupSetEditorPixelGrid(editor, "Set pixel grid", "Enter grid size <w>x<h>:"));
							}
						}
					},
					{SDLK_s, { "Open spritesheet preview...",
							[](MainEditor* editor) {
								if (editor->spritesheetPreview == NULL) {
									if (editor->tileDimensions.x == 0 || editor->tileDimensions.y == 0) {
										g_addNotification(Notification("Error", "Set the pixel grid first."));
										return;
									}
									SpritesheetPreviewScreen* newScreen = new SpritesheetPreviewScreen(editor);
									g_addScreen(newScreen);
									editor->spritesheetPreview = newScreen;
								}
								else {
									g_addNotification(Notification("Error", "Spritesheet preview is already open."));
								}
							}
						}
					},
					{SDLK_t, { "Open tileset preview...",
							[](MainEditor* editor) {
								if (editor->tileDimensions.x == 0 || editor->tileDimensions.y == 0) {
									g_addNotification(Notification("Error", "Set the pixel grid first."));
									return;
								}
								TilemapPreviewScreen* newScreen = new TilemapPreviewScreen(editor);
								g_addScreen(newScreen);
								//editor->spritesheetPreview = newScreen;
							}
						}
					},
	#if _DEBUG
					{SDLK_m, { "Open Minecraft skin preview...",
							[](MainEditor* editor) {
								if (editor->texW != editor->texH && editor->texW / 2 != editor->texH) {
									g_addNotification(Notification("Error", "Invalid size. Aspect must be 1:1 or 2:1."));
									return;
								}
								MinecraftSkinPreviewScreen* newScreen = new MinecraftSkinPreviewScreen(editor);
								g_addScreen(newScreen);
								//editor->spritesheetPreview = newScreen;
							}
						}
					},
	#endif
				},
				g_iconNavbarTabView
			}
		}
	};

	currentBrush = g_brushes[0];
	currentPattern = g_patterns[0];

	colorPicker = new EditorColorPicker(this);
	colorPicker->position.y = 80;
	colorPicker->position.x = 10;
	wxsManager.addDrawable(colorPicker);
	colorPicker->setMainEditorColorRGB(pickedColor);
	regenerateLastColors();

	brushPicker = new EditorBrushPicker(this);
	brushPicker->position.y = 480;
	brushPicker->position.x = 10;
	wxsManager.addDrawable(brushPicker);

	layerPicker = new EditorLayerPicker(this);
	layerPicker->position = XY{ 440, 80 };
	layerPicker->anchor = XY{ 1,0 };
	wxsManager.addDrawable(layerPicker);

	navbar = new ScreenWideNavBar<MainEditor*>(this, mainEditorKeyActions, { SDLK_f, SDLK_e, SDLK_l, SDLK_v });
	wxsManager.addDrawable(navbar);
}

void MainEditor::RecalcMousePixelTargetPoint(int x, int y) {
	mousePixelTargetPoint =
		XY{
			(canvasCenterPoint.x - x) / -scale,
			(canvasCenterPoint.y - y) / -scale
		};
	mousePixelTargetPoint2xP =
		XY{
			(int)((canvasCenterPoint.x - x) / (float)(-scale) / 0.5f),
			(int)((canvasCenterPoint.y - y) / (float)(-scale) / 0.5f)
	};
}

bool MainEditor::requestSafeClose() {
	if (!changesSinceLastSave) {
		closeNextTick = true;
		return true;
	}
	else {
		PopupYesNo* newPopup = new PopupYesNo("Close the project?", "You have unsaved changes! ");
		newPopup->setCallbackListener(EVENT_MAINEDITOR_CONFIRM_CLOSE, this);
		g_addPopup(newPopup);
	}
	return false;
}

void MainEditor::zoom(int how_much)
{
	XY screenCenterPoint = XY{
			(canvasCenterPoint.x - g_windowW/2) / -scale,
			(canvasCenterPoint.y - g_windowH/2) / -scale
	};
	scale += how_much;
	scale = scale < 1 ? 1 : scale;
	XY onscreenPointNow = XY{
		canvasCenterPoint.x + screenCenterPoint.x * scale,
		canvasCenterPoint.y + screenCenterPoint.y * scale
	};
	XY pointDiff = xySubtract(XY{ g_windowW / 2, g_windowH / 2 }, onscreenPointNow);
	canvasCenterPoint = xyAdd(canvasCenterPoint, pointDiff);
}

bool MainEditor::isInBounds(XY pos)
{
	return pos.x >= 0 && pos.x < texW && pos.y >= 0 && pos.y < texH;
}

void MainEditor::takeInput(SDL_Event evt) {

	if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.state) {
		wxsManager.tryFocusOnPoint(XY{ evt.button.x, evt.button.y });
	}
	if (evt.type == SDL_QUIT) {
		if (requestSafeClose()) {
			return;
		}
	}
	if (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_LALT) {
		wxsManager.forceFocusOn(navbar);
		return;
	}

	if (!wxsManager.anyFocused()) {
		switch (evt.type) {
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				if (evt.button.button == 1) {
					RecalcMousePixelTargetPoint(evt.button.x, evt.button.y);
					if (currentBrush != NULL) {
						if (evt.button.state) {
							if (!currentBrush->isReadOnly()) {
								commitStateToCurrentLayer();
							}
							currentBrush->clickPress(this, currentBrush->wantDoublePosPrecision() ? mousePixelTargetPoint2xP : mousePixelTargetPoint);
						}
						else {
							currentBrush->clickRelease(this, currentBrush->wantDoublePosPrecision() ? mousePixelTargetPoint2xP : mousePixelTargetPoint);
						}
					}
					mouseHoldPosition = mousePixelTargetPoint;
					leftMouseHold = evt.button.state;
				}
				else if (evt.button.button == 2) {
					middleMouseHold = evt.button.state;
				}
				else if (evt.button.button == 3) {
					RecalcMousePixelTargetPoint(evt.button.x, evt.button.y);
					if (currentBrush != NULL && currentBrush->overrideRightClick()) {
						if (evt.button.state) {
							currentBrush->rightClickPress(this, currentBrush->wantDoublePosPrecision() ? mousePixelTargetPoint2xP : mousePixelTargetPoint);
						}
					}
					else {
						colorPicker->setMainEditorColorRGB(g_ctrlModifier ? getCurrentLayer()->getPixelAt(mousePixelTargetPoint) : pickColorFromAllLayers(mousePixelTargetPoint));
					}
				}
				break;
			case SDL_MOUSEMOTION:
				RecalcMousePixelTargetPoint(evt.motion.x, evt.motion.y);
				if (middleMouseHold) {
					canvasCenterPoint.x += evt.motion.xrel;
					canvasCenterPoint.y += evt.motion.yrel;
				}
				else if (leftMouseHold) {
					if (currentBrush != NULL) {
						currentBrush->clickDrag(this, mouseHoldPosition, currentBrush->wantDoublePosPrecision() ? mousePixelTargetPoint2xP : mousePixelTargetPoint);
					}
					mouseHoldPosition = mousePixelTargetPoint;
				}
				if (currentBrush != NULL) {
					currentBrush->mouseMotion(this, currentBrush->wantDoublePosPrecision() ? mousePixelTargetPoint2xP : mousePixelTargetPoint);
				}
				break;
			case SDL_MOUSEWHEEL:
				if (g_ctrlModifier) {
					colorPicker->setMainEditorColorHSV(colorPicker->currentH, fxmin(fxmax(colorPicker->currentS + 0.1 * evt.wheel.y, 0), 1), colorPicker->currentV);
				}
				else if (g_shiftModifier) {
					double newH = dxmin(dxmax(colorPicker->currentH + (360.0 / 12) * evt.wheel.y, 0), 359);
					colorPicker->setMainEditorColorHSV(newH, colorPicker->currentS, colorPicker->currentV);
				}
				else {
					zoom(evt.wheel.y);
				}
				break;
			case SDL_KEYDOWN:
				switch (evt.key.keysym.sym) {
					case SDLK_e:
						colorPicker->eraserButton->click();
						//colorPicker->toggleEraser();
						break;
					case SDLK_RCTRL:
						middleMouseHold = !middleMouseHold;
						break;
					case SDLK_z:
						if (g_ctrlModifier) {
							if (g_shiftModifier) {
								redo();
							}
							else {
								undo();
							}
						}
						break;
					case SDLK_y:
						if (g_ctrlModifier) {
							redo();
						}
						break;
					case SDLK_s:
						if (g_ctrlModifier) {
							if (g_shiftModifier) {
								trySaveAsImage();
							}
							else {
								trySaveImage();
							}
						}
						break;
					case SDLK_F2:
						layer_promptRename();
						break;
				}
				break;
		}
	}
	else {
		wxsManager.passInputToFocused(evt);
	}
}

void MainEditor::eventFileSaved(int evt_id, PlatformNativePathString name, int exporterID)
{
	if (evt_id == EVENT_MAINEDITOR_SAVEFILE) {
		exporterID--;
		printf("eventFileSaved: got file name %ls\n", name.c_str());

		bool result = false;
		if (exporterID >= g_fileExportersMLNPaths.size()) {
			FileExportFlatNPath f = g_fileExportersFlatNPaths[exporterID - g_fileExportersMLNPaths.size()];
			Layer* flat = flattenImage();
			result = f.exportFunction(name, flat);
			delete flat;
		}
		else {
			FileExportMultiLayerNPath f = g_fileExportersMLNPaths[exporterID];
			result = f.exportFunction(name, this);
		}

		if (result) {
			//g_addPopup(new PopupMessageBox("File saved", "Save successful!"));
			lastConfirmedSave = true;
			lastConfirmedSavePath = name;
			lastConfirmedExporterId = exporterID;
			changesSinceLastSave = false;
			if (lastWasSaveAs) {
				platformOpenFileLocation(lastConfirmedSavePath);
			}
			g_addNotification(Notification("File saved", "Save successful!", 4000));
		}
		else {
			//g_addPopup(new PopupMessageBox("File not saved", "Save failed!"));
			g_addNotification(Notification("File not saved", "Save failed!", 6000, NULL, COLOR_ERROR));
		}
	}
}

void MainEditor::eventPopupClosed(int evt_id, BasePopup* p)
{
	if (evt_id == EVENT_MAINEDITOR_CONFIRM_CLOSE) {
		if (((PopupYesNo*)p)->result) {
			g_closeScreen(this);
		}
	}
}

void MainEditor::eventTextInputConfirm(int evt_id, std::string text)
{
	if (evt_id == EVENT_MAINEDITOR_SET_CURRENT_LAYER_NAME) {
		getCurrentLayer()->name = text;
		layerPicker->updateLayers();
	}
}

void MainEditor::FillTexture() {
	int* pixels = (int*)getCurrentLayer()->pixelData;
	//int pitch;
	//SDL_LockTexture(mainTexture, NULL, (void**)&pixels, &pitch);
	for (int x = 0; x < texW; x++) {
		for (int y = 0; y < texH; y++) {
			pixels[x + (y * texW)] = 0x00000000;
		}
	}
	//SDL_UnlockTexture(mainTexture);
}

void MainEditor::SetPixel(XY position, uint32_t color, uint8_t symmetry) {
	if (currentPattern->canDrawAt(position) && (!replaceAlphaMode || (replaceAlphaMode && ((layer_getPixelAt(position) & 0xFF000000) != 0)))) {
		getCurrentLayer()->setPixel(position, color & (eraserMode ? 0xffffff : 0xffffffff));
		colorPicker->pushLastColor(color);
	}
	if (symmetryEnabled[0] && !(symmetry & 0b10)) {
		int symmetryXPoint = symmetryPositions.x / 2;
		bool symXPointIsCentered = symmetryPositions.x % 2;
		int symmetryFlippedX = symmetryXPoint + (symmetryXPoint - position.x) - (symXPointIsCentered ? 0 : 1);
		SetPixel(XY{symmetryFlippedX, position.y}, color, symmetry | 0b10);
	}
	if (symmetryEnabled[1] && !(symmetry & 0b1)) {
		int symmetryYPoint = symmetryPositions.y / 2;
		bool symYPointIsCentered = symmetryPositions.y % 2;
		int symmetryFlippedY = symmetryYPoint + (symmetryYPoint - position.y) - (symYPointIsCentered ? 0 : 1);
		SetPixel(XY{position.x, symmetryFlippedY}, color, symmetry | 0b1);
	}
}

void MainEditor::DrawLine(XY from, XY to, uint32_t color) {
	rasterizeLine(from, to, [&](XY a)->void {
		SetPixel(a, color);
		});
}

void MainEditor::trySaveImage()
{
	lastWasSaveAs = false;
	if (!lastConfirmedSave) {
		trySaveAsImage();
	}
	else {
		eventFileSaved(EVENT_MAINEDITOR_SAVEFILE, lastConfirmedSavePath, lastConfirmedExporterId+1);
	}
	
}

void MainEditor::trySaveAsImage()
{
	lastWasSaveAs = true;
	platformTrySaveImageFile(this);
}

void MainEditor::recenterCanvas()
{
	canvasCenterPoint = XY{
		(g_windowW / 2) - (texW*scale)/2,
		(g_windowH / 2) - (texH*scale)/2
	};
}

void MainEditor::checkAndDiscardEndOfUndoStack()
{
	if (undoStack.size() > maxUndoHistory) {
		UndoStackElement l = undoStack[0];
		switch (l.type) {
			case UNDOSTACK_LAYER_DATA_MODIFIED:
				l.targetlayer->discardLastUndo();
				break;
			case UNDOSTACK_DELETE_LAYER:
				delete l.targetlayer;
				break;
		}
		
		undoStack.erase(undoStack.begin());
	}
}

void MainEditor::commitStateToLayer(Layer* l)
{
	l->commitStateToUndoStack();
	addToUndoStack(UndoStackElement{ l, UNDOSTACK_LAYER_DATA_MODIFIED });
}

void MainEditor::commitStateToCurrentLayer()
{
	commitStateToLayer(getCurrentLayer());
}

uint32_t MainEditor::pickColorFromAllLayers(XY pos)
{
	uint32_t c = 0;
	for (int x = layers.size() - 1; x >= 0; x--) {
		if (layers[x]->hidden) {
			continue;
		}
		uint32_t nextC = layers[x]->getPixelAt(pos);
		if ((c & 0xff000000) == 0 && (nextC & 0xff000000) == (0xff<<24)) {
			return nextC;
		}
		else {
			c = alphaBlend(nextC, c);
		}
	}
	return c;
}

void MainEditor::addToUndoStack(UndoStackElement undo)
{
	discardRedoStack();
	undoStack.push_back(undo);
	checkAndDiscardEndOfUndoStack();
	changesSinceLastSave = true;
}

void MainEditor::discardRedoStack()
{
	for (Layer*& x : layers) {
		x->discardRedoStack();
	}

	//clear redo stack
	for (UndoStackElement& l : redoStack) {
		if (l.type == UNDOSTACK_CREATE_LAYER) {
			delete l.targetlayer;
		}
	}
	redoStack.clear();
}

void MainEditor::undo()
{
	if (!undoStack.empty()) {
		UndoStackElement l = undoStack[undoStack.size() - 1];
		undoStack.pop_back();
		redoStack.push_back(l);
		switch (l.type) {
			case UNDOSTACK_LAYER_DATA_MODIFIED:
				l.targetlayer->undo();
				break;
			case UNDOSTACK_CREATE_LAYER:
				//remove layer from list
				for (int x = 0; x < layers.size(); x++) {
					if (layers[x] == l.targetlayer) {
						layers.erase(layers.begin() + x);
						break;
					}
				}
				if (selLayer >= layers.size()) {
					switchActiveLayer(layers.size() - 1);
				}
				layerPicker->updateLayers();
				break;
			case UNDOSTACK_DELETE_LAYER:
				//add layer to list
				layers.insert(layers.begin() + l.extdata, l.targetlayer);
				layerPicker->updateLayers();
				break;
			case UNDOSTACK_MOVE_LAYER:
			{
				Layer* lr = layers[l.extdata2];
				layers.erase(layers.begin() + l.extdata2);
				layers.insert(layers.begin() + l.extdata, lr);
				layerPicker->updateLayers();
			}
				break;
			case UNDOSTACK_ADD_COMMENT:
				_removeCommentAt({ l.extdata, l.extdata2 });
				break;
			case UNDOSTACK_REMOVE_COMMENT:
				comments.push_back(CommentData{ {l.extdata, l.extdata2}, l.extdata3 });
				break;
		}
	}
}

void MainEditor::redo()
{
	if (!redoStack.empty()) {
		UndoStackElement l = redoStack[redoStack.size() - 1];
		undoStack.push_back(l);
		redoStack.pop_back();
		switch (l.type) {
		case UNDOSTACK_LAYER_DATA_MODIFIED:
			l.targetlayer->redo();
			break;
		case UNDOSTACK_CREATE_LAYER:
			//add layer back to list
			layers.push_back(l.targetlayer);
			layerPicker->updateLayers();
			break;
		case UNDOSTACK_DELETE_LAYER:
			//add layer to list
			for (int x = 0; x < layers.size(); x++) {
				if (layers[x] == l.targetlayer) {
					layers.erase(layers.begin() + x);
					break;
				}
			}
			if (selLayer >= layers.size()) {
				switchActiveLayer(layers.size() - 1);
			}
			layerPicker->updateLayers();
			break;
		case UNDOSTACK_MOVE_LAYER:
		{
			Layer* lr = layers[l.extdata];
			layers.erase(layers.begin() + l.extdata);
			layers.insert(layers.begin() + l.extdata2, lr);
			layerPicker->updateLayers();
		}
			break;
		case UNDOSTACK_ADD_COMMENT:
			comments.push_back(CommentData{ {l.extdata, l.extdata2}, l.extdata3 });
			break;
		case UNDOSTACK_REMOVE_COMMENT:
			_removeCommentAt({ l.extdata, l.extdata2 });
			break;
		}
	}
}

Layer* MainEditor::newLayer()
{
	Layer* nl = new Layer(texW, texH);
	nl->name = std::format("New Layer {}", layers.size()+1);
	layers.push_back(nl);
	switchActiveLayer(layers.size() - 1);

	addToUndoStack(UndoStackElement{nl, UNDOSTACK_CREATE_LAYER});
	return nl;
}

void MainEditor::deleteLayer(int index) {
	if (layers.size() <= 1) {
		return;
	}

	Layer* layerAtPos = layers[index];
	layers.erase(layers.begin() + index);
	if (selLayer >= layers.size()) {
		switchActiveLayer(layers.size() - 1);
	}

	addToUndoStack(UndoStackElement{ layerAtPos, UNDOSTACK_DELETE_LAYER, index });
}

void MainEditor::regenerateLastColors()
{
	colorPicker->lastColors.clear();
	colorPicker->lastColorsChanged = true;
	Layer* flatLayer = flattenImage();
	auto colorPalette = flatLayer->get256MostUsedColors();
	delete flatLayer;
	for (auto& c : colorPalette) {
		colorPicker->pushLastColor(c);
	}
}

void MainEditor::moveLayerUp(int index) {
	if (index >= layers.size()-1) {
		return;
	}

	Layer* clayer = layers[index];
	layers.erase(layers.begin() + index);
	layers.insert(layers.begin() + index + 1, clayer);

	if (index == selLayer) {
		switchActiveLayer(selLayer + 1);
	}

	addToUndoStack(UndoStackElement{ clayer, UNDOSTACK_MOVE_LAYER, index, index + 1 });
}

void MainEditor::moveLayerDown(int index) {
	if (index  <= 0) {
		return;
	}

	Layer* clayer = layers[index];
	layers.erase(layers.begin() + index);
	layers.insert(layers.begin() + index - 1, clayer);

	if (index == selLayer) {
		switchActiveLayer(selLayer-1);
	}

	addToUndoStack(UndoStackElement{ clayer, UNDOSTACK_MOVE_LAYER, index, index - 1 });
}

void MainEditor::mergeLayerDown(int index)
{
	if (index == 0) {
		return;
	}
	Layer* topLayer = layers[index];
	Layer* bottomLayer = layers[index - 1];
	deleteLayer(index);
	commitStateToLayer(bottomLayer);
	Layer* merged = mergeLayers(bottomLayer, topLayer);
	memcpy(bottomLayer->pixelData, merged->pixelData, bottomLayer->w * bottomLayer->h * 4);
	bottomLayer->layerDirty = true;
	delete merged;
	
}

void MainEditor::duplicateLayer(int index)
{
	Layer* currentLayer = layers[index];
	Layer* newL = newLayer();
	memcpy(newL->pixelData, currentLayer->pixelData, texW * texH * 4);
	newL->name = "Copy:" + currentLayer->name;
	newL->layerDirty = true;
}

void MainEditor::layer_flipHorizontally()
{
	commitStateToCurrentLayer();
	getCurrentLayer()->flipHorizontally();
}
void MainEditor::layer_flipVertically()
{
	commitStateToCurrentLayer();
	getCurrentLayer()->flipVertically();
}

void MainEditor::layer_swapLayerRGBtoBGR()
{
	commitStateToCurrentLayer();
	Layer* clayer = getCurrentLayer();
	uint8_t* convData = (uint8_t*)malloc(clayer->w * clayer->h * 4);
	SDL_ConvertPixels(clayer->w, clayer->h, SDL_PIXELFORMAT_ARGB8888, clayer->pixelData, clayer->w * 4, SDL_PIXELFORMAT_ABGR8888, convData, clayer->w * 4);
	free(clayer->pixelData);
	clayer->pixelData = convData;
	clayer->layerDirty = true;
}

void MainEditor::layer_promptRename()
{
	PopupTextBox* ninput = new PopupTextBox("Rename layer", "Enter the new layer name:");
	ninput->setCallbackListener(EVENT_MAINEDITOR_SET_CURRENT_LAYER_NAME, this);
	ninput->tbox->text = this->getCurrentLayer()->name;
	g_addPopup(ninput);
}

uint32_t MainEditor::layer_getPixelAt(XY pos)
{
	return getCurrentLayer()->getPixelAt(pos);
}

void MainEditor::switchActiveLayer(int index)
{
	selLayer = index;
	layerSwitchTimer.start();
}

void MainEditor::layer_setAllAlpha255()
{
	commitStateToCurrentLayer();
	getCurrentLayer()->setAllAlpha255();
}

Layer* MainEditor::flattenImage()
{
	Layer* ret = new Layer(texW, texH);
	int x = 0;
	for (Layer*& l : layers) {
		if (l->hidden) {
			continue;
		}
		if (x++ == 0) {
			memcpy(ret->pixelData, l->pixelData, l->w * l->h * 4);
		}
		else {
			uint32_t* ppx = (uint32_t*)l->pixelData;
			uint32_t* retppx = (uint32_t*)ret->pixelData;
			for (uint64_t p = 0; p < l->w * l->h; p++) {
				uint32_t pixel = ppx[p];
				uint32_t srcPixel = retppx[p];
				retppx[p] = alphaBlend(srcPixel, pixel);
			}
		}
	}
	return ret;
}

Layer* MainEditor::mergeLayers(Layer* bottom, Layer* top)
{
	Layer* ret = new Layer(bottom->w, bottom->h);

	memcpy(ret->pixelData, bottom->pixelData, bottom->w * bottom->h * 4);

	uint32_t* ppx = (uint32_t*)top->pixelData;
	uint32_t* retppx = (uint32_t*)ret->pixelData;
	for (uint64_t p = 0; p < ret->w * ret->h; p++) {
		uint32_t pixel = ppx[p];
		uint32_t srcPixel = retppx[p];
		retppx[p] = alphaBlend(srcPixel, pixel);
	}

	return ret;
}

bool MainEditor::canAddCommentAt(XY a)
{
	for (CommentData& c : comments) {
		if (xyEqual(c.position, a)) {
			return false;
		}
	}
	return true;
}

void MainEditor::addCommentAt(XY a, std::string c)
{
	if (canAddCommentAt(a)) {
		addToUndoStack(UndoStackElement{ NULL, UNDOSTACK_ADD_COMMENT, a.x, a.y, c });

		CommentData newComment = { a, c };
		comments.push_back(newComment);
	}
}

void MainEditor::removeCommentAt(XY a)
{
	CommentData c = _removeCommentAt(a);
	if (c.data[0] != '\1') {

		addToUndoStack(UndoStackElement{ NULL, UNDOSTACK_REMOVE_COMMENT, a.x, a.y, c.data });
	}
}

CommentData MainEditor::_removeCommentAt(XY a)
{
	for (int x = 0; x < comments.size(); x++) {
		if (xyEqual(comments[x].position, a)) {
			CommentData c = comments[x];
			comments.erase(comments.begin() + x);
			return c;
		}
	}
	printf("_removeComment NOT FOUND\n");
	//shitass workaround tell noone thanks
	//@hirano185 hey girlie check this out!
	return { {0,0}, "\1" };
}

void MainEditor::layer_replaceColor(uint32_t from, uint32_t to)
{
	//commitStateToCurrentLayer();
	getCurrentLayer()->replaceColor(from, to);
}
