#include "TemplateRPG2KBattleAnim.h"

u8 battletemplate_patternO[5][5] = {
    {0,0,0,0,0},
    {0,1,1,1,1},
    {0,1,0,0,1},
    {0,1,0,0,1},
    {0,1,1,1,1}
};
u8 battletemplate_patternX[5][5] = {
    {0,0,0,0,0},
    {0,0,0,0,0},
    {0,0,1,1,0},
    {0,0,1,1,0},
    {0,0,0,0,0}
};

Layer* TemplateRPG2KBattleAnim::generate()
{
    Layer* ret = new Layer(480, 480);
    ret->name = "Template Layer";

    u32 tileBGs[] = {
        0xFF1D0000, 0xFF3B0000,
        0xFF1D1D00, 0xFF3B3B00,
        0xFF001D00, 0xFF003B00,
        0xFF00141D, 0xFF002332,
        0xFF12001D, 0xFF1F0032
    };
    for (int y = 0; y < 5; y++) {
        XY origin = { 0, 96 * y };
        drawCheckerboard(ret, origin, { 96,96 }, {5, 1}, tileBGs[y * 2], tileBGs[y * 2 + 1], y%2 == 1);
        for (int x = 0; x < 5; x++) {
            XY origin2 = { origin.x + 96 * x, origin.y };
            for (int xx = 0; xx < 5; xx++) {
                drawPattern(ret, xx == x ? (u8*)battletemplate_patternO : (u8*)battletemplate_patternX, { 5,5 }, {origin2.x + xx * 5, origin2.y}, (y+x)%2 == 1 ? tileBGs[y * 2] : tileBGs[y * 2 + 1]);
            }
        }
    }
    return ret;
}

std::vector<CommentData> TemplateRPG2KBattleAnim::placeComments()
{
    return {};
}
