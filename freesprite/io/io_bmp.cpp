#include "io_base.h"
#include "io_bmp.h"

void bmpEncode4BitEncodedMode(u8 v1v2, int count, std::vector<u8>& ret)
{
    while (count > 0) {
        int next = ixmin(255, count);
        ret.push_back((u8)next);
        ret.push_back(v1v2);
        count -= next;
    }
}

void bmpEncode4BitAbsoluteMode(std::vector<u8>& values4bit, std::vector<u8>& ret)
{
    if (values4bit.size() > 2) {
        BitWriter bits = BitWriter();
        for (u8& r : values4bit) {
            bits.writeBits(r, 4);
        }
        bits.flush();

        std::vector<u8>& values = bits.getData();
        for (int i = 0; i < values.size(); i++) {
            values[i] = (values[i] >> 4) | (values[i] << 4);
        }

        ret.push_back(0);
        ret.push_back((u8)values4bit.size());
        for (u8& r : values) {
            ret.push_back(r);
        }
        if ((values.size() % 2) == 1) {
            ret.push_back(0);
        }
    }
    else {
        for (u8& r : values4bit) {
            bmpEncode4BitEncodedMode(r<<4, 1, ret);
        }
    }
}

std::vector<u8> bmpEncode4BitRLELine(u32* values, int n)
{
    struct bmp4bRLERun {
        int count = 0;
        u8 vals[2] = { 0,0 };
    };

    std::vector<bmp4bRLERun> runs{};

    bmp4bRLERun lastRun{};

    for (int i = 0; i < n; i++) {
        u8 nextVal = (u8)values[i] & 0b1111;
        if (lastRun.count == 0) {
            lastRun.count++;
            lastRun.vals[0] = nextVal;
        }
        else if (lastRun.count == 1) {
            lastRun.count++;
            lastRun.vals[1] = nextVal;
        }
        else if (lastRun.vals[lastRun.count % 2] == nextVal) {
            lastRun.count++;
        }
        else {
            if (lastRun.count <= 2) {
                //explode the grouped runs into 2 separate single ones
                runs.push_back({ 1, {lastRun.vals[0],0} });
                lastRun = { 2, {lastRun.vals[1],nextVal} };
            }
            else {
                runs.push_back(lastRun);
                lastRun = { 1,{nextVal,0} };
            }
        }
    }
    if (lastRun.count > 0) {
        runs.push_back(lastRun);
    }

    std::vector<u8> nextAbsoluteRun{};
    std::vector<u8> ret{};
    for (bmp4bRLERun& r : runs) {
        if (r.count > 1) {
            bmpEncode4BitAbsoluteMode(nextAbsoluteRun, ret);
            nextAbsoluteRun = {};
            bmpEncode4BitEncodedMode((r.vals[0] << 4) | (r.vals[1]), r.count, ret);
        }
        else if (r.count == 1) {
            nextAbsoluteRun.push_back(r.vals[0]);
            if (nextAbsoluteRun.size() == 255) {
                bmpEncode4BitAbsoluteMode(nextAbsoluteRun, ret);
                nextAbsoluteRun = {};
            }
        }
        else {
            //??
        }
    }

    bmpEncode4BitAbsoluteMode(nextAbsoluteRun, ret);

    return ret;
}

void bmpEncode4BitRLE(LayerPalettized* data, FILE* f)
{
    u32* ppx = data->pixels32();
    for (int y = data->h - 1; y >= 0; y--) {
        u32* line = &ARRAY2DPOINT(ppx, 0, y, data->w);
        auto bytes = bmpEncode4BitRLELine(line, data->w);
        bytes.push_back(0);
        bytes.push_back(y == 0 ? 1 : 0);
        fwrite(bytes.data(), bytes.size(), 1, f);
    }
}

void bmpEncode8BitEncodedMode(u8 value, int count, std::vector<u8>& ret)
{
    while (count > 0) {
        int next = ixmin(255, count);
        ret.push_back((u8)next);
        ret.push_back(value);
        count -= next;
    }
}

