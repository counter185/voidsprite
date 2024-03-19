#include "maineditor.h"
#include "FontRenderer.h"
#include "EditorBrushPicker.h"
#include "GlobalNavBar.h"

MainEditor::MainEditor(XY dimensions) {
	SetUpWidgets();

	texW = dimensions.x;
	texH = dimensions.y;
	//canvasCenterPoint = XY{ texW / 2, texH / 2 };
	canvasCenterPoint = XY{ 0,0 };
	imgLayer = new Layer(texW, texH);
	FillTexture();
}
MainEditor::MainEditor(SDL_Surface* srf) {
	SetUpWidgets();

	//todo i mean just use MainEditor(Layer*) here
	texW = srf->w;
	texH = srf->h;

	imgLayer = new Layer(texW, texH);
	SDL_ConvertPixels(srf->w, srf->h, srf->format->format, srf->pixels, srf->pitch, SDL_PIXELFORMAT_ARGB8888, imgLayer->pixelData, texW*4);
	canvasCenterPoint = XY{ 0,0 };
}

MainEditor::MainEditor(Layer* layer)
{
	SetUpWidgets();

	texW = layer->w;
	texH = layer->h;

	imgLayer = layer;
	canvasCenterPoint = XY{ 0,0 };
}

MainEditor::~MainEditor() {
	printf("hello from destructor\n");
	wxsManager.freeAllDrawables();
	delete imgLayer;
}

void MainEditor::render() {
	EnsureTextureUnlocked();
	DrawBackground();

	SDL_Rect canvasRenderRect;
	canvasRenderRect.w = texW * scale;
	canvasRenderRect.h = texH * scale;
	canvasRenderRect.x = canvasCenterPoint.x;
	canvasRenderRect.y = canvasCenterPoint.y;

	imgLayer->render(canvasRenderRect);

	//g_fnt->RenderString(std::string("Scale: ") + std::to_string(scale), 0, 20);
	//g_fnt->RenderString(std::string("MousePixelPoint: ") + std::to_string(mousePixelTargetPoint.x) + std::string(":") + std::to_string(mousePixelTargetPoint.y), 0, 50);

	DrawForeground();

	wxsManager.renderAll();
}

void MainEditor::tick() {
	canvasCenterPoint = XY{
		iclamp(-texW * scale + 4, canvasCenterPoint.x, g_windowW - 4),
		iclamp(-texH * scale +4, canvasCenterPoint.y, g_windowH - 4)
	};
}

void MainEditor::DrawBackground()
{
	int lineX = 400;
	for (int x = 40 + (SDL_GetTicks64()%5000/5000.0 * 60); x < g_windowW + lineX; x += 60) {
		SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x40);
		SDL_RenderDrawLine(g_rd, x, 0, x - lineX, g_windowH);
		SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x0d);
		SDL_RenderDrawLine(g_rd, g_windowW - x, 0, g_windowW - x + lineX/4*6, g_windowH);
	}


	int lw = texW * scale + 2;
	int lh = texH * scale + 2;
	SDL_Rect r = { canvasCenterPoint.x - 1, canvasCenterPoint.y - 1, lw, lh };
	uint8_t a = 0xff;
	SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, a);
	SDL_RenderDrawRect(g_rd, &r);
	for (int x = 0; x < 6; x++) {
		r.w += 2;
		r.h += 2;
		r.x -= 1;
		r.y -= 1;
		a /= 2;
		SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, a);
		SDL_RenderDrawRect(g_rd, &r);
	}
	
}

void MainEditor::DrawForeground()
{
	SDL_Rect r = { 0, g_windowH - 30, g_windowW, 30 };
	SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0xb0);
	SDL_RenderFillRect(g_rd, &r);

	g_fnt->RenderString(std::format("{}x{} ({}%)", texW, texH, scale * 100), 2, g_windowH - 28, SDL_Color{255,255,255,0xa0});

	g_fnt->RenderString(std::format("{}:{}", mousePixelTargetPoint.x, mousePixelTargetPoint.y), 200, g_windowH - 28, SDL_Color{255,255,255,0xd0});
}

void MainEditor::SetUpWidgets()
{
	GlobalNavBar* editorNavbar = new GlobalNavBar(this);
	wxsManager.addDrawable(editorNavbar);

	colorPicker = new EditorColorPicker(this);
	colorPicker->position.y = 80;
	colorPicker->position.x = 10;
	wxsManager.addDrawable(colorPicker);
	colorPicker->setMainEditorColorRGB(pickedColor);

	brushPicker = new EditorBrushPicker(this);
	brushPicker->position.y = 480;
	brushPicker->position.x = 10;
	wxsManager.addDrawable(brushPicker);
}

