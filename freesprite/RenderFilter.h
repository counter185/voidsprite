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
        Layer* c = copy(src);
        u32* px = (u32*)c->pixelData;

        for (int i = 0; i < c->w * c->h; i++) {
            if (grayscale) {
                u8 r = rand() % 256;
                px[i] = black_to_alpha ? PackRGBAtoARGB(0xff,0xff,0xff, r) : PackRGBAtoARGB(r, r, r, 0xFF);
            }
            else {
                px[i] = rand() % 2 == 0 ? (black_to_alpha ? 0x00000000 : 0xFF000000) : 0xFFFFFFFF;
            }
        }
        return c;
    }
    virtual std::vector<FilterParameter> getParameters() { return {
        BOOL_PARAM("grayscale", 1),
        BOOL_PARAM("black is alpha", 1),
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
    virtual std::vector<FilterParameter> getParameters() { return {
        INT_PARAM("red.min", 0, 255, 0),
        INT_PARAM("red.max", 0, 255, 255),
        INT_PARAM("green.min", 0, 255, 0),
        INT_PARAM("green.max", 0, 255, 255),
        INT_PARAM("blue.min", 0, 255, 0),
        INT_PARAM("blue.max", 0, 255, 255),
        INT_PARAM("alpha.min", 0, 255, 255),
        INT_PARAM("alpha.max", 0, 255, 255),
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

    virtual std::vector<FilterParameter> getParameters() {
        return {
            INT_PARAM("columns", 1, 100, 16), 
            INT_PARAM("square.w", 1, 16, 1),
            INT_PARAM("square.h", 1, 16, 1)
        };
    }
};