void bmpEncode8BitAbsoluteMode(std::vector<u8>& values, std::vector<u8>& ret)
{
    if (values.size() > 2) {
        ret.push_back(0);
        ret.push_back((u8)values.size());
        for (u8& r : values) {
            ret.push_back(r);
        }
        if ((values.size() % 2) == 1) {
            ret.push_back(0);
        }
    }
    else {
        for (u8& r : values) {
            bmpEncode8BitEncodedMode(r, 1, ret);
        }
    }
}

std::vector<u8> bmpEncode8BitRLELine(u32* values, int n)
{
    struct bmpRLERun {
        int count = 0;
        u8 val = 0;
    };

    std::vector<bmpRLERun> runs{};

    bmpRLERun lastRun{};

    for (int i = 0; i < n; i++) {
        u8 nextVal = (u8)values[i];
        if (lastRun.val == nextVal) {
            lastRun.count++;
        }
        else {
            if (lastRun.count > 0) {
                runs.push_back(lastRun);
            }
            lastRun = {1, nextVal};
        }
    }
    if (lastRun.count > 0) {
        runs.push_back(lastRun);
    }

    std::vector<u8> nextAbsoluteRun{};
    std::vector<u8> ret{};
    for (bmpRLERun& r : runs) {
        if (r.count > 1) {
            bmpEncode8BitAbsoluteMode(nextAbsoluteRun, ret);
            nextAbsoluteRun = {};
            bmpEncode8BitEncodedMode(r.val, r.count, ret);
        }
        else if (r.count == 1) {
            nextAbsoluteRun.push_back(r.val);
            if (nextAbsoluteRun.size() == 255) {
                bmpEncode8BitAbsoluteMode(nextAbsoluteRun, ret);
                nextAbsoluteRun = {};
            }
        }
        else {
            //??
        }
    }

    bmpEncode8BitAbsoluteMode(nextAbsoluteRun, ret);

    return ret;
}

void bmpEncode8BitRLE(LayerPalettized* data, FILE* f)
{
    u32* ppx = data->pixels32();
    for (int y = data->h - 1; y >= 0; y--) {
        u32* line = &ARRAY2DPOINT(ppx, 0, y, data->w);
        auto bytes = bmpEncode8BitRLELine(line, data->w);
        bytes.push_back(0);
        bytes.push_back(y == 0 ? 1 : 0);
        fwrite(bytes.data(), bytes.size(), 1, f);
    }
}

void bmpDecode8BitRLE(Layer* ret, FILE* f)
{
    u32* ppx = ret->pixels32();
    memset(ppx, 0xff, ret->w * ret->h * 4);
    for (int y = 0; y < ret->h; y++) {
        int flippedY = ret->h - 1 - y;
        for (int x = 0; !feof(f); x++) {
            u8 command[2] = { 0,0 };
            fread(command, 2, 1, f);

            if (command[0] == 0) {
                if (command[1] == 0) {
                    //end of line
                    break;
                }
                else if (command[1] == 1) {
                    //end of bitmap
                    y = ret->h;
                    break;
                }
                else if (command[1] == 2) {
                    //delta
                    u8 rightUp[2] = { 0,0 };
                    fread(rightUp, 2, 1, f);
                    x += rightUp[0];
                    y += rightUp[1];
                    x--;
                    flippedY = ret->h - 1 - y;
                }
                else {
                    //absolute mode
                    for (int i = 0; i < command[1]; i++) {
                        u8 next = 0;
                        fread(&next, 1, 1, f);
                        if (x < ret->w) {
                            ARRAY2DPOINT(ppx, x, flippedY, ret->w) = next;
                        }
                        x++;
                    }
                    x--;
                    if (command[1] % 2 == 1) {
                        fseek(f, 1, SEEK_CUR);
                    }
                }
            }
            else {
                //encoded mode
                for (int i = 0; i < command[0]; i++) {
                    if (x < ret->w) {
                        ARRAY2DPOINT(ppx, x, flippedY, ret->w) = command[1];
                    }
                    x++;
                }
                x--;
            }
        }
    }
}

