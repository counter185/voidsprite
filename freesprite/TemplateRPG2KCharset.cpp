#include "Layer.h"
#include "TemplateRPG2KCharset.h"

Layer* TemplateRPG2KCharset::generate()
{
    Layer* ret = new Layer(288, 256);
    ret->name = "Template Layer";
    uint32_t tileBGColors[] = {
        0xff1e0000, 0xff380000,
        0xff382300, 0xff150d00,
        0xff0d1500, 0xff233900,
        0xff00431e, 0xff001f0e,
        0xff002122, 0xff004243,
        0xff001443, 0xff000a22,
        0xff100022, 0xff200043,
        0xff480036, 0xff220019
    };
    int index = 0;
    int iindex = 0;
    for (int y = 0; y < 2; y++) {
        for (int x = 0; x < 4; x++) {
            XY base = XY{ 72 * x, y * 128 };

            for (int yy = 0; yy < 4; yy++) {
                for (int xx = 0; xx < 3; xx++) {
                    XY from = xyAdd(base, XY{ xx * 24, yy * 32 });
                    XY to = xyAdd(from, XY{ 24,32 });
                    ret->fillRect(from, to, tileBGColors[index*2 + (iindex++ % 2)]);
                }
            }
            index++;
        }
    }
    return ret;
}
