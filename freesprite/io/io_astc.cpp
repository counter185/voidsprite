#include "io_base.h"
#include "io_astc.h"

#include "../astc_dec/astc_decomp.h"


bool DeASTCWithoutDeswizzle(Layer* ret, FILE* infile, int blockW, int blockH, OperationProgressReport* progress) {
    u8 buffer[16];
    u8* rgbaBuffer = (u8*)tracked_malloc(4 * blockH * blockW);
    DoOnReturn freeRgbaBuffer([rgbaBuffer]() { tracked_free(rgbaBuffer); });

    int blocksW = ceil((float)ret->w / blockW);
    int blocksH = ceil((float)ret->h / blockH);

    u64 numBlocks = (u64)blocksW * blocksH;
    u64 numBlocksProcessed = 0;

    progress->enterSection("Decompressing ASTC...");

    for (int blockY = 0; blockY < blocksH; blockY++) {
        for (int blockX = 0; blockX < blocksW; blockX++) {
            progress->enterSection(frmt("Block {}/{}", numBlocksProcessed++, numBlocks));
            fread(buffer, 1, 16, infile);
            
            bool success = basisu::astc::decompress(rgbaBuffer, buffer, false, blockW, blockH);
            if (!success) {
                logerr("[ASTC] decompression failed");
                return false;
            }
            int rgbaDataPointer = 0;
            for (int y = 0; y < blockH; y++) {
                for (int x = 0; x < blockW; x++) {
                    u8 r = rgbaBuffer[rgbaDataPointer++];
                    u8 g = rgbaBuffer[rgbaDataPointer++];
                    u8 b = rgbaBuffer[rgbaDataPointer++];
                    u8 a = rgbaBuffer[rgbaDataPointer++];
                    int pixelX = blockX * blockW + x;
                    int pixelY = blockY * blockH + y;
                    ret->setPixel({ pixelX, pixelY }, PackRGBAtoARGB(r, g, b, a));
                }
            }
            progress->exitSection();
        }
    }
    progress->exitSection();
    return true;
}

//function below is old silly code to get rid of
int DeASTC(Layer* ret, int width, int height, uint64_t fileLength, FILE* infile, int blockWidth, int blockHeight) {
    uint32_t* pxd = ret->pixels32();
    /*int skip = 232;
    int skipHowMany = 24;
    int skipCounter = 0;*/

    int astcErrors = 0;
    int blocksW = ceil((float)width / blockWidth);
    int blocksH = ceil((float)height / blockHeight);

    int x = 0;
    int y = 0;

    uint8_t astcData[16];

    while (y < blocksH && x < blocksW) {

        //for (int yyyyy = 0; yyyyy < 2; yyyyy++) {
        for (int xxxxx = 0; xxxxx < 2; xxxxx++) {

            //1x4 vertical strip
            for (int yyyy = 0; yyyy < 4; yyyy++) {

                //2x2 deswizzle
                for (int xx = 0; xx < 2; xx++) {
                    for (int yy = 0; yy < 2; yy++) {

                        fread(astcData, 1, 16, infile);
                        /*while (((uint64_t*)astcData)[0] == 0 && ((uint64_t*)astcData)[1] == 0 && ftell(infile) < fileLength) {
                            fread(astcData, 1, 16, infile);
                        }*/
                        uint8_t* rgbaData = (uint8_t*)tracked_malloc(4 * blockHeight * blockWidth);
                        bool success = basisu::astc::decompress(rgbaData, astcData, false, blockWidth, blockHeight);

                        if (!success) {
                            logprintf("ASTC decompression failed\n");
                            //astcErrors++;
                            //return;
                        }

                        int rgbaDataPointer = 0;

                        for (int yyy = 0; yyy < blockHeight; yyy++) {
                            for (int xxx = 0; xxx < blockWidth; xxx++) {
                                //uint32_t colorNow = ((uint32_t*)rgbaData)[yy * blockWidth + xx];

                                uint8_t r = rgbaData[rgbaDataPointer++];
                                uint8_t g = rgbaData[rgbaDataPointer++];
                                uint8_t b = rgbaData[rgbaDataPointer++];
                                uint8_t a = rgbaData[rgbaDataPointer++];

                                //if (y+yy > )
                                ret->setPixel({ (x + xxxxx * 2 + xx) * blockWidth + xxx, (y + yyyy * 2 + yy) * blockHeight + yyy }, PackRGBAtoARGB(r, g, b, a));
                                //ret->setPixel({ x + xx, y + yy }, PackRGBAtoARGB(rgbaData[rgbaDataPointer++], rgbaData[rgbaDataPointer++], rgbaData[rgbaDataPointer++], 255));
                                //ret->setPixel({ x + xx,y + yy }, colorNow);
                            }
                        }
                        tracked_free(rgbaData);
                    }
                }
            }
        }
        //}

        y += 8;
        if (y >= blocksH - 8) {
            y = 0;
            x += 4;
        }

    }

    logprintf("[ASTC] at %lx / %lx\n", ftell(infile), fileLength);
    return astcErrors;
}


Layer* readASTC(PlatformNativePathString path, u64 seek, OperationProgressReport* report)
{
    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {

        astc_header header{};
        fread(&header, sizeof(astc_header), 1, f);

        Layer* ret = Layer::tryAllocLayer(header.dim_x, header.dim_y);
        if (ret != NULL) {
            ret->name = "ASTC Layer";
            if (!DeASTCWithoutDeswizzle(ret, f, header.block_x, header.block_y, report)) {
                delete ret;
                ret = NULL;
            }
        }
        fclose(f);
        return ret;
    }
    return NULL;
}