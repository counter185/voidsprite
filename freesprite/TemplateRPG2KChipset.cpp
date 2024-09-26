#include "Layer.h"
#include "TemplateRPG2KChipset.h"

//this naming is NOT required to not piss off g++ "no definitions of"


Layer* TemplateRPG2KChipset::generate()
{
    Layer* ret = new Layer(480, 256);
    ret->name = "Template Layer";
    uint32_t tileBGColors_terrain[] = {
              0xff001e1e, 0xff003838,
        0xff000d15, 0xff002338,
        0xff00001d, 0xff000039,
        0xff1d001d, 0xff390039,
        0xff061d00, 0xff065d00,
        0xff065d00, 0xff061d00,
        0xff061d00, 0xff065d00,
        0xff065d00, 0xff061d00,
        0xff061d00, 0xff065d00,
        0xff065d00, 0xff061d00,
        0xff061d00, 0xff065d00,
        0xff065d00, 0xff061d00,
        0xff061d00, 0xff065d00,
        0xff065d00, 0xff061d00,
        0xff061d00, 0xff065d00,
        0xff065d00, 0xff061d00,
        0xff061d00, 0xff065d00

    };
    int index = 0;
    int iindex = 0;
    int x = 0;
    int y = 0;
    int iter = 0;
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 2; x++) {
            XY base = XY{ 48 * x,y * 64 };

            //this draws the animated tiles + autotiles pt 1

            for (int yy = 0; yy < 4; yy++) {
                for (int xx = 0; xx < 3; xx++) {
                    XY from = xyAdd(base, XY{ xx * 16, yy * 16 });
                    XY to = xyAdd(from, XY{ 16,16 });
                    ret->fillRect(from, to, tileBGColors_terrain[index * 2 + (iindex++ % 2)]);

                }

            }
            index++;

        }
    }

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 2; x++) {
            XY base = XY{ 96+(48 * x),y * 64 };

            //this draws the animated tiles + autotiles pt 2

            for (int yy = 0; yy < 4; yy++) {
                for (int xx = 0; xx < 3; xx++) {
                    XY from = xyAdd(base, XY{ xx * 16, yy * 16 });
                    XY to = xyAdd(from, XY{ 16,16 });
                    ret->fillRect(from, to, tileBGColors_terrain[index * 2 + (iindex++ % 2)]);

                }

            }
            index++;

        }
    }

    return ret;

    
}
    
std::vector<CommentData> TemplateRPG2KChipset::placeComments()
{
    return { 
        { {0,0}, "Water (Shallow)" },

    };
}

