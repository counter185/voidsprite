#include <stack>

#include <SDL3_image/SDL_image.h>

#include "globals.h"
#include "BaseFilter.h"
#include "RenderFilter.h"
#include "Layer.h"
#include "io/io_avif.h"
#include "json/json.hpp"

std::map<std::string, BaseFilter*> filterIDMap;

void registerFilter(BaseFilter* f, std::vector<BaseFilter*>* target = &g_filters)
{
    if (filterIDMap.contains(f->id())) {
        logwarn(frmt("duplicate filter id: {}", f->id()));
    }
    else if (f->id() == "filter.default" || f->id() == "") {
        logwarn(frmt("invalid filter id: {} ({})", f->id(), f->name()));
    }
    target->push_back(f);
    filterIDMap[f->id()] = f;
}
BaseFilter* g_getFilterByID(std::string id)
{
    if (filterIDMap.contains(id)) {
        return filterIDMap[id];
    }
    return NULL;
}

void g_loadFilters()
{
    registerFilter(new FilterBlur());
    registerFilter(new FilterSwapRGBToBGR());
    registerFilter(new FilterAdjustHSV());
    registerFilter(new FilterForEachPixel("Invert", "filter.invert", [](XY, Layer*, u32 px) {
        SDL_Color pxnow = uint32ToSDLColor(px);
        return PackRGBAtoARGB(255 - pxnow.r, 255 - pxnow.g, 255 - pxnow.b, pxnow.a);
    }));
    registerFilter(new FilterStrideGlitch());
    registerFilter(new FilterJPEGGlitch());
    registerFilter(new FilterPixelize());
    registerFilter(new FilterOutline());
    registerFilter(new FilterBrightnessContrast());
    registerFilter(g_filter_quantize = new FilterQuantize());
    registerFilter(new FilterJPEG());
    registerFilter(new FilterAVIF());
    /*registerFilter(new FilterKernelTransformation("Kernel blur", {
        {1,1,1,1,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,0,0,0,1},
        {1,1,1,1,1}
    }));*/
    registerFilter(g_filter_edgeDetect = new FilterKernelTransformation("Kernel edge detect", "filter.kerneledgedetect", {
        {0,-1,0},
        {-1,5,-1},
        {0,-1,0}
    }));
    registerFilter(new FilterOffset());
    registerFilter(new FilterRemoveChannels());
    registerFilter(new FilterAlphaThreshold());
    registerFilter(new FilterEdgeDetectOutline());
    registerFilter(new FilterBlendTowardsColor());

    for (auto pluginFilter : g_pluginFilters) {
        registerFilter(pluginFilter);
    }

    registerFilter(new GenNoiseFilter(), &g_renderFilters);
    registerFilter(new GenRGBNoiseFilter(), &g_renderFilters);
    registerFilter(new PrintPaletteFilter(), &g_renderFilters);
    registerFilter(new RGBGeneFilter(), &g_renderFilters);
    registerFilter(new RenderGridFilter(), &g_renderFilters);
}

std::string FilterPreset::serialize() {
    nlohmann::json j = nlohmann::json::object();
    j["filterID"] = filterID;
    nlohmann::json optionsJSON = nlohmann::json::object();
    for (auto& [k, v] : this->options) {
        optionsJSON[k] = v;
    }
    j["options"] = optionsJSON;
    return j.dump();
}

FilterPreset FilterPreset::deserialize(std::string s) {
    nlohmann::json j = nlohmann::json::parse(s);
    std::map<std::string, std::string> options;
    for (auto& [k, v] : j["options"].items()) {
        options[k] = v;
    }
    return FilterPreset(j["filterID"], options);
}

Layer* BaseFilter::copy(Layer* src)
{
    if (src == NULL) {
        return NULL;
    }
    return src->copyCurrentVariant();
}

