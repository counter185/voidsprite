#include "BaseFilter.h"
#include "Layer.h"

Layer* BaseFilter::copy(Layer* src)
{
    if (src == NULL) {
        return NULL;
    }
    Layer* ret = new Layer(src->w, src->h);
    ret->name = src->name;
    ret->colorKey = src->colorKey;
    memcpy(ret->pixelData, src->pixelData, src->w * src->h * 4);
    return ret;
}

Layer* FilterBlur::run(Layer* src, std::map<std::string, std::string> options)
{
    Layer* c = copy(src);
    int radiusX = options.contains("radius.x") ? std::stod(options["radius.x"]) : 4;
    int radiusY = options.contains("radius.y") ? std::stod(options["radius.y"]) : 4;
    for (int y = 0; y < c->h; y++) {
        for (int x = 0; x < c->w; x++) {

            u32 pxnow = src->getPixelAt({ x,y }, true);
            u64 r = (pxnow >> 16) & 0xFF;
            u64 g = (pxnow >> 8) & 0xFF;
            u64 b = pxnow & 0xFF;
            u64 a = (pxnow >> 24) & 0xFF;
            int measures = 1;

            for (int rx = 1; rx < radiusX; rx++) {
                if (x + rx < c->w) {
					u32 px = src->getPixelAt({ x + rx,y }, true);
					r += (px >> 16) & 0xFF;
					g += (px >> 8) & 0xFF;
					b += px & 0xFF;
                    a += (px >> 24) & 0xFF;
					measures++;
				}
				if (x - rx >= 0) {
					u32 px = src->getPixelAt({ x - rx,y }, true);
                    r += (px >> 16) & 0xFF;
                    g += (px >> 8) & 0xFF;
                    b += px & 0xFF;
                    a += (px >> 24) & 0xFF;
					measures++;
				}
            }

            for (int ry = 1; ry < radiusY; ry++) {
                if (y + ry < c->h) {
                    u32 px = src->getPixelAt({ x,y + ry }, true);
                    r += (px >> 16) & 0xFF;
                    g += (px >> 8) & 0xFF;
                    b += px & 0xFF;
                    a += (px >> 24) & 0xFF;
                    measures++;
                }
                if (y - ry >= 0) {
					u32 px = src->getPixelAt({ x,y - ry }, true);
                    r += (px >> 16) & 0xFF;
                    g += (px >> 8) & 0xFF;
                    b += px & 0xFF;
                    a += (px >> 24) & 0xFF;
					measures++;
				}
            }

            r /= measures;
            g /= measures;
            b /= measures;
            a /= measures;
            c->setPixel({ x,y }, PackRGBAtoARGB(r, g, b, a));
        }
    }
    return c;
}
