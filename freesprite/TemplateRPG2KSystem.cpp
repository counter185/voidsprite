#include "Layer.h"
#include "TemplateRPG2KSystem.h"

Layer* TemplateRPG2KSystem::generate()
{
    Layer* ret = new Layer(160, 80);
    ret->name = "Template Layer";
    uint32_t tileBGColors[] = {
        0xff1e0000, 0xff380000,
        0xff0d0d15, 0xff382338,
        0xff233923, 0xff0d150d,
        0xff001f0e, 0xff00431e,
        0xff004243, 0xff002122,
        0xff000a22, 0xff001443,
        0xff100022, 0xff200043,
        0xff220019, 0xff480036
    };
     uint32_t tileBGColors8x8[] = {
        0xff002338, 0xff000d15,
        0xff000d15, 0xff002338,
        0xff002338, 0xff000d15,
        0xff000d15, 0xff002338,
        0xff004243, 0xff002122,
    };
     uint32_t tileBGColorsText[] = {
        0xff99d0de, 0xff757500,
        0xff0fff00, 0xff757575,
        0xffff0000, 0xff380000,
        0xff1e0000, 0xff380000,
        0xff0d0d15, 0xff382338,
        0xff233923, 0xff0d150d,
        0xff00431e, 0xff001f0e,
        0xff004243, 0xff002122,
        0xff001443, 0xff000a22,
        0xff100022, 0xff200043,
        0xff480036, 0xff220019
    };
    int index = 0;
    int iindex = 0;
    int x = 0;
    int y = 0;
    XY base = XY{ 32 * x, y * 32 };
    XY base2 = XY{ 128 + (8 * x), y * 8 };
    XY base3 = XY{8 * x, 32 + (y * 8) };
    XY base4 = XY{16 * x, 48 + (y * 16) };


    for (int xx = 0; xx < 4; xx++) {
        XY from = xyAdd(base, XY{ xx * 32, 0 });
        XY to = xyAdd(from, XY{ 32,32 });
        ret->fillRect(from, to, tileBGColors[index * 2 + (iindex++ % 2)]);
        if (xx == 1) {
                index++;
        }
    }

    for (int y2 = 0; y2 < 4; y2++) {
        for (int x2 = 0; x2 < 4; x2++) {
            XY from = xyAdd(base2, XY{ x2 * 8, y2 * 8 });
            XY to = xyAdd(from, XY{ 8,8 });
            ret->fillRect(from, to, tileBGColors8x8[index * 2 + (iindex++ % 2)]);

        }
        index++;
    }

    for (int x3 = 0; x3 < 10; x3++) {
        XY from = xyAdd(base3, XY{ x3 * 16, 0 });
        XY to = xyAdd(from, XY{ 16,16 });
        ret->fillRect(from, to, tileBGColors[index * 2 + (iindex++ % 2)]);
        if (x3 == 1 || x3==7) {
                index++;
        }
    }

    index = 0;
    iindex = 0;
    int iter = 0;

     for (int y4 = 0; y4 < 2; y4++) {
        for (int x4 = 0; x4 < 10; x4++) {
            XY from = xyAdd(base4, XY{ x4 * 16, y4 * 16 });
            XY to = xyAdd(from, XY{ 16,16 });
            ret->fillRect(from, to, tileBGColorsText[index * 2 + (iindex++ % 2)]);     
            iter++;
            if (iter > 1) {
                index++;
                iter = 0;
            }
        }
    }

     index = 0;
     iindex = 0;

     ret->fillRect(XY{ 44,7 }, XY{ 51,14 }, tileBGColors8x8[index * 2 + (iindex++ % 2)]);

     ret->fillRect(XY{ 44,17 }, XY{ 51,24 }, tileBGColors8x8[index * 2 + (iindex++ % 2)]);


    return ret;
}
    
std::vector<CommentData> TemplateRPG2KSystem::placeComments()
{
    return { 
        { {0,0}, "Menu Background" },
        { {32,0}, "Window Frame" },
        { {44,7}, "UP Arrow" },
        { {44,17}, "DOWN Arrow" },
        { {64,0}, "Selection Frames (Animated)" },
        { {128,0}, "Shop Arrow (Stat Up)" },
        { {128,8}, "Shop Arrow (Can't Equip)" },
        { {128,16}, "Shop Arrow (Stat Down)" },
        { {128,24}, "Shop Arrow (Equipped)" },
        { {0,32}, "Fill Colour" },
        { {17,32}, "Text Shadow" },
        { {32,32}, "Timer Digits" },
        { {128,32}, "Airship Shadow" },
        { {0,48}, "Text Colour" },
        { {16,48}, "Stat Colour" },
        { {32,48}, "Stat Up Colour" },
        { {48,48}, "Stat Down Colour" },
        { {64,48}, "Health Down Colour" }
    };
}