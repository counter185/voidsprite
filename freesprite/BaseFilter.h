#pragma once
#include "globals.h"

#define INT_PARAM(name,min,max,def) (FilterParameter{name,(double)(min),(double)(max),(double)(def),PT_INT})
#define FLOAT_PARAM(name,min,max,def) (FilterParameter{name,min,max,def,PT_FLOAT})
#define COLORRGB_PARAM(name,def) (FilterParameter{name,0,0,0,PT_COLOR_RGB,def})
#define COLORL_PARAM(name) (FilterParameter{name,0,255,127,PT_COLOR_L})
#define BOOL_PARAM(name,def) (FilterParameter{name,0,1,def,PT_BOOL})
#define INT_RANGE_PARAM(name,min,max,defl,defm,col) (FilterParameter{name,min,max,defl,PT_INT_RANGE,col,defm})

enum ParameterType {
    PT_INT = 0,
    PT_FLOAT = 1,
    PT_COLOR_RGB = 2,
    PT_COLOR_L = 3,
    PT_BOOL = 4,
    PT_INT_RANGE = 5,
};

struct FilterParameter {
    std::string name;
    double minValue = 0;
    double maxValue = 1;
    double defaultValue = 0.5;
    ParameterType paramType;
    u32 vU32 = 0;
    double defaultValueTwo = 0.5;
};

void g_loadFilters();
BaseFilter* g_getFilterByID(std::string id);
inline BaseFilter* g_filter_edgeDetect = NULL;
inline BaseFilter* g_filter_quantize = NULL;

class FilterPreset {
public:
    std::string filterID;
    std::map<std::string, std::string> options;

    FilterPreset() {}
    FilterPreset(std::string filterID, std::map<std::string, std::string> options) : filterID(filterID), options(options) {}

    std::string serialize();
    static FilterPreset deserialize(std::string s);
};

class BaseFilter
{
public:
    virtual std::string name() { return "Filter"; }
    virtual std::string id() { return "filter.default";}

    virtual void setupParamBounds(Layer* target) {}

    /// <summary>
    /// Allocates a new layer, copies the source pixels to it and runs the filter on it.
    /// </summary>
    /// <param name="src">source layer to run the filter on</param>
    /// <param name="options">options</param>
    /// <returns>new layer</returns>
    virtual Layer* run(Layer* src, std::map<std::string, std::string> options) { return NULL; };
    virtual std::vector<FilterParameter> getParameters() { return {}; }
protected:
    /// <summary>
    /// Use this instead of Layer::copy in your filter function. Calls Layer::copyWithNoTextureInit so that it's thread-safe.
    /// </summary>
    /// <param name="src">source layer to be copied</param>
    /// <returns>a copy of the layer</returns>
    Layer* copy(Layer* src);
};

class FilterForEachPixel : public BaseFilter {
protected:
    std::string fName = "";
    std::string i = "";
    std::function<u32(XY, Layer*, u32)> f;
    std::vector<FilterParameter> fParams;
public:
    FilterForEachPixel(std::string name, std::string id, std::function<u32(XY, Layer*, u32)> f, std::vector<FilterParameter> fParams = {}) {
        this->fName = name;
        this->i = id;
        this->f = f;
        this->fParams = fParams;
    }

    std::string name() override { return fName; }
    std::string id() override { return i; }
    Layer* run(Layer* src, std::map<std::string, std::string> options) override;
    std::vector<FilterParameter> getParameters() override { return fParams; }
};

class FilterBlur : public BaseFilter
{
public:
    std::string name() override { return "Blur"; }
    std::string id() override { return "filter.blur"; }

    Layer* run(Layer* src, std::map<std::string, std::string> options) override;
    std::vector<FilterParameter> getParameters() override {
        return {
            INT_PARAM("radius.x", 1, 100, 4),
            INT_PARAM("radius.y", 1, 100, 4),
        };
    }
};

