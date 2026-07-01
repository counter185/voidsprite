#pragma once
#include <thread>
#include <mutex>

#include "BaseScreen.h"
#include "Canvas.h"
#include "PanelUserInteractable.h"
#include "BaseFilter.h"
#include "EventCallbackListener.h"

#define MULTITHREAD_IMAGE_PROCESSING ONPLATFORM(VSP_PLATFORM_EMSCRIPTEN, 0, 1)

class ScreenPreprocessImage;

inline std::vector<BaseFilter*> g_compositeFilters = {};
void loadCompositeFilters();

class CompositeFilterBackground : public BaseFilter {
    std::string name() override { return "Background"; }
    std::string id() override { return "filter.composite.background"; }

    Layer* run(Layer* src, ParameterStore* options) override;
    ParamList getParameters() override {
        return {
			COLORRGB_PARAM("color", 0xFFFFFFFF)
        };
    }
};

struct PreprocessImageFilter {
    BaseFilter* filter;
    ParameterStore params;
};

enum QuantizeMode {
    QUANTIZEMODE_MANUAL = 0,
    QUANTIZEMODE_TOPALETTE = 1
};

class PreprocessPreset {
protected:
    PreprocessPreset() {}
public:
    std::string name = "Preset";
    std::vector<FilterPreset> filters;

    PreprocessPreset(std::string name, std::vector<FilterPreset> filters) : name(name), filters(filters) {}

    std::string serialize();
    static PreprocessPreset deserialize(std::string s);
};

class PanelFilterList : public PanelUserInteractable {
private:
    ScrollingPanel* listPanel;
    ScreenPreprocessImage* parent = NULL;

    ScrollingPanel* colorListPanel;
    UIDropdown* quantizeModeDropdown = NULL;
public:
    UIColorInputField* gridColorField = NULL;
    std::function<void()> onFilterValueChangedCallback = NULL;

    PanelFilterList(ScreenPreprocessImage* caller);

    void setQuantizeModeFromUI(ScreenPreprocessImage* caller, int idx);

    void populateFilters(std::vector<PreprocessImageFilter>* filters);
    void populateColorList();
};

class ScreenPreprocessImage :
    public BaseScreen, public EventCallbackListener
{
protected:
    PanelFilterList* filterListPanel = NULL;
    Canvas c;
    Layer* target = NULL;
    std::recursive_mutex filterListMutex;
    std::vector<PreprocessImageFilter> filters;
    XY processedSize = { 0,0 };

    bool wasLocked = false;
    Layer* processedImage = NULL;
    std::mutex processedImageMutex;
    std::thread* imageProcessorThread = NULL;
    std::atomic<bool> threadRunning = false;
    std::atomic<bool> threadProcessing = false;

    QuantizeMode quantizeMode = QUANTIZEMODE_MANUAL;
public:
    std::atomic<bool> changes = true;
    std::atomic<bool> preDownscale = true;
    std::atomic<int> downscaleW = 1;
    std::atomic<int> downscaleH = 1;

    std::mutex quantizePaletteMutex;
    std::vector<u32> quantizePalette = {};

    std::vector<PreprocessPreset> builtinPresets = {
        PreprocessPreset("12-color convert", {
            FilterPreset("filter.brightnesscontrast", {
                {"brightness", "-36"},
                {"contrast", "1.448"},
                {"contrast.first", "0"}
            }),
            FilterPreset("filter.quantize", {
                {"num.colors","12"}
			})
        }),
        PreprocessPreset("12-color convert (bright)", {
            FilterPreset("filter.brightnesscontrast", {
                {"brightness", "-40"},
                {"contrast", "0.744"},
                {"contrast.first", "0"}
            }),
            FilterPreset("filter.quantize", {
                {"num.colors","12"}
			}),
            FilterPreset("filter.brightnesscontrast", {
                {"brightness", "8"},
                {"contrast", "1.456"},
                {"contrast.first", "1"}
            })
        }),
        PreprocessPreset("Brighten", {
            FilterPreset("filter.brightnesscontrast", {
                {"brightness","90"}
            })
        }),
        PreprocessPreset("Contrast+", {
            FilterPreset("filter.brightnesscontrast", {
                {"brightness","-70"},
                {"contrast", "1.4"},
                {"contrast.first", "1"}
            })
        }),
        PreprocessPreset("8 colors", {
            FilterPreset("filter.quantize", {
                {"num.colors","8"}
            })
        })
    };

    ScreenPreprocessImage(Layer* l);
    ~ScreenPreprocessImage();

    std::string getName() override { return TL("vsp.preprocess"); }

    void render() override;
    void tick() override;
    void defaultInputAction(SDL_Event evt) override;

    void eventFileSaved(int evt_id, PlatformNativePathString name, int exporterIndex = -1) override;
    void eventFileOpen(int evt_id, PlatformNativePathString name, int importerIndex = -1) override;

    void renderBackground();
    void renderCanvas();
    void drawForeground();

    void addFilter(BaseFilter* f);
    void removeFilter(int idx);
    void moveFilter(int from, int offs);
    void clearFilters();
    void applyPreset(PreprocessPreset preset);
    void openInEditor();
    void setQuantizeMode(QuantizeMode mode) { quantizeMode = mode; changes = true; }
    void promptLoadStackPreset();
    void promptSaveStackPreset();
    std::string saveToTempFile();

    void updateProcessedImage();

    void imageProcessThread();
    void quantizeToCurrentPalette(Layer* target);
};

