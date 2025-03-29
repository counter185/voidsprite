#include "TextureBuffer.h"

TextureAtlas::TextureAtlas(int txw, int txh) {
	txW = txw;
	txH = txh;
	texture = tracked_createTexture(g_rd, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, txW, txH);
}

TextureAtlas::~TextureAtlas() {
	tracked_destroyTexture(texture);
}

void TextureAtlas::blit(SDL_Surface* srf, int atx, int aty) {
	u32* pixels;
	u32 pitch;
	SDL_Surface* sourceTarget = srf;
	if (srf->format != SDL_PIXELFORMAT_ARGB8888) {
		sourceTarget = SDL_ConvertSurface(srf, SDL_PIXELFORMAT_ARGB8888);
	}
	//printf("LockTexture()\n");
	SDL_LockTexture(texture, NULL, (void**)&pixels, (int*)&pitch);
	//printf(SDL_GetError());
	for (int y = 0; y < srf->h; y++) {
		//we can assume this will be in bounds (checks are already being done in put())
		memcpy(&ARRAY2DPOINT(pixels, atx, aty + y, pitch/4), &ARRAY2DPOINT((u8*)sourceTarget->pixels, 0, y, sourceTarget->pitch) , 4 * srf->w);
		//memset(&ARRAY2DPOINT(pixels, atx, aty + y, pitch/4), 255, 4 * srf->w);
	}
	//memset(pixels, 255, 4 * srf->w * srf->h);
	if (sourceTarget != srf) {
		SDL_FreeSurface(sourceTarget);
	}
	SDL_UnlockTexture(texture);
}

RenderObject TextureAtlas::put(SDL_Surface* srf) {
	if (srf->w > txW || srf->h > txH) {
		return { false };
	}
	SDL_Rect area = { 0,0, srf->w, srf->h };
	if (allocatedRects.size() == 0) {
		allocatedRects.push_back(area);
		nextY += area.h;
		nextX = area.w;
	}
	else {
		area.x = nextX;
		area.y = cY;
		if (area.x + area.w > txW) {
			area.y = nextY;
			area.x = 0;
			cY = area.y;
			nextY += area.h;
		}
		if (area.y + area.h > txH) {
			return { false };
		}
		else if (area.y + area.h > nextY) {
			nextY = area.y + area.h;
		}
		nextX = area.w + area.x;
		allocatedRects.push_back(area);
	}

	blit(srf, area.x, area.y);
	return { true, texture, area };
}
