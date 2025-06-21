#pragma once
#include "Layer.h"

//todo: rename it all to indexed
class LayerPalettized :
    public Layer
{
protected:
	LayerPalettized() : Layer() {}
public:
	std::vector<uint32_t> palette;

	//here, pixelData is supposed to be treated as an array of 32-bit indices.
	//-1 means empty (transparent)

	LayerPalettized(int width, int height) {
		w = width;
		h = height;
		allocMemory();
	}

	static LayerPalettized* tryAllocIndexedLayer(int width, int height) {
		if (width > 0 && height > 0) {
			LayerPalettized* ret = new LayerPalettized();
			ret->w = width;
			ret->h = height;
			if (ret->allocMemory()) {
				return ret;
			}
			else {
				delete ret;
				return NULL;
			}
		}
		return NULL;
	}

	void updateTexture() override {
		uint8_t* pixels;
		int pitch;
		if (tex[g_rd] == NULL || !xyEqual(texDimensions[g_rd], XY{w,h})) {
			if (tex[g_rd] != NULL) {
				tracked_destroyTexture(tex[g_rd]);
			}
            tex[g_rd] = tracked_createTexture(g_rd, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);
			texDimensions[g_rd] = XY{ w,h };
			layerDirty[g_rd] = true;
			SDL_SetTextureBlendMode(tex[g_rd], SDL_BLENDMODE_BLEND);
		}
		SDL_LockTexture(tex[g_rd], NULL, (void**)&pixels, &pitch);
		//memcpy(pixels, pixelData, w * h * 4);
		uint32_t* pxd = (uint32_t*)pixels;
		int32_t* pxd2 = (int32_t*)pixelData;

		for (uint64_t index = 0; index < w * h; index++) {
			if (pxd2[index] < 0 || pxd2[index] >= palette.size()) {
				pxd[index] = 0;
			}
			else {
				pxd[index] = palette[pxd2[index]];
			}
		}

		if (colorKeySet) {
			uint32_t* px32 = (uint32_t*)pixelData;
			for (uint64_t p = 0; p < w * h; p++) {
				if ((px32[p] & 0xffffff) == (colorKey & 0xFFFFFF)) {
					pixels[p * 4 + 3] = 0;
				}
			}
		}
		SDL_UnlockTexture(tex[g_rd]);
		layerDirty[g_rd] = false;
	}

	bool allocMemory() override {
		if (Layer::allocMemory()) {
			memset(pixelData, -1, w * h * 4);
			isPalettized = true;
			return true;
		}
		return false;
	}

	uint32_t getPixelAt(XY position, bool ignoreLayerAlpha = true) override {
		if (position.x >= 0 && position.x < w
			&& position.y >= 0 && position.y < h) {
			uint32_t* intpxdata = (uint32_t*)pixelData;
			uint32_t pixel = intpxdata[position.x + (position.y * w)];
			//uint8_t alpha = (((pixel >> 24) / 255.0f) * (ignoreLayerAlpha ? 1.0f : (layerAlpha / 255.0f))) * 255;
			//pixel = (pixel & 0x00ffffff) | (alpha << 24);
			return pixel;
		}
		else {
			return -1;
		}
	}

	uint32_t getVisualPixelAt(XY position, bool ignoreLayerAlpha = true) override {
		int32_t paletteIndex = getPixelAt(position, ignoreLayerAlpha);
		return (paletteIndex >= 0 && paletteIndex < palette.size()) ? palette[paletteIndex] : 0x00000000;
	}

	Layer* toRGB() {
		Layer* rgbLayer = new Layer(w, h);
		rgbLayer->name = name;
		rgbLayer->hidden = hidden;
		uint32_t* rgbData = (uint32_t*)rgbLayer->pixelData;
		int32_t* pxd2 = (int32_t*)pixelData;
		for (uint64_t index = 0; index < w * h; index++) {
			if (pxd2[index] == -1 || pxd2[index] >= palette.size()) {
				rgbData[index] = 0;
			}
			else {
				rgbData[index] = palette[pxd2[index]];
			}
		}
		return rgbLayer;
	}

	std::vector<uint32_t> getUniqueColors(bool onlyRGB) override {
		std::map<uint32_t, int> cols;
		uint32_t* pixels = (uint32_t*)pixelData;
		for (uint64_t x = 0; x < w * h; x++) {
			uint32_t px = pixels[x];
			cols[px] = 1;
		}
		std::vector<uint32_t> ret;
		for (auto& a : cols) {
			ret.push_back(a.first);
		}
		return ret;
	}
};

