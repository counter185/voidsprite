#include "sdk_impl.h"
#include "LayerPalettized.h"

void impl_registerLayerImporter(
	const char* name,
	const char* extension,
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
		return canImportFunction((char*)convPathUTF8.c_str());
		};

	g_pluginRegisteredFileImporters.push_back(
		FileImporter::flatImporter(
			std::string(name),
			std::string(extension),
			wImportFunction,
			matchingExporter,
			1,
			wCanImportFunction
		)
	);
}

VSPFileExporter* impl_registerLayerExporter(const char* name,
	const char* extension,
	bool (*exportFunction)(VSPLayer* layer, char* path),
	bool (*canExportFunction)(VSPLayer* layer)) {

	std::function<bool(PlatformNativePathString, Layer*)> wExportFunction =
		[exportFunction](PlatformNativePathString path, Layer* layer) {
		std::string convPathUTF8 = convertStringToUTF8OnWin32(path);
		return exportFunction(layer, (char*)convPathUTF8.c_str());
		};

	std::function<bool(Layer*)> wCanExportFunction =
		[canExportFunction](Layer* layer) {
		return canExportFunction(layer);
		};

	FileExporter* exporter = FileExporter::flatExporter(
		std::string(name),
		std::string(extension),
		wExportFunction,
		1,
		wCanExportFunction
	);
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
bool impl_layerIsIndexed(VSPLayer* layer) {
	if (layer == NULL) {
		return false;
	}
	return layer->isPalettized;
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