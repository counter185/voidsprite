#pragma once
#include "globals.h"


class RenderObject {
public:
	bool valid;
	SDL_Texture* tx = NULL;
	SDL_Rect clip = {0,0,0,0};

	RenderObject(bool v) : valid(v) {}
	RenderObject(bool v, SDL_Texture* t, SDL_Rect c) : valid(v), tx(t), clip(c) {}

	void renderAt(SDL_Rect* dst) {
		SDL_RenderCopy(g_rd, tx, &clip, dst);
	}
};

class TextureAtlas {
	SDL_Texture* texture;
	int txW, txH;
	int nextX = 0, cY = 0, nextY = 0;
	std::vector<SDL_Rect> allocatedRects;

public:
	TextureAtlas(int txw, int txh);
	~TextureAtlas();

	void blit(SDL_Surface* srf, int atx, int aty);

	RenderObject put(SDL_Surface* srf);
};

class TextureBuffer
{
private:
	int txW, txH;
	std::vector<TextureAtlas*> texturesCreated;
public:
	TextureBuffer(int atlasW = 1024, int atlasH = 1024) {
		txW = atlasW;
		txH = atlasH;
	}

	~TextureBuffer() {
		for (auto*& atl : texturesCreated) {
			delete atl;
		}
	}

	RenderObject put(SDL_Surface* srf) {
		if (srf->w > txW || srf->h > txH) {
			return { false };
		}
		for (auto& atl : texturesCreated) {
			RenderObject ro = atl->put(srf);
			if (ro.valid) {
				return ro;
			}
		}
		texturesCreated.push_back(new TextureAtlas( txW, txH ));
		return texturesCreated.back()->put(srf);
	}
};

