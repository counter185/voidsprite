#pragma once
#include "mathops.h"

class ColorModel {
public:
    virtual std::vector<std::string> components() = 0;
    virtual std::map<std::string, std::pair<double, double>> componentRanges() {
        std::map<std::string, std::pair<double, double>> r;
        for (auto& c : components()) {
            r[c] = {0.0, 1.0};
        }
        return r;
    }
    virtual std::map<std::string, double> fromRGB(u32 color) = 0;
    virtual u32 toRGB(std::map<std::string, double> values) = 0;
};

class CMYKColorModel : public ColorModel {
public:
    std::vector<std::string> components() override {
        return { "C", "M", "Y", "K" };
    }
    std::map<std::string, double> fromRGB(u32 color) override {
        rgb rgb = u32ToRGB(color);
        double r = rgb.r;
        double g = rgb.g;
        double b = rgb.b;
        double k = 1 - dxmax(dxmax(r, g), b);
        double c = k != 1 ? (1 - r - k) / (1 - k) : 0;
        double m = k != 1 ? (1 - g - k) / (1 - k) : 0;
        double y = k != 1 ? (1 - b - k) / (1 - k) : 0;
        return {
            {"C", c},
            {"M", m},
            {"Y", y},
            {"K", k}
        };
    }
    u32 toRGB(std::map<std::string, double> values) override {
        double c = values["C"];
        double m = values["M"];
        double y = values["Y"];
        double k = values["K"];
        double r = 255 * (1 - c) * (1 - k);
        double g = 255 * (1 - m) * (1 - k);
        double b = 255 * (1 - y) * (1 - k);
        return PackRGBAtoARGB((u8)r, (u8)g, (u8)b, 255);
    }
};

class HSLColorModel : public ColorModel {
public:
    std::vector<std::string> components() override { return {"H", "S", "L"}; }
    std::map<std::string, std::pair<double, double>> componentRanges() override {
        return {
            {"H", {0, 360}},
            {"S", {0,1}},
            {"L", {0,1}}
        };
    };
    std::map<std::string, double> fromRGB(u32 color) override {
        rgb rgb = u32ToRGB(color);
        hsl hsl = rgb2hsl(rgb);
        return {
            {"H", hsl.h*360.0},
            {"S", hsl.s},
            {"L", hsl.l}
        };
    }
    u32 toRGB(std::map<std::string, double> values) override {
        hsl hsl;
        hsl.h = values["H"] / 360.0;
        hsl.s = values["S"];
        hsl.l = values["L"];
        rgb rgb = hsl2rgb(hsl);
        return PackRGBAtoARGB((u8)(rgb.r * 255), (u8)(rgb.g * 255), (u8)(rgb.b * 255), 255);
    }
};

class YCbCrColorModel : public ColorModel {
public:
    std::vector<std::string> components() override { return {"Y", "Cb", "Cr"}; }

    std::map<std::string, std::pair<double, double>> componentRanges() override {
        std::map<std::string, std::pair<double, double>> r;
        r["Y"] = {0.0, 1.0};
        r["Cb"] = {-0.5, 0.5};
        r["Cr"] = {-0.5, 0.5};

        return r;
    }

    std::map<std::string, double> fromRGB(u32 color) override {
        rgb c = u32ToRGB(color);

        double y = 0.299 * c.r + 0.587 * c.g + 0.114 * c.b;
        double cb = -0.1687 * c.r - 0.3313 * c.g + 0.5 * c.b;
        double cr = 0.5 * c.r - 0.4187 * c.g - 0.0813 * c.b;

        return {
            {"Y",  y },
            {"Cb", cb},
            {"Cr", cr}
        };
    }

    u32 toRGB(std::map<std::string, double> values) override {
        double y = values["Y"];
        double cb = values["Cb"];
        double cr = values["Cr"];

        double r = y + 1.402 * (cr);
        double g = y - 0.34414 * (cb) - 0.71414 * (cr);
        double b = y + 1.772 * (cb);

        r = clamp(r, 0.0, 1.0);
        g = clamp(g, 0.0, 1.0);
        b = clamp(b, 0.0, 1.0);

        return PackRGBAtoARGB(r*255, g*255, b*255, 255);
    }

private:
    double clamp(double x, double min, double max) { return dxmax(min, dxmin(x, max)); }
};


inline std::unordered_map<std::string, ColorModel*> g_colorModels;

inline void g_setupColorModels() {
    g_colorModels["CMYK"] = new CMYKColorModel();
    g_colorModels["HSL"] = new HSLColorModel();
    g_colorModels["YCbCr"] = new YCbCrColorModel();
}