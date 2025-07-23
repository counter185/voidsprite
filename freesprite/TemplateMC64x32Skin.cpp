#include "TemplateMC64x32Skin.h"
#include "Layer.h"

uint8_t patternFront[] = {
    1,1,1,
    1,1,1,
    1,1,1
};
uint8_t patternRight[] = {
    0,0,1,
    0,0,1,
    0,0,1
};
uint8_t patternLeft[] = {
    1,0,0,
    1,0,0,
    1,0,0
};
uint8_t patternBack[] = {
    1,1,1,
    1,0,1,
    1,1,1
};
uint8_t patternTop[] = {
    1,1,1,
    0,0,0,
    0,0,0
};
uint8_t patternBottom[] = {
    0,0,0,
    0,0,0,
    1,1,1
};

Layer* TemplateMC64x32Skin::generate()
{
    Layer* nlayer = new Layer(64, 32);
    nlayer->name = TL("vsp.layer.template");
    memset(nlayer->pixels32(), 0x00, 64 * 32 * 4);

    uint32_t headColors[2] = { 0xFF062608, 0xff1b591f };
    uint32_t hatColors[2] = { 0xFF042030, 0xff1b3459 };
    uint32_t bodyColors[2] = { 0xFF675215, 0xff232003 };
    uint32_t legColors[2] = { 0xFF82182f, 0xff380709 };
    uint32_t armColors[2] = { 0xFF3a8d28, 0xff0f3906 };

    //head
    nlayer->fillRect(XY{ 8,0 }, XY{ 16,8 }, headColors[0]);
    drawPattern(nlayer, patternTop, XY{ 3,3 }, XY{ 9, 1 }, headColors[1]);
    nlayer->fillRect(XY{ 16,0 }, XY{ 23,8 }, headColors[1]);
    drawPattern(nlayer, patternBottom, XY{ 3,3 }, XY{ 17, 1 }, headColors[0]);

    nlayer->fillRect(XY{ 0,8 }, XY{ 8,15 }, headColors[0]);
    drawPattern(nlayer, patternLeft, XY{ 3,3 }, XY{ 1,9 }, headColors[1]);
    nlayer->fillRect(XY{ 8,8 }, XY{ 16,15 }, headColors[1]);
    drawPattern(nlayer, patternFront, XY{ 3,3 }, XY{ 9, 9 }, headColors[0]);
    nlayer->fillRect(XY{ 16,8 }, XY{ 24,15 }, headColors[0]);
    drawPattern(nlayer, patternRight, XY{ 3,3 }, XY{ 17, 9 }, headColors[1]);
    nlayer->fillRect(XY{ 24,8 }, XY{ 31,15 }, headColors[1]);
    drawPattern(nlayer, patternBack, XY{ 3,3 }, XY{ 25, 9 }, headColors[0]);
    
    //hat
    nlayer->fillRect(XY{ 40,0 }, XY{ 48,8 }, hatColors[0]);
    nlayer->fillRect(XY{ 48,0 }, XY{ 55,8 }, hatColors[1]);

    nlayer->fillRect(XY{ 32,8 }, XY{ 40,15 }, hatColors[0]);
    nlayer->fillRect(XY{ 40,8 }, XY{ 48,15 }, hatColors[1]);
    nlayer->fillRect(XY{ 48,8 }, XY{ 56,15 }, hatColors[0]);
    nlayer->fillRect(XY{ 56,8 }, XY{ 63,15 }, hatColors[1]);

    //leg
    nlayer->fillRect(XY{ 4,16 }, XY{ 7,19 }, legColors[0]);
    nlayer->fillRect(XY{ 8,16 }, XY{ 11,19 }, legColors[1]);

    nlayer->fillRect(XY{ 0,20 }, XY{ 3, 31}, legColors[0]);
    nlayer->fillRect(XY{ 4,20 }, XY{ 7, 31}, legColors[1]);
    nlayer->fillRect(XY{ 8,20 }, XY{ 11, 31}, legColors[0]);
    nlayer->fillRect(XY{ 12,20 }, XY{ 15, 31}, legColors[1]);

    //body
    nlayer->fillRect(XY{ 16,20 }, XY{ 19,31 }, bodyColors[0]);
    nlayer->fillRect(XY{ 20,20 }, XY{ 27,31 }, bodyColors[1]);
    nlayer->fillRect(XY{ 28,20 }, XY{ 31,31 }, bodyColors[0]);
    nlayer->fillRect(XY{ 32,20 }, XY{ 39,31 }, bodyColors[1]);

    nlayer->fillRect(XY{ 20,16 }, XY{ 27,19 }, bodyColors[0]);
    nlayer->fillRect(XY{ 28,16 }, XY{ 35,19 }, bodyColors[1]);

    //arm
    nlayer->fillRect(XY{ 44,16 }, XY{ 47,19 }, armColors[0]);
    nlayer->fillRect(XY{ 48,16 }, XY{ 51,19 }, armColors[1]);

    nlayer->fillRect(XY{ 40,20 }, XY{ 43, 31 }, armColors[0]);
    nlayer->fillRect(XY{ 44,20 }, XY{ 47, 31 }, armColors[1]);
    nlayer->fillRect(XY{ 48,20 }, XY{ 51, 31 }, armColors[0]);
    nlayer->fillRect(XY{ 52,20 }, XY{ 55, 31 }, armColors[1]);


    return nlayer;
}
