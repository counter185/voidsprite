#pragma once

#include "globals.h"
#include "Layer.h"
#include "FileIO.h"
#include "BaseFilter.h"
#include "maineditor.h"
#include "brush/BaseBrush.h"
#include "Notification.h"

#define VSPLayer Layer
#define VSPFileExporter FileExporter
#define VSPFilter BaseFilter
#define VSPEditorContext MainEditor
#define VSPBrush BaseBrush

#include "sdk_structs.h"

inline std::map<int, voidspriteSDK*> g_vspsdks;

class BrushExternal : public BaseBrush {
public:
    std::string name = "External brush";
    std::string tooltip = "";
    bool wantDoublePrecision = false;
    std::function<void(VSPBrush*, VSPEditorContext*, int, int)> fClick = NULL;
    std::function<void(VSPBrush*, VSPEditorContext*, int, int, int,int)> fDrag = NULL;
    std::function<void(VSPBrush*, VSPEditorContext*, int, int)> fRelease = NULL;

    std::string getName() override { return name; }
    std::string getTooltip() override { return tooltip; }
    bool wantDoublePosPrecision() override { return wantDoublePrecision; }

    void clickPress(MainEditor* editor, XY pos) override { if (fClick != NULL) { fClick(this, editor, pos.x, pos.y); } }
    void clickDrag(MainEditor* editor, XY from, XY to) override { if (fDrag != NULL) { fDrag(this, editor, from.x, from.y, to.x, to.y); } }
    void clickRelease(MainEditor* editor, XY pos) override { if (fRelease != NULL) { fRelease(this, editor, pos.x, pos.y); } }
};

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

VSPBrush* impl_registerBrush(
    const char* name,
    const char* tooltip,
    bool wantDoublePrecision,
    void (*clickFunction)(VSPBrush* brush, VSPEditorContext* editor, int x, int y),
    void (*dragFunction)(VSPBrush* brush, VSPEditorContext* editor, int fromX, int fromY, int toX, int toY),
    void (*releaseFunction)(VSPBrush* brush, VSPEditorContext* editor, int x, int y));

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

inline uint32_t impl_editorGetActiveColor(VSPEditorContext* editor) { return editor != NULL ? editor->getActiveColor() : 0; }
inline int impl_editorGetNumLayers(VSPEditorContext* editor) { return editor != NULL ? (int)editor->layers.size() : 0; }
inline VSPLayer* impl_editorGetLayer(VSPEditorContext* editor, int index) { return (editor != NULL && index >= 0 && index < (int)editor->layers.size()) ? editor->layers[index] : NULL; }
inline VSPLayer* impl_editorGetActiveLayer(VSPEditorContext* editor) { return editor != NULL ? editor->getCurrentLayer() : NULL; }
inline void impl_editorSetPixel(VSPEditorContext* editor, int x, int y, uint32_t color) { if (editor != NULL) { editor->SetPixel({x,y}, color); } }

inline void impl_vspPostNotification(const char* title, const char* message, u32 color, int durationMS) {
    g_addNotificationFromThread(Notification(title, message, durationMS, NULL, uint32ToSDLColor(color)));
}

inline void impl_vspPostSuccessNotification(const char* title, const char* message) {
    g_addNotificationFromThread(SuccessNotification(title, message));
}

inline void impl_vspPostErrorNotification(const char* title, const char* message) {
    g_addNotificationFromThread(ErrorNotification(title, message));
}

inline void panicAndClose() {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "voidsprite",
        TL("vsp.error.pluginfail").c_str(), g_wd);
    loginfo_sync("hit jump from plugin to placeholder. very bad");
    //expect the stack to be completely broken here so just exit
    exit(1);
}

inline void g_createVSPSDK() {
    voidspriteSDK* v1SDK = (voidspriteSDK*)tracked_malloc(sizeof(voidspriteSDK) + sizeof(void*)*100, "Plugins"); //new voidspriteSDK();
    v1SDK->util_fopenUTF8 = [](char* path_utf8, const char* mode) { return platformOpenFile(convertStringOnWin32(path_utf8), convertStringOnWin32(mode)); };
    v1SDK->util_free = [](void* a) { free(a); };

    v1SDK->registerFilter = impl_registerFilter;
    v1SDK->registerLayerImporter = impl_registerLayerImporter;
    v1SDK->registerLayerExporter = impl_registerLayerExporter;
    v1SDK->registerBrush = impl_registerBrush;

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

    v1SDK->editorGetActiveColor = impl_editorGetActiveColor;
    v1SDK->editorGetNumLayers = impl_editorGetNumLayers;
    v1SDK->editorGetLayer = impl_editorGetLayer;
    v1SDK->editorGetActiveLayer = impl_editorGetActiveLayer;
    v1SDK->editorSetPixel = impl_editorSetPixel;

    v1SDK->vspPostNotification = impl_vspPostNotification;
    v1SDK->vspPostSuccessNotification = impl_vspPostSuccessNotification;
    v1SDK->vspPostErrorNotification = impl_vspPostErrorNotification;

    //hit panicAndClose if any later function doesn't exist in the current version
    void** nextPlaceholderJump = (void**)(v1SDK + 1);
    for (int i = 0; i < 100; i++) {
        nextPlaceholderJump[i] = (void*)&panicAndClose;
    }
    g_vspsdks[1] = v1SDK;
}