void MainEditor::RecalcMousePixelTargetPoint(int x, int y) {
	mousePixelTargetPoint =
		XY{
			(canvasCenterPoint.x - x) / -scale,
			(canvasCenterPoint.y - y) / -scale
	};
}

void MainEditor::takeInput(SDL_Event evt) {

	if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.button == 1 && evt.button.state) {
		wxsManager.tryFocusOnPoint(XY{ evt.button.x, evt.button.y });
	}

	if (!wxsManager.anyFocused()) {
		switch (evt.type) {
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				if (evt.button.button == 1) {
					RecalcMousePixelTargetPoint(evt.button.x, evt.button.y);
					if (currentBrush != NULL) {
						if (evt.button.state) {
							currentBrush->clickPress(this, mousePixelTargetPoint);
						}
						else {
							currentBrush->clickRelease(this, mousePixelTargetPoint);
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
					colorPicker->setMainEditorColorRGB(imgLayer->getPixelAt(mousePixelTargetPoint));
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
						currentBrush->clickDrag(this, mouseHoldPosition, mousePixelTargetPoint);
					}
					mouseHoldPosition = mousePixelTargetPoint;
				}
				break;
			case SDL_MOUSEWHEEL:
				scale += evt.wheel.y;
				scale = scale < 1 ? 1 : scale;
				break;
			case SDL_KEYDOWN:
				switch (evt.key.keysym.sym) {
					case SDLK_e:
						colorPicker->toggleEraser();
						break;
					case SDLK_RCTRL:
						middleMouseHold = !middleMouseHold;
						break;
				}
				break;
		}
	}
	else {
		wxsManager.passInputToFocused(evt);
	}
}

void MainEditor::eventFileSaved(int evt_id, std::string name)
{
	if (evt_id == EVENT_MAINEDITOR_SAVEFILE) {
		printf("eventFileSaved: got file name %s\n", name.c_str());
		
		SDL_Surface* nsrf = SDL_CreateRGBSurfaceWithFormat(0, imgLayer->w, imgLayer->h, 32, SDL_PIXELFORMAT_ARGB8888);
		memcpy(nsrf->pixels, imgLayer->pixelData, nsrf->w * nsrf->h * 4);
		IMG_SavePNG(nsrf, name.c_str());
		SDL_FreeSurface(nsrf);
	}
}

void MainEditor::FillTexture() {
	int* pixels = (int*)imgLayer->pixelData;
	//int pitch;
	//SDL_LockTexture(mainTexture, NULL, (void**)&pixels, &pitch);
	for (int x = 0; x < texW; x++) {
		for (int y = 0; y < texH; y++) {
			pixels[x + (y * texW)] = 0x00000000;
		}
	}
	//SDL_UnlockTexture(mainTexture);
}

void MainEditor::EnsureTextureLocked() {
	/*if (!textureLocked) {
		SDL_LockTexture(mainTexture, NULL, (void**)&lockedPixels, &pitch);
		textureLocked = true;
	}*/
}

void MainEditor::EnsureTextureUnlocked() {
	/*if (textureLocked) {
		SDL_UnlockTexture(mainTexture);
		textureLocked = false;
	}*/
}

void MainEditor::SetPixel(XY position, uint32_t color) {
	imgLayer->setPixel(position, color & (eraserMode ? 0xffffff : 0xffffffff));
}

void MainEditor::DrawLine(XY from, XY to, uint32_t color) {
	if (from.x == to.x) {
		int yMin = from.y > to.y ? to.y : from.y;
		int yMax = from.y > to.y ? from.y : to.y;
		for (int y = yMin; y <= yMax; y++) {
			SetPixel(XY{ from.x, y }, color);
		}
	}
	else {
		float a = (float)(from.y - to.y) / (from.x - to.x);
		float b = from.y - a * from.x;

		int xMin = from.x > to.x ? to.x : from.x;
		int xMax = from.x > to.x ? from.x : to.x;

		for (int x = xMin; x <= xMax; x++) {
			int yPos = (int)(a * x + b);
			SetPixel(XY{ x, yPos }, color);
		}
	}
}

void MainEditor::trySaveImage()
{
	platformTrySaveImageFile(this);
}
