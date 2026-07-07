#pragma once
#include "globals.h"

inline u64 thumbnails_cacheSize = 0;
const u64 thumbnails_maxCache = 8 * 1024 * 1024;	//8mb cache

void thumbnails_start();
void thumbnails_stop();
void thumbnails_threadLoop();
void thumbnails_clearCache();
void thumbnails_clearHalfOfCache();

void thumbnails_request(PlatformNativePathString path);
XY thumbnails_getSize(PlatformNativePathString path);
void thumbnails_render(PlatformNativePathString path, SDL_Rect* src, SDL_Rect* dst);