class FilterSwapRGBToBGR : public BaseFilter {
public:
    std::string name() override { return "Swap channels RGB->BGR"; }
    std::string id() override { return "filter.rgb2bgr"; }
    Layer* run(Layer* src, std::map<std::string, std::string> options) override;
};

class FilterAdjustHSV : public BaseFilter {
public:
    std::string name() override { return "Adjust HSV"; }
    std::string id() override { return "filter.hsvadj"; }
    Layer* run(Layer* src, std::map<std::string, std::string> options) override;
    std::vector<FilterParameter> getParameters() override {
        return {
            FLOAT_PARAM("hue", -360, 360, 0),
            FLOAT_PARAM("saturation", -100, 100, 0),
            FLOAT_PARAM("value", -100, 100, 0),
            BOOL_PARAM("colorize", 0)
        };
    }
};

class FilterStrideGlitch : public BaseFilter {
public:
    std::string name() override { return "Stride glitch"; }
    std::string id() override { return "filter.strideglitch"; }
    Layer* run(Layer* src, std::map<std::string, std::string> options) override;
    std::vector<FilterParameter> getParameters() override {
        return {
            INT_PARAM("splits", 1, 100, 4),
            INT_RANGE_PARAM("length", 1, 100, 3, 8, 0xffffffff)
        };
    }
};

class FilterPixelize : public BaseFilter {
public:
    std::string name() override { return "Pixelize"; }
    std::string id() override { return "filter.pixelize"; }
    Layer* run(Layer* src, std::map<std::string, std::string> options) override;
    std::vector<FilterParameter> getParameters() override {
        return {
            INT_PARAM("size.x", 1, 100, 2),
            INT_PARAM("size.y", 1, 100, 2),
            BOOL_PARAM("one sample", 1)
        };
    }
};

class FilterOutline : public BaseFilter {
public:
    std::string name() override { return "Outline"; }
    std::string id() override { return "filter.outline"; }
    Layer* run(Layer* src, std::map<std::string, std::string> options) override;
    std::vector<FilterParameter> getParameters() override {
        return {
            INT_PARAM("thickness", 1, 50, 1),
            BOOL_PARAM("corners", 0),
            COLORRGB_PARAM("color", 0xFFFFFFFF)
        };
    }
};

class FilterBrightnessContrast : public BaseFilter {
public:
    std::string name() override { return "Brightness/contrast"; }
    std::string id() override { return "filter.brightnesscontrast"; }
    Layer* run(Layer* src, std::map<std::string, std::string> options) override;
    std::vector<FilterParameter> getParameters() override {
        return {
            INT_PARAM("brightness", -255, 255, 0),
            FLOAT_PARAM("contrast", 0, 2, 1),
            BOOL_PARAM("contrast.first", 0)
        };
    }
};

class FilterQuantize : public BaseFilter {
public:
    std::string name() override { return "Quantize colors"; }
    std::string id() override { return "filter.quantize"; }
    Layer* run(Layer* src, std::map<std::string, std::string> options) override;
    std::vector<FilterParameter> getParameters() override {
        return {
            INT_PARAM("num.colors", 1, 256, 128),
        };
    }
};

class FilterJPEG : public BaseFilter {
public:
    std::string name() override { return "JPEG compression"; }
    std::string id() override { return "filter.jpeg"; }
    Layer* run(Layer* src, std::map<std::string, std::string> options) override;
    std::vector<FilterParameter> getParameters() override {
        return {
            INT_PARAM("quality", 1, 100, 50),
        };
    }
};
class FilterJPEGGlitch : public BaseFilter {
public:
    std::string name() override { return "JPEG glitch"; }
    std::string id() override { return "filter.jpegglitch"; }
    Layer* run(Layer* src, std::map<std::string, std::string> options) override;
    std::vector<FilterParameter> getParameters() override {
        return {
            INT_PARAM("quality", 1, 100, 50),
            INT_PARAM("iterations", 1, 100, 4),
        };
    }
};
class FilterAVIF : public BaseFilter {
public:
    std::string name() override { return "AVIF compression"; }
    std::string id() override { return "filter.avi"; }
    Layer* run(Layer* src, std::map<std::string, std::string> options) override;
    std::vector<FilterParameter> getParameters() override {
        return {
            INT_PARAM("quality", 1, 100, 50),
        };
    }
};

