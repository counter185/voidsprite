#pragma once


inline std::map<std::string, u64> g_named_memmap;
inline std::map<void*, std::pair<u64, std::string>> allocated_mems;
inline int g_allocated_textures = 0;

inline SDL_Texture* tracked_createTextureFromSurface(SDL_Renderer* renderer, SDL_Surface* surface) {
    SDL_Texture* ret = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_SetTextureScaleMode(ret, SDL_SCALEMODE_NEAREST);
#if _DEBUG
    if (ret != NULL) {
        g_allocated_textures++;
        allocated_mems[(void*)ret] = {surface->w * surface->h * 4, "Textures"};
        g_named_memmap["Textures"] += surface->w * surface->h * 4;
    }
#endif
    return ret;
}
inline SDL_Texture* tracked_createTexture(SDL_Renderer* renderer, u32 format, SDL_TextureAccess access, int w, int h) {
    SDL_Texture* ret = SDL_CreateTexture(renderer, format, access, w, h);
    SDL_SetTextureScaleMode(ret, SDL_SCALEMODE_NEAREST);
#if _DEBUG
    if (ret != NULL) {
        g_allocated_textures++;
        allocated_mems[(void*)ret] = {w * h * 4, "Textures"};
        g_named_memmap["Textures"] += w * h * 4;
    }
#endif
    return ret;
}
inline void tracked_destroyTexture(SDL_Texture* texture) {
#if _DEBUG
    if (texture != NULL) {
        g_allocated_textures--;
        g_named_memmap["Textures"] -= allocated_mems[texture].first;
        allocated_mems.erase(texture);
    }
#endif
    SDL_DestroyTexture(texture);
}

inline void* tracked_malloc(u64 bytes, std::string name = "unk") {
	void* ret = malloc(bytes);
#if _DEBUG
	if (ret != NULL) {
        allocated_mems[ret] = { bytes, name };
        g_named_memmap[name] += bytes;
	}
#endif
	return ret;
}

inline void tracked_free(void *ptr) {
    if (ptr != NULL) {
#if _DEBUG
        g_named_memmap[allocated_mems[ptr].second] -= allocated_mems[ptr].first;
        allocated_mems.erase(ptr);
#endif
        free(ptr);
    }
}

inline std::string bytesToFriendlyString(u64 bytes) {
    std::string suffixes[] = { "B", "KB", "MB", "GB", "TB" };
    double bytesf = (double)bytes;
    int suffixIndex = 0;
    while (bytesf >= 1024 && suffixIndex < 4) {
        bytesf /= 1024;
        suffixIndex++;
    }
    return std::format("{:.4f}{}", bytesf, suffixes[suffixIndex]);
}