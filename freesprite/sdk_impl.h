#pragma once

#include "globals.h"
#include "Layer.h"
#include "FileIO.h"
#include "BaseFilter.h"

#define VSPLayer Layer
#define VSPFileExporter FileExporter
#define VSPFilter BaseFilter

#include "sdk_structs.h"

inline std::map<int, voidspriteSDK*> g_vspsdks;

class FilterExternal : public BaseFilter {
private:
    std::map<std::string, std::string> optionsNow;
public:
    std::string n = "External filter";
    std::function<void(Layer*, VSPFilter*)> f = NULL;
    std::vector<FilterParameter> fParams = {};

    double getDoubleValue(const char* name) { return std::stod(optionsNow[std::string(name)]); }
    int getIntValue(const char* name) { return std::stoi(optionsNow[std::string(name)]); }
    bool getBoolValue(const char* name) { return getDoubleValue(name) == 1; }
    double getRangeValue2(const char* name) { return std::stod(optionsNow[std::string(name)]); }

    std::string name() override { return n; }
    Layer* run(Layer* src, std::map<std::string, std::string> options) override {
        optionsNow = options;
		Layer* c = copy(src);
        f(c, this);
        return c;
    }
    std::vector<FilterParameter> getParameters() override { return fParams; }
};

VSPFilter* impl_registerFilter(
    const char* name,
    void (*filterFunction)(VSPLayer* layer, VSPFilter* filter));

void impl_registerLayerImporter(
    const char* name,
    const char* extension,
    int layerTypes,
    VSPFileExporter* matchingExporter,
    VSPLayer* (*importFunction)(char* path),
    bool (*canImportFunction)(char* path));

VSPFileExporter* impl_registerLayerExporter(
    const char* name,
    const char* extension,
    int layerTypes,
    bool (*exportFunction)(VSPLayer* layer, char* path),
    bool (*canExportFunction)(VSPLayer* layer));

VSPLayer*	impl_layerAllocNew(int type, int width, int height);
void		impl_layerFree(VSPLayer* layer);
VSPLayerInfo*	impl_layerGetInfo(VSPLayer*);
void		impl_layerSetPixel(VSPLayer* layer, int x, int y, uint32_t color);
uint32_t	impl_layerGetPixel(VSPLayer* layer, int x, int y);
uint32_t*	impl_layerGetRawPixelData(VSPLayer* layer);

inline void impl_filterNewBoolParameter(VSPFilter* filter, const char* name, bool defaultValue) { ((FilterExternal*)filter)->fParams.push_back(BOOL_PARAM(std::string(name), defaultValue?1.0:0.0)); }
inline void impl_filterNewIntParameter(VSPFilter* filter, const char* name, int minValue, int maxValue, int defaultValue) { ((FilterExternal*)filter)->fParams.push_back(INT_PARAM(std::string(name), minValue, maxValue, defaultValue)); }

inline bool impl_filterGetBoolValue(BaseFilter* f, const char* name) { return f != NULL ? ((FilterExternal*)f)->getBoolValue(name) : false; }
inline int impl_filterGetIntValue(BaseFilter* f, const char* name) { return f != NULL ? ((FilterExternal*)f)->getIntValue(name) : 0; }
inline double impl_filterGetDoubleValue(BaseFilter* f, const char* name) { return f != NULL ? ((FilterExternal*)f)->getDoubleValue(name) : 0.0; }
inline double impl_filterGetRangeValue2(BaseFilter* f, const char* name) { return f != NULL ? ((FilterExternal*)f)->getRangeValue2(name) : 0.0; }

inline void g_createVSPSDK() {
    voidspriteSDK* v1SDK = new voidspriteSDK();
	v1SDK->util_fopenUTF8 = [](char* path_utf8, const char* mode) { return platformOpenFile(convertStringOnWin32(path_utf8), convertStringOnWin32(mode)); };
	v1SDK->registerFilter = impl_registerFilter;
    v1SDK->registerLayerImporter = impl_registerLayerImporter;
    v1SDK->registerLayerExporter = impl_registerLayerExporter;
    v1SDK->layerAllocNew = impl_layerAllocNew;
    v1SDK->layerFree = impl_layerFree;
    v1SDK->layerGetInfo = impl_layerGetInfo;
    v1SDK->layerSetPixel = impl_layerSetPixel;
    v1SDK->layerGetPixel = impl_layerGetPixel;
    v1SDK->layerGetRawPixelData = impl_layerGetRawPixelData;
    v1SDK->filterNewBoolParameter = impl_filterNewBoolParameter;
    v1SDK->filterNewIntParameter = impl_filterNewIntParameter;
    v1SDK->filterGetBoolValue = impl_filterGetBoolValue;
    v1SDK->filterGetIntValue = impl_filterGetIntValue;
    v1SDK->filterGetDoubleValue = impl_filterGetDoubleValue;
    v1SDK->filterGetRangeValue1 = impl_filterGetDoubleValue;
    v1SDK->filterGetRangeValue2 = impl_filterGetRangeValue2;
    g_vspsdks[1] = v1SDK;
}