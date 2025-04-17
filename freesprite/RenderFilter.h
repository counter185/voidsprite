#pragma once
#include "BaseFilter.h"
#include "Layer.h"

/// <summary>
/// BaseFilter that renders something instead of manipulating pixel data. May or may not use what's already in the layer.
/// </summary>
class RenderFilter : public BaseFilter
{};

class GenNoiseFilter : public RenderFilter 
{
public:
    std::string name() override { return "Monochrome noise"; }
    Layer* run(Layer* src, std::map<std::string, std::string> options) override {
        bool grayscale = std::stod(options["grayscale"]) == 1;
        bool black_to_alpha = std::stod(options["black is alpha"]) == 1;
        u8 range_min = std::stoul(options["range.min"]);
        u8 range_max = std::stoul(options["range.max"]);

        Layer* c = copy(src);
        u32* px = (u32*)c->pixelData;

        for (int i = 0; i < c->w * c->h; i++) {
            if (grayscale) {
                u8 r = range_min == range_max ? range_max : (rand() % (range_max - range_min)) + range_min;
                px[i] = black_to_alpha ? PackRGBAtoARGB(0xff, 0xff, 0xff, r) : PackRGBAtoARGB(r, r, r, 0xFF);
            }
            else {
                u8 r = range_min == range_max ? range_max : rand() % 2 == 0 ? range_min : range_max;
                px[i] = black_to_alpha ? PackRGBAtoARGB(0xff, 0xff, 0xff, r) : PackRGBAtoARGB(r, r, r, 0xFF);
            }
        }

        return c;
    }
    std::vector<FilterParameter> getParameters() override { return {
        BOOL_PARAM("grayscale", 1),
        BOOL_PARAM("black is alpha", 1),
        INT_RANGE_PARAM("range", 0, 255, 0, 255, 0xffffffff),
    }; }

};

class GenRGBNoiseFilter : public RenderFilter 
{
public:
    std::string name() override { return "RGBA noise"; }
    Layer* run(Layer* src, std::map<std::string, std::string> options) override {
        
        int rmin = std::stoi(options["red.min"]), rmax = std::stoi(options["red.max"]);
        int gmin = std::stoi(options["green.min"]), gmax = std::stoi(options["green.max"]);
        int bmin = std::stoi(options["blue.min"]), bmax = std::stoi(options["blue.max"]);
        int amin = std::stoi(options["alpha.min"]), amax = std::stoi(options["alpha.max"]);
        if (rmax > rmin) {
            int t = rmax;
            rmax = rmin;
            rmin = t;
        }
        if (gmax > gmin) {
            int t = gmax;
            gmax = gmin;
            gmin = t;
        }
        if (bmax > bmin) {
            int t = bmax;
            bmax = bmin;
            bmin = t;
        }
        if (amax > amin) {
            int t = amax;
            amax = amin;
            amin = t;
        }

        Layer* c = copy(src);
        u32* px = (u32*)c->pixelData;

        for (int i = 0; i < c->w * c->h; i++) {
            px[i] = PackRGBAtoARGB(
                randomInt(rmin, rmax),
                randomInt(gmin, gmax),
                randomInt(bmin, bmax),
                randomInt(amin, amax)
            );
        }
        return c;
    }
    std::vector<FilterParameter> getParameters() override { return {
        // INT_PARAM("red.min", 0, 255, 0),
        // INT_PARAM("red.max", 0, 255, 255),
        // INT_PARAM("green.min", 0, 255, 0),
        // INT_PARAM("green.max", 0, 255, 255),
        // INT_PARAM("blue.min", 0, 255, 0),
        // INT_PARAM("blue.max", 0, 255, 255),
        // INT_PARAM("alpha.min", 0, 255, 255),
        // INT_PARAM("alpha.max", 0, 255, 255),

        INT_RANGE_PARAM("red", 0, 255, 0, 255, 0xffff0000),
        INT_RANGE_PARAM("green", 0, 255, 0, 255, 0xff00ff00),
        INT_RANGE_PARAM("blue", 0, 255, 0, 255, 0xff0000ff),
        INT_RANGE_PARAM("alpha", 0, 255, 0, 255, 0xffffffff),
    }; }

};

class PrintPaletteFilter : public RenderFilter {
    std::string name() override { return "Render palette"; }
    Layer* run(Layer* src, std::map<std::string, std::string> options) override { 
        Layer* c = copy(src);
        int columns = std::stoi(options["columns"]);
        XY sqDim = {std::stoi(options["square.w"]), std::stoi(options["square.h"])};
        auto cols = src->getUniqueColors(true);
        int ynow = 0;
        int xnow = 0;
        int limit = 1000;
        for (auto& cc : cols) {
            XY tileOrigin = {xnow * sqDim.x, ynow * sqDim.y};
            c->fillRect(tileOrigin, xyAdd(tileOrigin, xySubtract(sqDim, {1,1})), cc);
            if (xnow++ == columns) {
                xnow = 0;
                ynow++;
            }
        }
        return c;
    }

    std::vector<FilterParameter> getParameters() override {
        return {
            INT_PARAM("columns", 1, 100, 16), 
            INT_PARAM("square.w", 1, 16, 1),
            INT_PARAM("square.h", 1, 16, 1)
        };
    }
};
