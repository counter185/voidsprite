#include "globals.h"
#include "BaseFilter.h"
#include "RenderFilter.h"
#include "Layer.h"

void g_loadFilters()
{
    g_filters.push_back(new FilterBlur());
    g_filters.push_back(new FilterSwapRGBToBGR());
    g_filters.push_back(new FilterAdjustHSV());
    g_filters.push_back(new FilterForEachPixel("Invert", [](XY, Layer*, u32 px) {
        SDL_Color pxnow = uint32ToSDLColor(px);
        return PackRGBAtoARGB(255 - pxnow.r, 255 - pxnow.g, 255 - pxnow.b, pxnow.a);
    }));
    g_filters.push_back(new FilterStrideGlitch());
    g_filters.push_back(new FilterPixelize());
    g_filters.push_back(new FilterOutline());
    g_filters.push_back(new FilterBrightnessContrast());


    g_renderFilters.push_back(new GenNoiseFilter());
    g_renderFilters.push_back(new GenRGBNoiseFilter());
}

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

Layer* FilterStrideGlitch::run(Layer* src, std::map<std::string, std::string> options)
{
    int splits = ixmax(1, ixmin(std::stod(options["splits"]), src->h));
    int lengthMin = std::stoi(options["length.min"]);
    int lengthMax = std::stoi(options["length.max"]);
    Layer* c = copy(src);
    int h = c->h;
    std::stack<int> splitPoints;
    int ch = h;
    int hFragment = h / splits;
    ch -= hFragment;
    for (int i = 0; i < splits; i++) {
        int splitDistance = randomInt(0, hFragment);
        int wDistance = randomInt(0, c->w);
        int hPoint = ch + splitDistance;
        ch -= hFragment;
        splitPoints.push(c->w * hPoint + wDistance);
    }
    u32* ppx = (u32*)c->pixelData;
    u32* srcPpx = (u32*)src->pixelData;
    u64 dataPtr = 0;
    u64 srcDataPtr = 0;
    int currentRepeats = 0;
    int strideShiftSum = 0;
    int lastRepeats = 0;
    while (dataPtr < c->w * c->h && srcDataPtr < c->w * c->h) {
        if (!splitPoints.empty() && dataPtr == splitPoints.top()) {
            splitPoints.pop();
            currentRepeats += randomInt(lengthMin, lengthMax);
            lastRepeats += currentRepeats;
        }
        if (dataPtr % c->w == 0) {
            srcDataPtr += lastRepeats;
            lastRepeats = 0;
        }
        ppx[dataPtr] = srcPpx[srcDataPtr];
        dataPtr++;
        if (currentRepeats == 0) {
            srcDataPtr++;
        }
        else {
            currentRepeats--;
        }
    }

    return c;
}

Layer* FilterPixelize::run(Layer* src, std::map<std::string, std::string> options)
{
    Layer* c = copy(src);
    XY pixelSize = { std::stod(options["size.x"]), std::stod(options["size.y"]) };
    int tilesW = (int)ceil(c->w / (double)pixelSize.x);
    int tilesH = (int)ceil(c->h / (double)pixelSize.y);

    for (int y = 0; y < tilesH; y++) {
        for (int x = 0; x < tilesW; x++) {

            XY origin = { x * pixelSize.x, y * pixelSize.y };

            u64 rs = 0, gs = 0, bs = 0, as = 0;
            double aSum = 0;
            int measures = 0;
            for (int yy = 0; yy < pixelSize.y; yy++) {
                for (int xx = 0; xx < pixelSize.x; xx++) {
                    if (pointInBox(xyAdd(origin, { xx,yy }), { 0,0,c->w,c->h })) {
                        measures++;
                        SDL_Color px = uint32ToSDLColor(src->getPixelAt(xyAdd(origin, {xx, yy}), true));
                        rs += px.r;
                        gs += px.g;
                        bs += px.b;
                        as += px.a;
                        aSum += px.a / 255.0;
                    }
                }
            }
            rs = aSum > 0 ? (u64)(rs / aSum) : 0;
            gs = aSum > 0 ? (u64)(gs / aSum) : 0;
            bs = aSum > 0 ? (u64)(bs / aSum) : 0;
            as /= measures;
            c->fillRect(origin, xyAdd(origin, pixelSize), PackRGBAtoARGB(rs, gs, bs, as));

        }
    }

    return c;
}

Layer* FilterOutline::run(Layer* src, std::map<std::string, std::string> options)
{
    Layer* c = copy(src);
    int iterations = std::stoi(options["thickness"]);
    bool corners = std::stoi(options["corners"]);
    u32 activeColor = std::stoul(options["!editor:activecolor"]);

    for (int i = 0; i < iterations; i++) {
        uint8_t* placePixelData = (uint8_t*)tracked_malloc(c->w * c->h);

        for (int y = 0; y < c->h; y++) {
            for (int x = 0; x < c->w; x++) {
                ARRAY2DPOINT(placePixelData, x, y, c->w) =
                    (c->getPixelAt({ x,y }) & 0xFF000000) == 0 ? 0 : 1;
            }
        }

        XY neighbors[8] = { 
            {1,0}, {0,1}, {-1,0}, {0,-1},
            {1,1}, {-1,1}, {1,-1}, {-1,-1}
        };

        for (int y = 0; y < c->h; y++) {
            for (int x = 0; x < c->w; x++) {
                XY newPos = { x,y };
                if (ARRAY2DPOINT(placePixelData, newPos.x, newPos.y, c->w) == 1) {
                    continue;
                }

                int neighborCount = 0;
                for (int i = 0; i < (corners ? 8 : 4); i++) {
                    XY neighbor = neighbors[i];
                    XY checkPos = xyAdd(newPos, neighbor);
                    if (pointInBox(checkPos, { 0,0,c->w,c->h })) {
                        if (ARRAY2DPOINT(placePixelData, checkPos.x, checkPos.y, c->w)) {
                            neighborCount++;
                        }
                    }
                }
                if (neighborCount > 0) {
                    c->setPixel(newPos, activeColor);
                }
            }
        }

        tracked_free(placePixelData);
    }

    return c;
}

Layer* FilterBrightnessContrast::run(Layer* src, std::map<std::string, std::string> options)
{
    Layer* c = copy(src);
    double brightness = std::stod(options["brightness"]);
    double contrast = std::stod(options["contrast"]);

    u32* ppx = (u32*)c->pixelData;
    for (u64 x = 0; x < c->w * c->h; x++) {
        SDL_Color px = uint32ToSDLColor(ppx[x]);
        px.r = ixmax(0, ixmin(255, (int)(px.r * contrast + brightness)));
        px.g = ixmax(0, ixmin(255, (int)(px.g * contrast + brightness)));
        px.b = ixmax(0, ixmin(255, (int)(px.b * contrast + brightness)));
        ppx[x] = sdlcolorToUint32(px);
    }

    return c;
}
