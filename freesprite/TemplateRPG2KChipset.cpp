#include "Layer.h"
#include "TemplateRPG2KChipset.h"

//this naming is NOT required to not piss off g++ "no definitions of"


Layer* TemplateRPG2KChipset::generate()
{
    Layer* ret = new Layer(480, 256);
    ret->name = TL("vsp.layer.template");
    uint32_t tileBGColors_terrain[] = {
              0xff001e5e, 0xff003868,
        0xff000d15, 0xff002338,
        0xff00001d, 0xff000039,
        0xff1d001d, 0xff390039,
        0xff061d00, 0xff063d00,
        0xff003b31, 0xff002620,
        0xff002620, 0xff003b31,
        0xff063d00, 0xff061d00,
        0xff061d00, 0xff063d00,
         0xff003b31, 0xff002620,
        0xff002620, 0xff003b31,
        0xff063d00, 0xff061d00,
        0xff061d00, 0xff063d00,
         0xff003b31, 0xff002620,
        0xff002620, 0xff003b31,
        0xff063d00, 0xff061d00,
        0xff061d00, 0xff063d00,
         0xff003b31, 0xff002620,
        0xff002620, 0xff003b31,
        0xff063d00, 0xff061d00,
        0xff061d00, 0xff063d00,
         0xff003b31, 0xff002620,
        0xff002620, 0xff003b31

    };
    uint32_t tileBGColors_tiles[] = {
        0xff3d1300,0xff240b00,
        0xff240b00,0xff3d1300,
        0xff3d1300,0xff240b00,
        0xff240b00,0xff3d1300,
        0xff3d1300,0xff240b00,
        0xff240b00,0xff3d1300,
        0xff3d1300,0xff240b00,
        0xff240b00,0xff3d1300,
        0xff3d1300,0xff240b00,
        0xff240b00,0xff3d1300,
        0xff3d1300,0xff240b00,
        0xff240b00,0xff3d1300,
        0xff2a003b,0xff1a0024,
        0xff1a0024,0xff2a003b,
        0xff2a003b,0xff1a0024,
        0xff1a0024,0xff2a003b,
        0xff2a003b,0xff1a0024,
        0xff1a0024,0xff2a003b,
        0xff2a003b,0xff1a0024,
        0xff1a0024,0xff2a003b,
        0xff2a003b,0xff1a0024,
        0xff1a0024,0xff2a003b,
        0xff2a003b,0xff1a0024,
        0xff1a0024,0xff2a003b,
    };
    int index = 0;
    int iindex = 0;
    int x = 0;
    int y = 0;
    int m = 0;
    int iter = 0;
    int bgc_iter = 0;
    for (int mover = 0; mover < 5; mover++) {
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 2; x++) {
                XY base = XY{ m + (48 * x),y * 64 };
                
                //this draws the aLL OF EVERYTHING except that one small tile

                for (int yy = 0; yy < 4; yy++) {
                    for (int xx = 0; xx < 3; xx++) {
                        XY from = xyAdd(base, XY{ xx * 16, yy * 16 });
                        XY to = xyAdd(from, XY{ 16,16 });
                        switch (bgc_iter) {
                        case 0:
                        case 1:
                            ret->fillRect(from, to, tileBGColors_terrain[index * 2 + (iindex++ % 2)]);
                            break;
                        case 2:
                        case 3:
                        case 4:
                            ret->fillRect(from, to, tileBGColors_tiles[index * 2 + (iindex++ % 2)]);
                            break;
                        }
                        
                    }

                }
                index++;

            }
            iter++;
        }
        if (iter == 4) {
            m += 96;
            iter = 0;
              switch (bgc_iter) {
            case 0:
                index = 4;
                break;
            case 1:
                index = 0;
                break;
            }
            bgc_iter++;
            
        }
    }

    ret->fillRect({368,112},{383,127}, 0xff705c00);
    ret->fillRect({288,128},{303,143}, 0xff705c00);
    
    return ret;

    
}
    
std::vector<CommentData> TemplateRPG2KChipset::placeComments()
{
    return { 
        { {0,0}, "Water (Shallow)" },
        { {48,0}, "Alternate Water (Shallow)" },
        { {0,128}, "Autotiles" },
        { {0,64}, "Water (Deep)" },
        { {48,64}, "Animated Tiles" },
        { {192,0}, "Lower Layer Tiles" },
        { {288,128}, "Upper Layer Tiles" },
        { {368,112}, "Air Tile" },


    };
}

