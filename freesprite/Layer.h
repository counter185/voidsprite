#pragma once
#include "globals.h"
class Layer
{
public:
	uint8_t* pixelData;	//!!! THIS IS IN ARGB
	int w, h;
	SDL_Texture* tex;
	bool layerDirty = true;

	Layer(int width, int height) {
		w = width;
		h = height;
		pixelData = (uint8_t*)malloc(width * height * 4);
		tex = SDL_CreateTexture(g_rd, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);
		SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
	}

	~Layer() {
		free(pixelData);
		SDL_DestroyTexture(tex);
	}

	void updateTexture() {
		uint8_t* pixels;
		int pitch;
		SDL_LockTexture(tex, NULL, (void**)&pixels, &pitch);
		memcpy(pixels, pixelData, w * h * 4);
		SDL_UnlockTexture(tex);
		layerDirty = false;
	}

	void render(SDL_Rect where) {
		if (layerDirty) {
			updateTexture();
		}
		SDL_RenderCopy(g_rd, tex, NULL, &where);
	}

	void setPixel(XY position, uint32_t color) {
		uint32_t* intpxdata = (uint32_t*)pixelData;
		if (position.x >= 0 && position.x < w
			&& position.y >= 0 && position.y < h) {
			intpxdata[position.x + (position.y * w)] = color;
			layerDirty = true;
		}
	}

	void flipHorizontally() {
		uint32_t* px32 = (uint32_t*)pixelData;
		//px32[0] = 0xFFFFFFFF;
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w / 2; x++) {
				uint32_t p = *(px32 + (y*w) + (w - 1 - x));
				*(px32 + (y*w) + (w - 1 - x)) = *(px32 + (y*w) + x);
				*(px32 + (y*w) + x) = p;
			}
		}
		layerDirty = true;
	}
};

