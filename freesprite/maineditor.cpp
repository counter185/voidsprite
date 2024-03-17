#include "maineditor.h"
#include "FontRenderer.h"

MainEditor::MainEditor(XY dimensions) {
	SetUpWidgets();

	texW = dimensions.x;
	texH = dimensions.y;
	//canvasCenterPoint = XY{ texW / 2, texH / 2 };
	canvasCenterPoint = XY{ 0,0 };
	mainTexture = SDL_CreateTexture(g_rd, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, texW, texH);
	FillTexture();
}
MainEditor::MainEditor(std::string path) {
	SetUpWidgets();

	SDL_Surface* srf = IMG_Load(path.c_str());
	texW = srf->w;
	texH = srf->h;
	mainTexture = SDL_CreateTexture(g_rd, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, texW, texH);
	EnsureTextureLocked();
	SDL_ConvertPixels(srf->w, srf->h, srf->format->format, srf->pixels, srf->pitch, SDL_PIXELFORMAT_ARGB8888, lockedPixels, pitch);
	EnsureTextureUnlocked();
	canvasCenterPoint = XY{ 0,0 };
	SDL_FreeSurface(srf);
}

void MainEditor::render() {
	EnsureTextureUnlocked();
	SDL_Rect canvasRenderRect;
	canvasRenderRect.w = texW * scale;
	canvasRenderRect.h = texH * scale;
	canvasRenderRect.x = canvasCenterPoint.x;
	canvasRenderRect.y = canvasCenterPoint.y;

	SDL_RenderCopy(g_rd, mainTexture, NULL, &canvasRenderRect);

	g_fnt->RenderString(std::string("Scale: ") + std::to_string(scale), 0, 20);
	g_fnt->RenderString(std::string("MousePixelPoint: ") + std::to_string(mousePixelTargetPoint.x) + std::string(":") + std::to_string(mousePixelTargetPoint.y), 0, 50);

	wxsManager.renderAll();
}

void MainEditor::tick() {

}

void MainEditor::SetUpWidgets()
{
	colorPicker = new EditorColorPicker(this);
	colorPicker->position.y = 80;
	colorPicker->position.x = 10;
	wxsManager.addDrawable(colorPicker);
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
					if (evt.button.state) {
						RecalcMousePixelTargetPoint(evt.button.x, evt.button.y);
						SetPixel(mousePixelTargetPoint, 0xFF000000 | pickedColor);
						mouseHoldPosition = mousePixelTargetPoint;
					}
					leftMouseHold = evt.button.state;
				}
				else if (evt.button.button == 2) {
					middleMouseHold = evt.button.state;
				}
				break;
			case SDL_MOUSEMOTION:
				RecalcMousePixelTargetPoint(evt.motion.x, evt.motion.y);
				if (middleMouseHold) {
					canvasCenterPoint.x += evt.motion.xrel;
					canvasCenterPoint.y += evt.motion.yrel;
				}
				else if (leftMouseHold) {
					DrawLine(mouseHoldPosition, mousePixelTargetPoint, 0xFF000000 | pickedColor);
					mouseHoldPosition = mousePixelTargetPoint;
				}
				break;
			case SDL_MOUSEWHEEL:
				scale -= evt.wheel.y;
				scale = scale < 1 ? 1 : scale;
				break;
			case SDL_KEYDOWN:
				switch (evt.key.keysym.sym) {
					/*case SDLK_l:
						DrawLine(XY{ 20,5 }, XY{ 50, 35 }, 0xFF000000);
						break;*/
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

void MainEditor::FillTexture() {
	int* pixels;
	int pitch;
	SDL_LockTexture(mainTexture, NULL, (void**)&pixels, &pitch);
	for (int x = 0; x < texW; x++) {
		for (int y = 0; y < texH; y++) {
			pixels[x + (y * texW)] = 0xFFA0A0A0;
		}
	}
	SDL_UnlockTexture(mainTexture);
}

void MainEditor::EnsureTextureLocked() {
	if (!textureLocked) {
		SDL_LockTexture(mainTexture, NULL, (void**)&lockedPixels, &pitch);
		textureLocked = true;
	}
}

void MainEditor::EnsureTextureUnlocked() {
	if (textureLocked) {
		SDL_UnlockTexture(mainTexture);
		textureLocked = false;
	}
}

void MainEditor::SetPixel(XY position, uint32_t color) {
	if (position.x >= 0 && position.x < texW
		&& position.y >= 0 && position.y < texH) {
		EnsureTextureLocked();
		lockedPixels[position.x + (position.y * texW)] = color;
	}
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