Layer* FilterBlur::run(Layer* src, ParameterStore* options)
{
    Layer* c = copy(src);
    int radiusX = options->getFloat("radius.x");//options.contains("radius.x") ? std::stod(options["radius.x"]) : 4;
    int radiusY = options->getFloat("radius.y");//options.contains("radius.y") ? std::stod(options["radius.y"]) : 4;
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

Layer* FilterForEachPixel::run(Layer* src, ParameterStore* options)
{
    Layer* c = copy(src);
    for (int y = 0; y < c->h; y++) {
        for (int x = 0; x < c->w; x++) {
            u32 px = src->getPixelAt({x, y}, true);
            c->setPixel({ x,y }, f({ x,y }, src, px, options));
        }
    }
    return c;
}

Layer* FilterSwapRGBToBGR::run(Layer* src, ParameterStore* options)
{
    Layer* c = copy(src);
    SDL_ConvertPixels(c->w, c->h, SDL_PIXELFORMAT_ARGB8888, src->pixels32(), c->w * 4, SDL_PIXELFORMAT_ABGR8888, c->pixels32(), c->w * 4);
    return c;
}

Layer* FilterAdjustHSV::run(Layer* src, ParameterStore* options)
{
    bool colorize = options->getBool("colorize"); //std::stoi(options["colorize"]);
    double h = options->getFloat("hue"); // std::stod(options["hue"]);
    double s = options->getFloat("saturation") / 100.0; // std::stod(options["saturation"]) / 100.0;
    double v = options->getFloat("value") / 100.0; // std::stod(options["value"]) / 100.0;
    hsv hsvv = { h,s,v };
    Layer* c = copy(src);
    for (int y = 0; y < c->h; y++) {
        for (int x = 0; x < c->w; x++) {
            u32 px = src->getPixelAt({ x, y }, true);
            if (colorize) {
                px = hsvShift(px, { 0, -1, 0 });
            }
            c->setPixel({ x,y }, hsvShift(px, hsvv));
        }
    }
    return c;
}

Layer* FilterStrideGlitch::run(Layer* src, ParameterStore* options)
{
    int splits = ixmax(1, ixmin(options->getInt("splits"), src->h));
    auto lengthRange = options->getIntRange("length");
    int lengthMin = lengthRange.first;
    int lengthMax = lengthRange.second;
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
    u32* ppx = c->pixels32();
    u32* srcPpx = src->pixels32();
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

Layer* FilterPixelize::run(Layer* src, ParameterStore* options)
{
    bool sampleOnePoint = options->getBool("one sample");// std::stoi(options["one sample"]);

    Layer* c = copy(src);
    XY pixelSize = { options->getInt("size.x"), options->getInt("size.y") };
    int tilesW = (int)ceil(c->w / (double)pixelSize.x);
    int tilesH = (int)ceil(c->h / (double)pixelSize.y);

    for (int y = 0; y < tilesH; y++) {
        for (int x = 0; x < tilesW; x++) {

            XY origin = { x * pixelSize.x, y * pixelSize.y };

            u32 outColor = 0;

            if (!sampleOnePoint) {
                u64 rs = 0, gs = 0, bs = 0, as = 0;
                double aSum = 0;
                int measures = 0;
                for (int yy = 0; yy < pixelSize.y; yy++) {
                    for (int xx = 0; xx < pixelSize.x; xx++) {
                        if (pointInBox(xyAdd(origin, { xx,yy }), { 0,0,c->w,c->h })) {
                            measures++;
                            SDL_Color px = uint32ToSDLColor(src->getPixelAt(xyAdd(origin, { xx, yy }), true));
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
                outColor = PackRGBAtoARGB(rs, gs, bs, as);
            }
            else {
                outColor = src->getPixelAt(origin, true);
            }

            c->fillRect(origin, xyAdd(origin, pixelSize), outColor);
        }
    }

    return c;
}

Layer* FilterOutline::run(Layer* src, ParameterStore* options)
{
    Layer* c = copy(src);
    int iterations = options->getInt("thickness");// std::stoi(options["thickness"]);
    bool corners = options->getBool("corners");// std::stoi(options["corners"]);
    u32 activeColor = options->getColorRGB("color"); //std::stoul(options["color"], 0, 16);

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

Layer* FilterBrightnessContrast::run(Layer* src, ParameterStore* options)
{
    Layer* c = copy(src);
    bool contrastFirst = options->getBool("contrast.first");// std::stoi(options["contrast.first"]);
    double brightness = options->getFloat("brightness");// std::stod(options["brightness"]);
    double contrast = options->getFloat("contrast");// std::stod(options["contrast"]);

    u32* ppx = c->pixels32();
    for (u64 x = 0; x < c->w * c->h; x++) {
        SDL_Color px = uint32ToSDLColor(ppx[x]);
        if (contrastFirst) {
            px.r = ixmax(0, ixmin(255, (int)(px.r * contrast + brightness)));
            px.g = ixmax(0, ixmin(255, (int)(px.g * contrast + brightness)));
            px.b = ixmax(0, ixmin(255, (int)(px.b * contrast + brightness)));
        }
        else {
            px.r = ixmax(0, ixmin(255, (int)((px.r + brightness) * contrast)));
            px.g = ixmax(0, ixmin(255, (int)((px.g + brightness) * contrast)));
            px.b = ixmax(0, ixmin(255, (int)((px.b + brightness) * contrast)));
        }
        ppx[x] = sdlcolorToUint32(px);
    }

    return c;
}

std::pair<std::vector<u32>, std::vector<u32>> evalMedianCutBucket(std::vector<u32> colors) {
    if (colors.size() == 0) {
        return { {}, {} };
    }
    u8 minR = (colors[0] & 0xFF0000) >> 16;
    u8 maxR = (colors[0] & 0xFF0000) >> 16;
    u8 minG = (colors[0] & 0x00FF00) >> 8;
    u8 maxG = (colors[0] & 0x00FF00) >> 8;
    u8 minB = (colors[0] & 0x0000FF);
    u8 maxB = (colors[0] & 0x0000FF);
    for (u32 c : colors) {
        SDL_Color cc = uint32ToSDLColor(c);
        minR = ixmin(minR, cc.r);
        maxR = ixmax(maxR, cc.r);
        minG = ixmin(minG, cc.g);
        maxG = ixmax(maxG, cc.g);
        minB = ixmin(minB, cc.b);
        maxB = ixmax(maxB, cc.b);
        if ((minR == 0 && maxR == 255) || (minG == 0 && maxG == 255) || (minB == 0 && maxB == 255)) {
            break;
        }
    }
    u8 rRange = maxR - minR;
    u8 gRange = maxG - minG;
    u8 bRange = maxB - minB;
    u8 maxRange = ixmax(ixmax(rRange, gRange), bRange);
    std::pair<std::vector<u32>, std::vector<u32>> ret;
    u32 midPoint = maxRange == rRange ? minR + rRange / 2 
                 : maxRange == gRange ? minG + gRange / 2 
                 : minB + bRange / 2;
    u32 mask = maxRange == rRange ? 0xff0000
             : maxRange == gRange ? 0x00ff00
             : 0xff;
    midPoint *= mask / 0xff;
    std::sort(colors.begin(), colors.end(), [mask](u32 a, u32 b) { return (a & mask) < (b & mask); });
    int indexMaxStart = -1;
    for (int x = 0; x < colors.size(); x++) {
        if ((colors[x] & mask) > midPoint) {
            indexMaxStart = x;
            break;
        }
    }
    if (indexMaxStart == -1) {
        ret.first = colors;
        ret.second = {};
    }
    else if (indexMaxStart == 0) {
        ret.first = {};
        ret.second = colors;
    }
    else {
        ret.first = std::vector<u32>(colors.begin(), colors.begin() + indexMaxStart);
        ret.second = std::vector<u32>(colors.begin() + indexMaxStart, colors.end());
    }
    return ret;
}
u32 evalBucketMean(std::vector<u32> colors) {
    if (colors.size() == 0) {
        return 0;
    }
    u64 r = 0, g = 0, b = 0, a = 0;
    for (u32 c : colors) {
        SDL_Color cc = uint32ToSDLColor(c);
        r += cc.r;
        g += cc.g;
        b += cc.b;
        a += cc.a;
    }
    return PackRGBAtoARGB(r / colors.size(), g / colors.size(), b / colors.size(), a / colors.size());
}

Layer* FilterQuantize::run(Layer* src, ParameterStore* options)
{
    int numColors = options->getInt("num.colors"); //std::stoi(options["num.colors"]);
    auto colors = src->getUniqueColors(true);
    if (colors.size() <= numColors) {
        return copy(src);
    }
    u32* pppx = src->pixels32();
    bool oneColorWillBeAlpha = numColors > 1 && std::any_of(pppx, pppx + src->w * src->h, [](u32 a) { return (a & 0xff000000) == 0; });
    if (oneColorWillBeAlpha) {
        numColors--;
    }
    std::vector<std::vector<u32>> buckets = {colors};
    while (buckets.size() < numColors) {
        std::sort(buckets.begin(), buckets.end(), [](std::vector<u32> a, std::vector<u32> b) {return a.size() > b.size(); });
        auto split = evalMedianCutBucket(buckets.front());
        buckets.erase(buckets.begin());
        if (split.first.size() > 0) {
            buckets.push_back(split.first);
        }
        if (split.second.size() > 0) {
            buckets.push_back(split.second);
        }
    }
    std::map<u32, u32> remap;
    for (auto& b : buckets) {
        u32 mean = evalBucketMean(b);
        for (u32 c : b) {
            remap[c&0xFFFFFF] = mean;
        }
    }
    Layer* c = copy(src);
    u32* ppx = c->pixels32();
    for (u64 x = 0; x < c->w * c->h; x++) {
        if (oneColorWillBeAlpha && (ppx[x] & 0xFF000000) == 0) {
            ppx[x] = 0;
        }
        else {
            ppx[x] = 0xFF000000 | remap[ppx[x] & 0xFFFFFF];
        }
    }
    return c;
}

Layer* FilterJPEG::run(Layer* src, ParameterStore* options)
{
    int quality = options->getInt("quality");// std::stoi(options["quality"]);
    int iterations = options->getInt("iterations");// std::stoi(options["iterations"]);
    Layer* c = copy(src);
    for (int i = 0; i < iterations; i++) {
        SDL_Surface* srf = SDL_CreateSurface(c->w, c->h, SDL_PIXELFORMAT_ARGB8888);
        if (srf != NULL) {
            SDL_LockSurface(srf);
            copyPixelsToTexture(c->pixels32(), c->w, c->h, (u8*)srf->pixels, srf->pitch);
            SDL_UnlockSurface(srf);
            SDL_IOStream* stream = SDLVectorU8IOStream::OpenNew();
            IMG_SaveJPG_IO(srf, stream, false, quality);
            SDL_DestroySurface(srf);
            SDL_SeekIO(stream, 0, SDL_IO_SEEK_SET);
            srf = IMG_LoadJPG_IO(stream);
            SDL_CloseIO(stream);

            if (srf != NULL) {
                SDL_Surface* srf2 = SDL_ConvertSurface(srf, SDL_PIXELFORMAT_ARGB8888);
                SDL_DestroySurface(srf);
                srf = srf2;
                for (int y = 0; y < ixmin(srf->h, c->h); y++) {
                    memcpy(&(ARRAY2DPOINT(c->pixels32(), 0, y, c->w)), &(ARRAY2DPOINT((u8*)srf->pixels, 0, y, srf->pitch)), ixmin(srf->w, c->w) * 4);
                }
            }
            SDL_FreeSurface(srf);
        }
    }
    return c;

}
Layer* FilterJPEGGlitch::run(Layer* src, ParameterStore* options)
{
    int quality =    options->getInt("quality");// std::stoi(options["quality"]);
    int iterations = options->getInt("iterations");// std::stoi(options["iterations"]);
    Layer* c = copy(src);
    SDL_Surface* srf = SDL_CreateSurface(c->w, c->h, SDL_PIXELFORMAT_ARGB8888);
    if (srf != NULL) {
        SDL_LockSurface(srf);
        copyPixelsToTexture(c->pixels32(), c->w, c->h, (u8*)srf->pixels, srf->pitch);
        SDL_UnlockSurface(srf);
        SDLVectorU8IOStream* rawStream = NULL;
        SDL_IOStream* stream = SDLVectorU8IOStream::OpenNew(&rawStream);
        IMG_SaveJPG_IO(srf, stream, false, quality);
        SDL_DestroySurface(srf);
        std::vector<u8>& v = rawStream->data;
        std::vector<u8> jpegSOS = { 0xFF, 0xDA };
        std::vector<u8> jpegEOI = { 0xFF, 0xD9 };
        int sosOffset = 0x10;
        auto posSOS = std::search(v.begin(), v.end(), std::begin(jpegSOS), std::end(jpegSOS));
        auto posEOI = std::search(v.begin(), v.end(), std::begin(jpegEOI), std::end(jpegEOI));
        if (posSOS != v.end() && posEOI != v.end() && posSOS + sosOffset < posEOI) {
            int idxSOS = posSOS - v.begin();
            int idxEOI = posEOI - v.begin();
            idxSOS += sosOffset;
            int sizeFromEOItoSOS = (int)(idxEOI - idxSOS);
            for (int i = 0; i < iterations; i++) {
                int glitchPos = randomInt(0, sizeFromEOItoSOS);
                int insertChars = randomInt(1, 4);
                for (int j = 0; j < insertChars; j++) {
                    v.insert(v.begin() + idxSOS + glitchPos, (u8)randomInt(0, 127));    //don't accidentally insert a new marker hopefully
                }
            }
        } else {
            logerr("[FilterJPEGGlitch] no SOS or EOI markers");
        }

        SDL_SeekIO(stream, 0, SDL_IO_SEEK_SET);
        srf = IMG_LoadJPG_IO(stream);
        if (srf == NULL) {
            logerr("[FilterJPEGGlitch] failed to load glitched JPEG");
        }
        SDL_CloseIO(stream);

        if (srf != NULL) {
            SDL_Surface* srf2 = SDL_ConvertSurface(srf, SDL_PIXELFORMAT_ARGB8888);
            SDL_DestroySurface(srf);
            srf = srf2;
            for (int y = 0; y < ixmin(srf->h, c->h); y++) {
                memcpy(&(ARRAY2DPOINT(c->pixels32(), 0, y, c->w)), &(ARRAY2DPOINT((u8*)srf->pixels, 0, y, srf->pitch)), ixmin(srf->w, c->w) * 4);
            }
        }
        SDL_FreeSurface(srf);
    }
    return c;

}

Layer* FilterAVIF::run(Layer* src, ParameterStore* options)
{
    int quality = options->getInt("quality");//std::stoi(options["quality"]);
    Layer* c = copy(src);

    auto memAvif = writeAVIFToMem(c, quality);
    SDL_IOStream* stream = SDL_IOFromMem(memAvif.data(), memAvif.size());
    SDL_Surface* srf = readAVIFFromMem(memAvif.data(), memAvif.size());
    if (srf != NULL) {
        DoOnReturn freeSrf([srf]() { SDL_DestroySurface(srf); });

        for (int y = 0; y < ixmin(srf->h, c->h); y++) {
            memcpy(&(ARRAY2DPOINT(c->pixels32(), 0, y, c->w)), &(ARRAY2DPOINT((u8*)srf->pixels, 0, y, srf->pitch)), ixmin(srf->w, c->w) * 4);
        }
    }
    return c;
}

Layer* FilterKernelTransformation::run(Layer* src, ParameterStore* options)
{
    Layer* c = copy(src);
    int kerH = kernel.size();
    int kerW = kernel[0].size();
    int scale = options->getInt("scale");// (int)std::stod(options["scale"]); //this->scale;
    int w = c->w;
    int h = c->h;
    int k = kerH;
    int d = k / 2;
    for (int x = d; x < w - d; x++) {
        for (int y = d; y < h - d; y++) {
            int temp[4] = {0,0,0,0};

            for (int a = 0; a < k; a++) {
                for (int b = 0; b < k; b++) {
                    auto xn = x + a - d;
                    auto yn = y + b - d;
                    SDL_Color pixel = uint32ToSDLColor(src->getPixelAt({ xn, yn }));
                    temp[0] += pixel.r * kernel[a][b];
                    temp[1] += pixel.g * kernel[a][b];
                    temp[2] += pixel.b * kernel[a][b];
                    temp[3] += pixel.a * kernel[a][b];
                }
            }

            temp[0] /= scale;
            temp[1] /= scale;
            temp[2] /= scale;
            temp[3] /= scale;
            c->setPixel({ x,y }, sdlcolorToUint32({(u8)temp[0], (u8)temp[1], (u8)temp[2], (u8)temp[3]}));
        }
    }


    return c;
}

void FilterOffset::setupParamBounds(Layer* target)
{
    lastLayerDims = { target->w, target->h };
}

Layer* FilterOffset::run(Layer* src, ParameterStore* options)
{
    Layer* c = copy(src);

    int offsetX =options->getInt("offset.x");
    int offsetY =options->getInt("offset.y");
    bool wrap = options->getBool("wrap");

    for (int x = 0; x < c->w; x++) {
        for (int y = 0; y < c->h; y++) {

            XY sourcePosition = xySubtract({ x,y }, { offsetX, offsetY });
            if (!pointInBox(sourcePosition, { 0,0,c->w,c->h })) {
                if (wrap) {
                    //sourcePosition will only be negative in this case
                    sourcePosition = { (c->w + sourcePosition.x) % c->w, (c->h + sourcePosition.y) % c->h };
                }
                else {
                    c->setPixel({ x,y }, 0);
                    continue;
                }
            }

            c->setPixel({ x,y }, src->getPixelAt(sourcePosition));
        }
    }
    return c;
}

Layer* FilterRemoveChannels::run(Layer* src, ParameterStore* options)
{
    Layer* c = copy(src);

    bool r = options->getBool("remove.r");
    bool g = options->getBool("remove.g");
    bool b = options->getBool("remove.b");
    bool a = options->getBool("remove.a");

    for (int x = 0; x < c->w; x++) {
        for (int y = 0; y < c->h; y++) {
            u32 px = c->getPixelAt({ x,y }, true);
            SDL_Color col = uint32ToSDLColor(px);

            c->setPixel({ x,y }, PackRGBAtoARGB(r?0:col.r, g?0:col.g, b?0:col.b, a?0:col.a));
        }
    }

    return c;
}

Layer* FilterAlphaThreshold::run(Layer* src, ParameterStore* options)
{
    Layer* c = copy(src);
    int threshold = options->getInt("threshold");// std::stoi(options["threshold"]);

    for (int x = 0; x < c->w; x++) {
        for (int y = 0; y < c->h; y++) {
            SDL_Color col = uint32ToSDLColor(c->getPixelAt({ x,y }, true));
            c->setPixel({ x,y }, PackRGBAtoARGB(col.r, col.g, col.b, col.a >= threshold ? 255 : 0));
        }
    }

    return c;
}

Layer* FilterEdgeDetectOutline::run(Layer* src, ParameterStore* options)
{
    u32 outlineColor = options->getColorRGB("color");
    double threshold = options->getFloat("threshold") / 100.0;

    Layer* cc = FilterKernelTransformation::run(src, options);
    Layer* c = copy(src);

    for (int y = 0; y < c->h; y++) {
        for (int x = 0; x < c->w; x++) {
            u32 srcc = src->getPixelAt({ x,y }, true);
            u32 edgec = cc->getPixelAt({ x,y }, true);
            SDL_Color edgecc = uint32ToSDLColor(srcc);
            hsl hc = rgb2hsl(u32ToRGB(edgec));
            if (hc.l >= threshold) {
                c->setPixel({ x,y }, alphaBlend(srcc, alphaBlend(outlineColor, edgecc.a)));
            }
        }
    }

    delete cc;
    return c;
}

Layer* FilterBlendTowardsColor::run(Layer* src, ParameterStore* options) {

    u32 target = options->getColorRGB("color");
    double factor = options->getFloat("factor") / 100.0;

    Layer* c = copy(src);
    for (int y = 0; y < c->h; y++) {
        for (int x = 0; x < c->w; x++) {
            u32 srcPx = c->getPixelAt({x,y});
            u8 srcAlpha = uint32ToSDLColor(srcPx).a;
            u32 dstPx = modAlpha(target, (u8)(255 * factor));
            c->setPixel({x,y}, modAlpha(alphaBlend(modAlpha(srcPx, 255), dstPx), srcAlpha));
        }
    }
    return c;

}
