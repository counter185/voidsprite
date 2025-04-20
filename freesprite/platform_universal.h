#pragma once

#include "globals.h"

inline Layer* universal_platformGetLayerFromClipboard() {
	std::map<std::string,bool> formats = {};
	size_t numMimeTypes = 0;
	char** mimetypes = SDL_GetClipboardMimeTypes(&numMimeTypes);
	for (size_t i = 0; i < numMimeTypes; ++i) {
		loginfo(std::format("Found clipboard mimetype: {}", mimetypes[i]));
		formats[mimetypes[i]] = true;
	}

	Layer* ret = NULL;
	if (mimetypes != NULL) {
		if (formats.contains("image/png")) {
			size_t dataSize = 0;
			u8* pngdata = (u8*)SDL_GetClipboardData("image/png", &dataSize);
			if (pngdata != NULL) {
				ret = readPNGFromMem(pngdata, dataSize);
				SDL_free(pngdata);
			}
		}
		else {
			for (auto& [type, _] : formats) {
				if (stringStartsWithIgnoreCase(type, "image/")) {
					size_t dataSize = 0;
					u8* imageData = (u8*)SDL_GetClipboardData(type.c_str(), &dataSize);
					SDL_IOStream* io = SDL_IOFromMem(imageData, dataSize);
					SDL_Surface* srf = IMG_Load_IO(io, true);
					if (srf != NULL) {
						ret = new Layer(srf);
						SDL_FreeSurface(srf);
					}
					else {
						logerr(std::format("Failed to get clipboard image:\n  {}", SDL_GetError()));
					}
				}
				if (ret != NULL) {
					break;
				}
			}
		}
		SDL_free(mimetypes);
	}
	else {
		logerr(std::format("Failed to get clipboard mimetypes:\n  {}", SDL_GetError()));
	}
	return ret;
	
}