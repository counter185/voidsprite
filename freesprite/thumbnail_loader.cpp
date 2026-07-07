#include <thread>

#include "thumbnail_loader.h"
#include "FileIO.h"

struct ThumbnailData {
	PlatformNativePathString filePath;
	Layer* thumbnail;
};

std::thread* threadThumbnailLoader = NULL;

std::recursive_mutex thumbnailsMutex, requestedPathsMutex;
std::vector<ThumbnailData> thumbnails;
std::vector<PlatformNativePathString> requestedPaths;
XY thumbnailMaxSize = { 384,384 };
bool thumbnails_running = false;

void thumbnails_start()
{
#if VSP_PLATFORM != VSP_PLATFORM_EMSCRIPTEN
	thumbnails_running = true;
	threadThumbnailLoader = new std::thread(thumbnails_threadLoop);
#endif
}

void thumbnails_stop()
{
#if VSP_PLATFORM != VSP_PLATFORM_EMSCRIPTEN
	if (thumbnails_running) {
		thumbnails_running = false;
		if (threadThumbnailLoader->joinable()) {
			threadThumbnailLoader->join();
		}
	}
#endif
}

void thumbnails_threadLoop()
{
	g_interactiveContext = false;
	while (thumbnails_running) {
		requestedPathsMutex.lock();
		PlatformNativePathString next;
		if (!requestedPaths.empty()) {
			next = requestedPaths.back();
			requestedPaths.pop_back();
		}
		requestedPathsMutex.unlock();
		if (!next.empty() && std::filesystem::exists(next)) {
			Layer* l = loadAnyIntoFlat(convertStringToUTF8OnWin32(next));
			if (l != NULL) {
				if (l->w > thumbnailMaxSize.x || l->h > thumbnailMaxSize.y) {
					SDL_Rect scaleSize = fitInside({ 0,0,thumbnailMaxSize.x, thumbnailMaxSize.y }, { 0,0,l->w, l->h });
					Layer* newL = l->copyAllVariantsScaled({ scaleSize.w, scaleSize.h });
					delete l;
					l = newL;
				}
			}

			thumbnailsMutex.lock();
			thumbnails_cacheSize += l != NULL ? (l->w * l->h * 4) : 0;
			thumbnails.push_back({ next, l });
			if (thumbnails_cacheSize >= thumbnails_maxCache && thumbnails.size() > 1) {
				g_startNewMainThreadOperation([]() {thumbnails_clearHalfOfCache(); });
			}
			thumbnailsMutex.unlock();
		}
		SDL_Delay(500);
	}
}

void thumbnails_clearCache()
{
	std::lock_guard<std::recursive_mutex> lock(thumbnailsMutex);
	for (auto& t : thumbnails) {
		delete t.thumbnail;
	}
	thumbnails.clear();
}

void thumbnails_clearHalfOfCache()
{
	std::lock_guard<std::recursive_mutex> lock(thumbnailsMutex);
	int num = thumbnails.size() / 2;
	for (int i = 0; i < num; i++) {
		ThumbnailData& front = thumbnails.front();
		if (front.thumbnail != NULL) {
			thumbnails_cacheSize -= front.thumbnail->w * front.thumbnail->h * 4;
			delete front.thumbnail;
		}
		thumbnails.erase(thumbnails.begin());
	}
}

void thumbnails_request(PlatformNativePathString path)
{
#if VSP_PLATFORM != VSP_PLATFORM_EMSCRIPTEN
	const u64 maxFileSize = 50 * 1024 * 1024;//max 50mb files
	{
		std::lock_guard<std::recursive_mutex> lock(thumbnailsMutex);
		auto find = std::find_if(thumbnails.begin(), thumbnails.end(), [path](ThumbnailData& t) { return t.filePath == path; });
		if (find != thumbnails.end()) {
			return;
		}
	}

	{
		std::lock_guard<std::recursive_mutex> lock(requestedPathsMutex);
		auto find = std::find(requestedPaths.begin(), requestedPaths.end(), path);
		if (find != requestedPaths.end()) {
			return;
		}
		else {
			try {
				if (std::filesystem::file_size(path) < maxFileSize) {
					requestedPaths.push_back(path);
				}
				else {
					std::lock_guard<std::recursive_mutex> lock(thumbnailsMutex);
					//autofail if file is too big
					thumbnails.push_back({path, NULL});
				}
			}
			catch (std::exception&) {}
		}
	}
#endif
}

XY thumbnails_getSize(PlatformNativePathString path)
{
#if VSP_PLATFORM != VSP_PLATFORM_EMSCRIPTEN
	std::lock_guard<std::recursive_mutex> lock(thumbnailsMutex);
	auto find = std::find_if(thumbnails.begin(), thumbnails.end(), [path](ThumbnailData& t) { return t.filePath == path; });
	if (find == thumbnails.end()) {
		thumbnails_request(path);
		return {0,0};
	}
	else {
		return (*find).thumbnail != NULL ? XY{ (*find).thumbnail->w, (*find).thumbnail->h } : XY{0,0};
	}
#else
	return { 0,0 };
#endif
}

void thumbnails_render(PlatformNativePathString path, SDL_Rect* src, SDL_Rect* dst)
{
#if VSP_PLATFORM != VSP_PLATFORM_EMSCRIPTEN
	std::lock_guard<std::recursive_mutex> lock(thumbnailsMutex);
	auto find = std::find_if(thumbnails.begin(), thumbnails.end(), [path](ThumbnailData& t) { return t.filePath == path; });
	if (find != thumbnails.end()) {
		(*find).thumbnail->prerender();
		SDL_RenderCopy(g_rd, (*find).thumbnail->renderData[g_rd].tex, src, dst);
	}
	else {
		thumbnails_request(path);
	}
#endif
}
