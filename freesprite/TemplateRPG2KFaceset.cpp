#include "Layer.h"
#include "TemplateRPG2KFaceset.h"

//this naming is required to not piss off g++ "multiple definitions of"
uint8_t rpg2kfaceset_patternUp[5][5] = {
    {0,0,1,0,0},
    {0,1,1,0,0},
    {1,0,1,0,0},
    {0,0,1,0,0},
    {0,0,1,0,0},
};
uint8_t rpg2kfaceset_patternRight[5][5] = {
    {0,1,1,1,0},
    {1,0,0,0,1},
    {0,0,0,1,0},
    {0,0,1,0,0},
    {1,1,1,1,1},
};
uint8_t rpg2kfaceset_patternDown[5][5] = {
    {0,1,1,1,0},
    {1,0,0,0,1},
    {0,0,1,1,0},
    {1,0,0,0,1},
    {0,1,1,1,0},
};
uint8_t rpg2kfaceset_patternLeft[5][5] = {
    {0,1,0,1,0},
    {0,1,0,1,0},
    {0,1,1,1,0},
    {0,0,0,1,0},
    {0,0,0,1,0},
};


Layer* TemplateRPG2KFaceset::generate()
{
    Layer* ret = new Layer(192, 192);
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
    int x = 0;
    int y = 0;
    XY base = XY{ 48 * x, y * 48 };

    for (int yy = 0; yy < 4; yy++) {
        for (int xx = 0; xx < 4; xx++) {
            XY from = xyAdd(base, XY{ xx * 48, yy * 48 });
            XY to = xyAdd(from, XY{ 48,48 });
            ret->fillRect(from, to, tileBGColors[index * 2 + (iindex++ % 2)]);

        }
        XY arrowSymbolOrigin = xyAdd(base, XY{ 1, yy * 48 + 1 });
        uint8_t* patternSymbols[] = { (uint8_t*)rpg2kfaceset_patternUp, (uint8_t*)rpg2kfaceset_patternRight, (uint8_t*)rpg2kfaceset_patternDown, (uint8_t*)rpg2kfaceset_patternLeft };
        uint8_t* patternSymbol = patternSymbols[yy];
        for (int yyy = 0; yyy < 5; yyy++) {
            for (int xxx = 0; xxx < 5; xxx++) {
                if (patternSymbol[yyy * 5 + xxx] == 1) {
                    ret->setPixel(xyAdd(arrowSymbolOrigin, XY{ xxx,yyy }), tileBGColors[(index * 2 + (iindex % 2)) + 1]);
                }
            }
        }
        index++;
    }
    return ret;
}
    

