#include "sdk_impl.h"
#include "LayerPalettized.h"

VSPFilter* impl_registerFilter(const char* name, void (*filterFunction)(VSPLayer* layer, VSPFilter* filter))
{
	FilterExternal* f = new FilterExternal();
	f->n = std::string(name);
	f->f = filterFunction;
	g_pluginFilters.push_back(f);
	return f;
}

void impl_registerLayerImporter(
	const char* name,
	const char* extension,
	int layerTypes,
	VSPFileExporter* matchingExporter,
	VSPLayer* (*importFunction)(char* path),
	bool (*canImportFunction)(char* path)) {

	std::function<Layer* (PlatformNativePathString, u64)> wImportFunction =
		[importFunction](PlatformNativePathString path, u64) {
		std::string convPathUTF8 = convertStringToUTF8OnWin32(path);
		return importFunction((char*)convPathUTF8.c_str());
	};
	std::function<bool(PlatformNativePathString)> wCanImportFunction =
		[canImportFunction](PlatformNativePathString path) {
		std::string convPathUTF8 = convertStringToUTF8OnWin32(path);
		return canImportFunction == NULL ? true : canImportFunction((char*)convPathUTF8.c_str());
		};

	loginfo(std::format("Registering new file importer: {}  [{}]", std::string(name), std::string(extension)));
	g_pluginRegisteredFileImporters.push_back(
		FileImporter::flatImporter(
			std::string(name),
			std::string(extension),
			wImportFunction,
			matchingExporter,
			layerTypes,
			wCanImportFunction
		)
	);
}

VSPFileExporter* impl_registerLayerExporter(
	const char* name,
	const char* extension,
	int layerTypes,
	bool (*exportFunction)(VSPLayer* layer, char* path),
	bool (*canExportFunction)(VSPLayer* layer)) {

	std::function<bool(PlatformNativePathString, Layer*)> wExportFunction =
		[exportFunction](PlatformNativePathString path, Layer* layer) {
		std::string convPathUTF8 = convertStringToUTF8OnWin32(path);
		return exportFunction(layer, (char*)convPathUTF8.c_str());
		};

	std::function<bool(Layer*)> wCanExportFunction =
		[canExportFunction](Layer* layer) {
		return canExportFunction == NULL ? true : canExportFunction(layer);
		};

	loginfo(std::format("Registering new file exporter: {}  [{}]", std::string(name), std::string(extension)));
	FileExporter* exporter = FileExporter::flatExporter(
		std::string(name),
		std::string(extension),
		wExportFunction,
		layerTypes,
		wCanExportFunction
	);
	g_pluginRegisteredFileExporters.push_back(exporter);
	return exporter;
}

VSPLayer* impl_layerAllocNew(int type, int width, int height) {
	if (type == VSP_LAYER_RGBA) {
		return Layer::tryAllocLayer(width, height);
	}
	else if (type == VSP_LAYER_INDEXED) {
		return LayerPalettized::tryAllocIndexedLayer(width, height);
	}
	else {
		return NULL;
	}
}
void impl_layerFree(VSPLayer* layer) {
	if (layer == NULL) {
		return;
	}
	delete layer;
}
VSPLayerInfo* impl_layerGetInfo(VSPLayer* layer) {
	if (layer == NULL) {
		return NULL;
	}
	VSPLayerInfo* info = (VSPLayerInfo*)malloc(sizeof(VSPLayerInfo));
	*info = {};
	info->type = layer->isPalettized ? VSP_LAYER_INDEXED : VSP_LAYER_RGBA;
	info->width = layer->w;
	info->height = layer->h;
	return info;
}
void impl_layerSetPixel(VSPLayer* layer, int x, int y, uint32_t color) {
	if (layer == NULL) {
		return;
	}
	return layer->setPixel({ x,y }, color);
}
uint32_t impl_layerGetPixel(VSPLayer* layer, int x, int y) {
	if (layer == NULL) {
		return 0;
	}
	return layer->getPixelAt({ x,y });
}
uint32_t* impl_layerGetRawPixelData(VSPLayer* layer) {
	if (layer == NULL) {
		return NULL;
	}
	return (uint32_t*)layer->pixelData;
}