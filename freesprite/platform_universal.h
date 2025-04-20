#pragma once

#include "globals.h"

inline bool universal_platformPushLayerToClipboard(Layer* l) {
	std::vector<u8> pngData = writePNGToMem(l);
	static uint64_t fileLength;
	fileLength = pngData.size();
	u8* pngDataCopy = (u8*)tracked_malloc(fileLength, "Clipboard data");
	memcpy(pngDataCopy, pngData.data(), fileLength);
	SDL_ClipboardDataCallback cb = [](void* userdata, const char* mimetype, size_t* size) {
		loginfo(std::format("Requested clipboard mime type: {}", mimetype));
		if (mimetype != NULL) {
			std::string mtype = mimetype;
			if (mtype == "image/png") {
				*size = fileLength;
				return (const void*)userdata;
			}
			else {
				*size = 0;
				return (const void*)NULL;
			}
		}
		*size = 0;
		return (const void*)NULL;
	};
	SDL_ClipboardCleanupCallback ccb = [](void* userdata) {
		loginfo("Clipboard cleanup callback called");
		tracked_free(userdata);
	};
	const char* mimetypesReturned[] = {"image/png"};
	return SDL_SetClipboardData(cb, ccb, pngDataCopy, (const char**)mimetypesReturned, 1);
}

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