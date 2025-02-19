#include "BaseFilter.h"
#include "Layer.h"

Layer* BaseFilter::copy(Layer* src)
{
    if (src == NULL) {
        return NULL;
    }
    return src->copyWithNoTextureInit();
}

Layer* FilterBlur::run(Layer* src, std::map<std::string, std::string> options)
{
    Layer* c = copy(src);
    int radiusX = options.contains("radius.x") ? std::stod(options["radius.x"]) : 4;
    int radiusY = options.contains("radius.y") ? std::stod(options["radius.y"]) : 4;
    for (int y = 0; y < c->h; y++) {
        for (int x = 0; x < c->w; x++) {

            SDL_Color pxnow = uint32ToSDLColor(src->getPixelAt({ x,y }, true));
            double r = pxnow.r;
            double g = pxnow.g;
            double b = pxnow.b;
            u64 a = pxnow.a;
            int measures = 1;
            double aSum = pxnow.a / 255.0f;

            for (int rx = 1; rx < radiusX; rx++) {
                for (const int& m : { -1,1 }) {
                    if ((x + rx * m < c->w) && (x + rx*m >= 0)) {
                        SDL_Color px = uint32ToSDLColor(src->getPixelAt({ x + rx*m,y }, true));
                        double alpha = px.a / 255.0;
                        r += px.r * alpha;
                        g += px.g * alpha;
                        b += px.b * alpha;
                        a += px.a;
                        aSum += alpha;
                        measures++;
                    }
                }
            }

            for (int ry = 1; ry < radiusY; ry++) {
                for (const int& m : { -1,1 }) {
                    if ((y + ry * m < c->h) && (y + ry*m >= 0)) {
                        SDL_Color px = uint32ToSDLColor(src->getPixelAt({ x,y + ry*m }, true));
                        double alpha = px.a / 255.0;
                        r += px.r * alpha;
                        g += px.g * alpha;
                        b += px.b * alpha;
                        a += px.a;
                        aSum += alpha;
                        measures++;
                    }
                }
            }
            if (aSum > 0) {
                r /= aSum;
                g /= aSum;
                b /= aSum;
            }
            else {
                r = 0;
                g = 0;
                b = 0;
            }
            a /= measures;
            c->setPixel({ x,y }, PackRGBAtoARGB((u8)r, (u8)g, (u8)b, a));
        }
    }
    return c;
}

Layer* FilterForEachPixel::run(Layer* src, std::map<std::string, std::string> options)
{
    Layer* c = copy(src);
    for (int y = 0; y < c->h; y++) {
        for (int x = 0; x < c->w; x++) {
            u32 px = src->getPixelAt({x, y}, true);
            c->setPixel({ x,y }, f({ x,y }, src, px));
        }
    }
    return c;
}

Layer* FilterSwapRGBToBGR::run(Layer* src, std::map<std::string, std::string> options)
{
    Layer* c = copy(src);
    SDL_ConvertPixels(c->w, c->h, SDL_PIXELFORMAT_ARGB8888, src->pixelData, c->w * 4, SDL_PIXELFORMAT_ABGR8888, c->pixelData, c->w * 4);
    return c;
}

Layer* FilterAdjustHSV::run(Layer* src, std::map<std::string, std::string> options)
{
    double h = std::stod(options["hue"]);
    double s = std::stod(options["saturation"]) / 100.0;
    double v = std::stod(options["value"]) / 100.0;
    hsv hsvv = { h,s,v };
    Layer* c = copy(src);
    for (int y = 0; y < c->h; y++) {
        for (int x = 0; x < c->w; x++) {
            u32 px = src->getPixelAt({ x, y }, true);
            c->setPixel({ x,y }, hsvShift(px, hsvv));
        }
    }
    return c;
}