void bmpDecode4BitRLE(Layer* ret, FILE* f)
{
    u32* ppx = ret->pixels32();
    memset(ppx, 0xff, ret->w * ret->h * 4);
    for (int y = 0; y < ret->h; y++) {
        int flippedY = ret->h - 1 - y;
        for (int x = 0; !feof(f); ) {
            u8 command[2] = { 0,0 };
            fread(command, 2, 1, f);

            if (command[0] == 0) {
                if (command[1] == 0) {
                    //end of line
                    break;
                }
                else if (command[1] == 1) {
                    //end of bitmap
                    y = ret->h;
                    break;
                }
                else if (command[1] == 2) {
                    //delta
                    u8 rightUp[2] = { 0,0 };
                    fread(rightUp, 2, 1, f);
                    x += rightUp[0];
                    y += rightUp[1];
                    flippedY = ret->h - 1 - y;
                }
                else {
                    //absolute mode
                    BitReader rd = BitReader(f);
                    rd.readHighestBit = true;
                    int count = command[1];
                    for (int i = 0; i < count; i++) {
                        u8 next = 0;
                        next = rd.readBits(4);
                        if (x < ret->w) {
                            ARRAY2DPOINT(ppx, x, flippedY, ret->w) = next;
                        }
                        x++;
                    }
                    if (align(count,2)/2 % 2 == 1) {
                        fseek(f, 1, SEEK_CUR);
                    }
                }
            }
            else {
                //encoded mode
                u8 values[] = { command[1] >> 4, command[1] & 0b1111 };
                for (int i = 0; i < command[0]; i++) {
                    if (x < ret->w) {
                        ARRAY2DPOINT(ppx, x, flippedY, ret->w) = values[i%2];
                    }
                    x++;
                }
            }
        }
    }
}

