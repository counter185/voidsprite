#pragma once
#include "globals.h"

#define INT_PARAM(name,min,max,def) (FilterParameter{name,min,max,def,PT_INT})
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

class BaseFilter
{
public:
    virtual std::string name() { return "Filter"; }

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
    std::function<u32(XY, Layer*, u32)> f;
    std::vector<FilterParameter> fParams;
public:
    FilterForEachPixel(std::string name, std::function<u32(XY, Layer*, u32)> f, std::vector<FilterParameter> fParams = {}) {
        this->fName = name;
        this->f = f;
        this->fParams = fParams;
    }

    std::string name() override { return fName; }
    Layer* run(Layer* src, std::map<std::string, std::string> options) override;
    std::vector<FilterParameter> getParameters() override { return fParams; }
};

class FilterBlur : public BaseFilter
{
public:
    std::string name() override { return "Blur"; }

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
    Layer* run(Layer* src, std::map<std::string, std::string> options) override;
};

class FilterAdjustHSV : public BaseFilter {
public:
    std::string name() override { return "Adjust HSV"; }
    Layer* run(Layer* src, std::map<std::string, std::string> options) override;
    std::vector<FilterParameter> getParameters() override {
        return {
            FLOAT_PARAM("hue", -360, 360, 0),
            FLOAT_PARAM("saturation", -100, 100, 0),
            FLOAT_PARAM("value", -100, 100, 0),
        };
    }
};

class FilterStrideGlitch : public BaseFilter {
public:
    std::string name() override { return "Stride glitch"; }
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
    Layer* run(Layer* src, std::map<std::string, std::string> options) override;
    std::vector<FilterParameter> getParameters() override {
        return {
            INT_PARAM("size.x", 1, 100, 2),
            INT_PARAM("size.y", 1, 100, 2),
        };
    }
};

class FilterOutline : public BaseFilter {
public:
    std::string name() override { return "Outline"; }
    Layer* run(Layer* src, std::map<std::string, std::string> options) override;
    std::vector<FilterParameter> getParameters() override {
        return {
            INT_PARAM("thickness", 1, 50, 1),
            BOOL_PARAM("corners", 0)
        };
    }
};

class FilterBrightnessContrast : public BaseFilter {
public:
    std::string name() override { return "Brightness/contrast"; }
    Layer* run(Layer* src, std::map<std::string, std::string> options) override;
    std::vector<FilterParameter> getParameters() override {
        return {
            INT_PARAM("brightness", -255, 255, 0),
            FLOAT_PARAM("contrast", 0, 2, 1)
        };
    }
};

class FilterQuantize : public BaseFilter {
public:
    std::string name() override { return "Quantize colors"; }
    Layer* run(Layer* src, std::map<std::string, std::string> options) override;
    std::vector<FilterParameter> getParameters() override {
        return {
            INT_PARAM("num.colors", 1, 256, 128),
        };
    }
};
