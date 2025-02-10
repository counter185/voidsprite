#pragma once
#include "globals.h"

#define INT_PARAM(name,min,max,def) (FilterParameter{name,min,max,def,PT_INT})
#define FLOAT_PARAM(name,min,max,def) (FilterParameter{name,min,max,def,PT_FLOAT})
#define COLORRGB_PARAM(name,def) (FilterParameter{name,min,max,def,PT_COLOR_RGB})
#define COLORL_PARAM(name) (FilterParameter{name,0,255,127,PT_COLOR_L})
#define BOOL_PARAM(name,def) (FilterParameter{name,0,1,def,PT_BOOL})

enum ParameterType {
	PT_INT = 0,
	PT_FLOAT = 1,
	PT_COLOR_RGB = 2,
	PT_COLOR_L = 3,
	PT_BOOL = 4,
};

struct FilterParameter {
	std::string name;
	double minValue = 0;
	double maxValue = 1;
	double defaultValue = 0.5;
	ParameterType paramType;
};

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