Layer* readBMP(PlatformNativePathString path, u64 seek, OperationProgressReport* report)
{
    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {
        DoOnReturn closeFile([f]() {fclose(f); });
        vsp_BITMAPFILEHEADER header{};
        fread(&header, sizeof(vsp_BITMAPFILEHEADER), 1, f);

        if (header.magic != 'MB') {
            logerr(frmt("[BMP] invalid magic  {:x}", header.magic));
            return NULL;
        }
        loginfo(frmt("[BMP] DataOffset = {:x}", header.dataOffset));

        u64 infoHeaderOffset = ftell(f);
        vsp_BITMAPINFOHEADER infoHeader{};
        fread(&infoHeader, sizeof(vsp_BITMAPINFOHEADER), 1, f);

        loginfo(frmt("[BMP] {}x{}, bitCount={}, compression={}, imageSize={}, numColors={}", infoHeader.width, infoHeader.height, infoHeader.bitCount, infoHeader.compression, infoHeader.imageSize, infoHeader.colorsUsed));

        fseek(f, infoHeaderOffset + infoHeader.size, SEEK_SET);
        std::vector<u32> palette{};
        if (infoHeader.bitCount <= 8) {
            int numColors = infoHeader.bitCount == 1 ? 2
                : infoHeader.bitCount == 4 ? 16
                : infoHeader.bitCount == 8 ? 256
                : 0;
            for (int i = 0; i < numColors; i++) {
                u8 rgb[4];
                fread(rgb, 4, 1, f);
                palette.push_back(PackRGBAtoARGB(rgb[2], rgb[1], rgb[0], 255));
            }
        }
        fseek(f, header.dataOffset, SEEK_SET);

        Layer* ret = NULL;
        if (infoHeader.bitCount == 1) {
            if (infoHeader.compression != 0) {
                logerr(frmt("[BMP] invalid compression for bitcount 1:  {}", infoHeader.compression));
                return NULL;
            }
            //1-bit
            ret = LayerPalettized::tryAllocIndexedLayer(infoHeader.width, infoHeader.height);
            if (ret == NULL) {
                logerr("[BMP] alloc failed");
                return NULL;
            }
            ret->name = "BMP 1-bit layer";
            ((LayerPalettized*)ret)->palette = palette;
            BitReader bits(f);
            bits.readHighestBit = true;
            u32* ppx = ret->pixels32();
            int bytesInLine = align(align(ret->w, 8) / 8, 4);
            for (int y = 0; y < ret->h; y++) {
                int flippedY = ret->h - 1 - y;
                bits.flush();
                fseek(f, header.dataOffset + bytesInLine * y, SEEK_SET);
                for (int x = 0; x < ret->w; x++) {
                    ARRAY2DPOINT(ppx, x, flippedY, ret->w) = bits.readBits(1);
                }
            }
        }
        else if (infoHeader.bitCount == 4) {
            if (infoHeader.compression != 0 && infoHeader.compression != 2) {
                logerr(frmt("[BMP] unsupported compression for bitcount 4:  {}", infoHeader.compression));
                return NULL;
            }
            ret = LayerPalettized::tryAllocIndexedLayer(infoHeader.width, infoHeader.height);
            if (ret == NULL) {
                logerr("[BMP] alloc failed");
                return NULL;
            }
            ret->name = "BMP 4-bit layer";
            ((LayerPalettized*)ret)->palette = palette;
            //uncompressed
            if (infoHeader.compression == 0) {
                BitReader bits(f);
                bits.readHighestBit = true;
                u32* ppx = ret->pixels32();
                int bytesInLine = align(align(ret->w, 2) / 2, 4);
                for (int y = 0; y < ret->h; y++) {
                    int flippedY = ret->h - 1 - y;
                    bits.flush();
                    fseek(f, header.dataOffset + bytesInLine * y, SEEK_SET);
                    for (int x = 0; x < ret->w; x++) {
                        u8 index = (u8)bits.readBits(4);
                        //index = ((index & 0b1) << 3) | ((index & 0b10) << 1) | ((index & 0b100) >> 1) | ((index & 0b1000) >> 3);
                        ARRAY2DPOINT(ppx, x, flippedY, ret->w) = index;
                    }
                }
            }
            //rle 4 bit
            else if (infoHeader.compression == 2) {
                ret->name = "BMP 4-bit RLE layer";
                bmpDecode4BitRLE(ret, f);
            }
        }
        else if (infoHeader.bitCount == 8) {
            if (infoHeader.compression != 0 && infoHeader.compression != 1) {
                logerr(frmt("[BMP] unsupported compression for bitcount 8:  {}", infoHeader.compression));
                return NULL;
            }
            //8-bit
            ret = LayerPalettized::tryAllocIndexedLayer(infoHeader.width, infoHeader.height);
            if (ret == NULL) {
                logerr("[BMP] alloc failed");
                return NULL;
            }
            ret->name = "BMP 8-bit layer";
            ((LayerPalettized*)ret)->palette = palette;
            //uncompressed
            if (infoHeader.compression == 0) {
                u32* ppx = ret->pixels32();
                int bytesInLine = align(ret->w, 4);
                for (int y = 0; y < ret->h; y++) {
                    int flippedY = ret->h - 1 - y;
                    fseek(f, header.dataOffset + bytesInLine * y, SEEK_SET);
                    for (int x = 0; x < ret->w; x++) {
                        u8 next = 0;
                        fread(&next, 1, 1, f);
                        ARRAY2DPOINT(ppx, x, flippedY, ret->w) = next;
                    }
                }
            }
            //rle 8 bit
            else if (infoHeader.compression == 1) {
                ret->name = "BMP 8-bit RLE layer";
                bmpDecode8BitRLE(ret, f);
            }
        }
        else if (infoHeader.bitCount == 24) {
            if (infoHeader.compression != 0) {
                logerr(frmt("[BMP] invalid compression for bitcount 24:  {}", infoHeader.compression));
                return NULL;
            }
            //24-bit
            ret = LayerPalettized::tryAllocLayer(infoHeader.width, infoHeader.height);
            if (ret == NULL) {
                logerr("[BMP] alloc failed");
                return NULL;
            }
            ret->name = "BMP 24-bit layer";
            //uncompressed
            if (infoHeader.compression == 0) {
                u32* ppx = ret->pixels32();
                int bytesInLine = align(ret->w * 3, 4);
                for (int y = 0; y < ret->h; y++) {
                    int flippedY = ret->h - 1 - y;
                    fseek(f, header.dataOffset + bytesInLine * y, SEEK_SET);
                    for (int x = 0; x < ret->w; x++) {
                        u8 next[3] = {0,0,0};
                        fread(&next, 3, 1, f);
                        ARRAY2DPOINT(ppx, x, flippedY, ret->w) = PackRGBAtoARGB(next[2], next[1], next[0], 255);
                    }
                }
            }
        }
        else if (infoHeader.bitCount == 32) {
            if (infoHeader.compression != 0 && infoHeader.compression != 3) {
                logerr(frmt("[BMP] invalid compression for bitcount 32:  {}", infoHeader.compression));
                return NULL;
            }
            //32-bit
            ret = LayerPalettized::tryAllocLayer(infoHeader.width, infoHeader.height);
            if (ret == NULL) {
                logerr("[BMP] alloc failed");
                return NULL;
            }
            ret->name = "BMP 32-bit layer";
            //uncompressed
            bool discardAlpha = infoHeader.compression == 0;
            u32* ppx = ret->pixels32();
            int bytesInLine = ret->w * 4;
            for (int y = 0; y < ret->h; y++) {
                int flippedY = ret->h - 1 - y;
                fseek(f, header.dataOffset + bytesInLine * y, SEEK_SET);
                for (int x = 0; x < ret->w; x++) {
                    u8 next[4] = {0,0,0,0};
                    fread(&next, 4, 1, f);
                    ARRAY2DPOINT(ppx, x, flippedY, ret->w) = PackRGBAtoARGB(next[2], next[1], next[0], discardAlpha ? 255 : next[3]);
                }
            }
        }
        else {
            logerr(frmt("[BMP] unknown bitcount: {}", infoHeader.bitCount));
        }
        return ret;
    }
    return NULL;
}

