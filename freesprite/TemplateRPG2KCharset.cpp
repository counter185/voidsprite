#include "Layer.h"
#include "TemplateRPG2KCharset.h"

//this naming is required to not piss off g++ "multiple definitions of"
u8 rpg2kcharset_patternUp[5][5] = {
    {0,0,1,0,0},
    {0,1,1,1,0},
    {1,0,1,0,1},
    {0,0,1,0,0},
    {0,0,1,0,0},
};
u8 rpg2kcharset_patternRight[5][5] = {
    {0,0,1,0,0},
    {0,0,0,1,0},
    {1,1,1,1,1},
    {0,0,0,1,0},
    {0,0,1,0,0},
};
u8 rpg2kcharset_patternDown[5][5] = {
    {0,0,1,0,0},
    {0,0,1,0,0},
    {1,0,1,0,1},
    {0,1,1,1,0},
    {0,0,1,0,0},
};
u8 rpg2kcharset_patternLeft[5][5] = {
    {0,0,1,0,0},
    {0,1,0,0,0},    
    {1,1,1,1,1},
    {0,1,0,0,0},
    {0,0,1,0,0},
};


Layer* TemplateRPG2KCharset::generate()
{
    Layer* ret = new Layer(288, 256);
    ret->name = "Template Layer";
    u32 tileBGColors[] = {
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
    (void) index, (void) iindex;
    for (int y = 0; y < 2; y++) {
        for (int x = 0; x < 4; x++) {
            XY base = XY{ 72 * x, y * 128 };

            int tileBGColorIndex = (x + y * 4) * 2;
            drawCheckerboard(ret, base, { 24, 32 }, { 3,4 }, tileBGColors[tileBGColorIndex], tileBGColors[tileBGColorIndex + 1]);

            for (int yy = 0; yy < 4; yy++) {
                XY arrowSymbolOrigin = xyAdd(base, XY{ 1, yy * 32 + 1 });
                u8* patternSymbols[] = {(u8*)rpg2kcharset_patternUp, (u8*)rpg2kcharset_patternRight, (u8*)rpg2kcharset_patternDown, (u8*)rpg2kcharset_patternLeft};
                u8* patternSymbol = patternSymbols[yy];
                drawPattern(ret, patternSymbol, { 5,5 }, arrowSymbolOrigin, tileBGColors[tileBGColorIndex + (yy+1)%2]);
            }
            index++;
        }
    }
    return ret;
}

std::vector<CommentData> TemplateRPG2KCharset::placeComments()
{
    return { 
        { {0,0}, "Animation frame 2" },
        { {24,0}, "Idle" },
        { {48,0}, "Animation frame 1" },
    };
}
