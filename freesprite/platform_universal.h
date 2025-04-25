#pragma once

#include "globals.h"
#include "EventCallbackListener.h"
#include "PopupFilePicker.h"

int findIndexByExtension(
        std::vector<std::pair<std::string, std::string>> &filetypes,
        std::string filename) {
    std::transform(filename.begin(), filename.end(), filename.begin(),
                   ::tolower);
    for (int x = 0; x < filetypes.size(); x++) {
        auto &p = filetypes[x];
        if (filename.size() > p.first.size()) {
            if (filename.substr(filename.size() - p.first.size()) == p.first) {
                return x + 1;
            }
        }
    }
    return -1;
}

inline void universal_platformTrySaveOtherFile(
	EventCallbackListener *caller,
	std::vector<std::pair<std::string, std::string>> filetypes,
	std::string windowTitle, int evt_id) {
	PopupFilePicker* fp = PopupFilePicker::SaveFile(windowTitle, filetypes);
	fp->setCallbackListener(evt_id, caller);
	g_addPopup(fp);    
}

inline void universal_platformTryLoadOtherFile(
	EventCallbackListener *listener,
	std::vector<std::pair<std::string, std::string>> filetypes,
	std::string windowTitle, int evt_id) {
	//universal_platformTryLoadOtherFile(listener, filetypes, windowTitle, evt_id);
	PopupFilePicker* fp = PopupFilePicker::OpenFile(windowTitle, filetypes);
	fp->setCallbackListener(evt_id, listener);
	g_addPopup(fp);
}

inline void universalSDL_platformTryLoadOtherFile(EventCallbackListener* listener, std::vector<std::pair<std::string, std::string>> filetypes, std::string windowTitle, int evt_id) {

    EventCallbackListener* lsnr = listener;
    static std::function<void(void*, const char* const*, int)> dialogCallback;
    dialogCallback = [lsnr, evt_id, &filetypes](void* userdata, const char* const* filelist, int filter) {
        if (filelist != NULL) {
            std::string filenameStr = filelist[0];
            loginfo(std::format("File selected: {}", filenameStr));
            EventCallbackListener* listener = (EventCallbackListener*)userdata;
            lsnr->eventFileOpen(evt_id, convertStringOnWin32(filenameStr), findIndexByExtension(filetypes, filenameStr));
        }
    };
    static SDL_DialogFileCallback dialogCallback2;
    dialogCallback2 = [](void* userdata, const char* const* filelist, int filter) {
        auto fn = *((std::function<void(void*, const char* const*, int)>*)userdata);
        fn(userdata, filelist, filter);
    };

    std::vector<std::pair<std::string, std::string>> transformedFiletypes = {};
    for (auto& p : filetypes) {
        transformedFiletypes.push_back({p.first == "" ? "*" : p.first, p.second});
    }

    std::vector<SDL_DialogFileFilter> filters = {};
    for (auto& p : transformedFiletypes) {
        SDL_DialogFileFilter filter = {};
        filter.name = p.second.c_str();
        filter.pattern = p.first.c_str();
        filters.push_back(filter);
    }

    SDL_ShowOpenFileDialog(
        dialogCallback2,
        &dialogCallback,
        g_wd,
        filters.data(),
        filters.size(),
        "/sdcard/",
        false
    );

    logerr(SDL_GetError());
}

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