class FilterKernelTransformation : public BaseFilter {
protected:
    std::vector<std::vector<int>> kernel;
    int scale = 16;
    std::string n = "Kernel transformation";
    std::string i = "";
public:
    FilterKernelTransformation(std::string filterName, std::string i, std::vector<std::vector<int>> k) : n(filterName), i(i), kernel(k) {}

    std::string name() override { return n; }
    std::string id() override { return i; }
    Layer* run(Layer* src, std::map<std::string, std::string> options) override;
    std::vector<FilterParameter> getParameters() override {
        return {
            INT_PARAM("scale", 1, 64, 16),
        };
    }
};

class FilterOffset : public BaseFilter {
protected:
    XY lastLayerDims = { 100,100 };
public:
    std::string name() override { return "Offset"; }
    std::string id() override { return "filter.offset"; }
    void setupParamBounds(Layer* target) override;
    Layer* run(Layer* src, std::map<std::string, std::string> options) override;
    std::vector<FilterParameter> getParameters() override {
        return {
            INT_PARAM("offset.x", -lastLayerDims.x, lastLayerDims.x, 0),
            INT_PARAM("offset.y", -lastLayerDims.y, lastLayerDims.y, 0),
            BOOL_PARAM("wrap", 1)
        };
    }
};
class FilterRemoveChannels : public BaseFilter {
public:
    std::string name() override { return "Remove channels"; }
    std::string id() override { return "filter.removechannels"; }
    Layer* run(Layer* src, std::map<std::string, std::string> options) override;
    std::vector<FilterParameter> getParameters() override {
        return {
            BOOL_PARAM("remove.r", 0),
            BOOL_PARAM("remove.g", 0),
            BOOL_PARAM("remove.b", 0),
            BOOL_PARAM("remove.a", 0)
        };
    }
};

class FilterAlphaThreshold : public BaseFilter {
public:
    std::string name() override { return "Alpha threshold"; }
    std::string id() override { return "filter.alphathreshold"; }
    Layer* run(Layer* src, std::map<std::string, std::string> options) override;
    std::vector<FilterParameter> getParameters() override {
        return {
            INT_PARAM("threshold", 0, 255, 128)
        };
    }
};

class FilterEdgeDetectOutline : public FilterKernelTransformation {
protected:
    std::vector<std::vector<int>> kernel;
    int scale = 16;
public:
    FilterEdgeDetectOutline() : FilterKernelTransformation("", "",
        { 
            {0,-1,0},
            {-1,5,-1},
            {0,-1,0} 
        }) {}

    std::string name() override { return "Edge detect outline"; }
    std::string id() override { return "filter.edgedetectoutline"; }
    Layer* run(Layer* src, std::map<std::string, std::string> options) override;
    std::vector<FilterParameter> getParameters() override {
        return {
            INT_PARAM("scale", 1, 64, 16),
            FLOAT_PARAM("threshold", 0, 100, 60),
            COLORRGB_PARAM("color", 0xFFFFFFFF),
        };
    }
};

class FilterBlendTowardsColor : public BaseFilter {
public:
    std::string name() override { return "Blend towards color"; }
    std::string id() override { return "filter.blendtocolor"; }
    Layer* run(Layer* src, std::map<std::string, std::string> options) override;
    std::vector<FilterParameter> getParameters() override {
        return {
            COLORRGB_PARAM("color", 0xFF000000),
            FLOAT_PARAM("factor", 0, 100, 50)
        };
    }
};
