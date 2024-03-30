#pragma once
#include "globals.h"
class Layer
{
public:
	uint8_t* pixelData;	//!!! THIS IS IN ARGB
	std::vector<uint8_t*> undoQueue;
	std::vector<uint8_t*> redoQueue;
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
	Layer(SDL_Surface* from) : Layer(from->w, from->h) {
		//pixelData = (uint8_t*)malloc(w * h * 4);
		if (from->format->format == SDL_PIXELFORMAT_ARGB8888) {
			memcpy(pixelData, from->pixels, w * h * 4);
		}
		else {
			SDL_ConvertPixels(w, h, from->format->format, from->pixels, from->pitch, SDL_PIXELFORMAT_ARGB8888, pixelData, w * 4);
			SDL_FreeSurface(from);
		}
	}

	~Layer() {
		free(pixelData);
		for (uint8_t*& u : undoQueue) {
			free(u);
		}
		for (uint8_t*& r : redoQueue) {
			free(r);
		}
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

	unsigned int getPixelAt(XY position) {
		if (position.x >= 0 && position.x < w
			&& position.y >= 0 && position.y < h) {
			uint32_t* intpxdata = (uint32_t*)pixelData;
			return intpxdata[position.x + (position.y * w)];
		}
		else {
			return 0xFF000000;
		}
	}

	void flipHorizontally() {
		uint32_t* px32 = (uint32_t*)pixelData;
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w / 2; x++) {
				uint32_t p = *(px32 + (y*w) + (w - 1 - x));
				*(px32 + (y*w) + (w - 1 - x)) = *(px32 + (y*w) + x);
				*(px32 + (y*w) + x) = p;
			}
		}
		layerDirty = true;
	}

	void discardRedoStack() {
		for (uint8_t*& redoD : redoQueue) {
			free(redoD);
		}
		redoQueue.clear();
	}
	void discardLastUndo() {
		free(undoQueue[0]);
		undoQueue.erase(undoQueue.begin());
	}
	void commitStateToUndoStack() {
		discardRedoStack();
		uint8_t* copiedPixelData = (uint8_t*)malloc(w * h * 4);
		if (copiedPixelData != NULL) {
			memcpy(copiedPixelData, pixelData, w * h * 4);
			undoQueue.push_back(copiedPixelData);
		}
		else {
			printf("malloc FAILED we are FUCKED\n");
		}
	}
	void undo() {
		if (!undoQueue.empty()) {
			redoQueue.push_back(pixelData);
			pixelData = undoQueue[undoQueue.size() - 1];
			undoQueue.pop_back();
			layerDirty = true;
		}
	}
	void redo() {
		if (!redoQueue.empty()) {
			undoQueue.push_back(pixelData);
			pixelData = redoQueue[redoQueue.size()-1];
			redoQueue.pop_back();
			layerDirty = true;
		}
	}
};