void bmpWriteToFile(Layer* data, FILE* f, OperationProgressReport* progress, ParameterStore* params)
{
    bool rgb32bit = params != NULL && params->hasParam("bmp.rgb32bit") ? params->getBool("bmp.rgb32bit") : false;
    bool rgbTransparent = params != NULL && params->hasParam("bmp.rgbtransparent") ? params->getBool("bmp.rgbtransparent") : false;
    bool indexedCompressRLE = params != NULL && params->hasParam("bmp.indexedrle") ? params->getBool("bmp.indexedrle") : true;

    progress->enterSection("Writing headers...");
    vsp_BITMAPFILEHEADER h1{};
    vsp_BITMAPINFOHEADER h2{};

    bool writeChannelMasks = !data->isPalettized && rgb32bit && rgbTransparent;
    bool canBe4Bit = data->isPalettized && ((LayerPalettized*)data)->palette.size() <= 16;

    h1.magic = 'MB';
    h1.dataOffset = sizeof(h1) + sizeof(h2) + (writeChannelMasks ? 4*4 : 0) + (data->isPalettized ? (canBe4Bit ? 16 : 256) * 4 : 0);
    h1.reserved = 0;
    u64 sizePosition = 2;
    fwrite(&h1, sizeof(vsp_BITMAPFILEHEADER), 1, f);

    h2.size = sizeof(h2) + (writeChannelMasks ? 4*4 : 0);
    h2.width = data->w;
    h2.height = data->h;
    h2.planes = 1;
    h2.bitCount = data->isPalettized ? (canBe4Bit ? 4 : 8)
        : rgb32bit ? 32 : 24;
    //todo
    h2.compression = data->isPalettized ? (indexedCompressRLE ? (canBe4Bit ? 2 : 1) : 0)
        : (rgb32bit && rgbTransparent) ? 3 : 0;
    h2.imageSize = 0;
    //idk
    h2.xPixelsPerM = 100;
    h2.yPixelsPerM = 100;
    h2.colorsUsed = data->isPalettized ? (canBe4Bit ? 16 : 256)
        : 0;
    h2.colorsImportant = 0;
    fwrite(&h2, sizeof(vsp_BITMAPINFOHEADER), 1, f);

    if (writeChannelMasks) {
        //write bitmapinfoheader v3 masks
        u32 masks[4] = {0xff0000, 0x00ff00, 0x0000ff, 0xff000000};
        fwrite(masks, 4, 4, f);
    }

    u64 imageSizePosition = 0x22;

    if (data->isPalettized) {
        progress->updateLastSection("Writing palette...");
        auto& palette = ((LayerPalettized*)data)->palette;
        for (int i = 0; i < (canBe4Bit ? 16 : 256); i++) {
            //b, g then r
            auto color = uint32ToSDLColor(palette.size() > i ? palette[i] : 0);
            u8 bytes[4] = { color.b, color.g, color.r, 0 };
            fwrite(bytes, 4, 1, f);
        }
    }

    progress->updateLastSection("Writing pixel data...");
    u32* ppx = data->pixels32();
    if (data->isPalettized && indexedCompressRLE) {
        if (canBe4Bit) {
            bmpEncode4BitRLE((LayerPalettized*)data, f);
        }
        else {
            bmpEncode8BitRLE((LayerPalettized*)data, f);
        }
    }
    else {
        for (int y = data->h - 1; y >= 0; y--) {
            int bytesInLine = data->isPalettized ? data->w : data->w * (rgb32bit ? 4 : 3);
            for (int x = 0; x < data->w; x++) {
                u32 px = ARRAY2DPOINT(ppx, x, y, data->w);
                //8bit
                if (data->isPalettized) {
                    u8 a = (u8)px;
                    fwrite(&a, 1, 1, f);
                }
                //24/32bit
                else {
                    auto color = uint32ToSDLColor(px);
                    u8 bytes[4] = { color.b, color.g, color.r, color.a };
                    fwrite(bytes, rgb32bit ? 4 : 3, 1, f);
                }
            }
            //align to 4
            u8 pad[4] = { 0,0,0,0 };
            int padBytes = align(bytesInLine, 4) - bytesInLine;
            if (padBytes > 0) {
                fwrite(pad, padBytes, 1, f);
            }
        }
    }

    u64 finalFileSize = ftell(f);
    u32 imageDataSize = finalFileSize - h1.dataOffset;
    fseek(f, imageSizePosition, SEEK_SET);
    fwrite(&imageDataSize, 4, 1, f);

    fseek(f, sizePosition, SEEK_SET);
    fwrite(&finalFileSize, 4, 1, f);
}

bool writeBMP(PlatformNativePathString path, Layer* data, OperationProgressReport* progress, ParameterStore* params) {

    FILE* f = platformOpenFile(path, PlatformFileModeWB);
    if (f != NULL) {
        DoOnReturn closeFile([f]() {fclose(f); });

        bmpWriteToFile(data, f, progress, params);
        return true;
    }
    return false;
}

bool writeCaveStoryPBM(PlatformNativePathString path, Layer* data, OperationProgressReport* progress) {

    FILE* nfile = platformOpenFile(path, PlatformFileModeWB);
    if (nfile != NULL) {
        bmpWriteToFile(data, nfile, progress, NULL);
        fseek(nfile, 0, SEEK_END);
        fwrite("(C)Pixel", 8, 1, nfile);
        fclose(nfile);
        return true;
    }
    return false;
}