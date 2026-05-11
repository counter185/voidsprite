#include "../libbzip2/bzlib.h"

#include "io_base.h"
#include "io_gamemaker.h"

Layer* readYoYoTex(PlatformNativePathString path, u64 seek) {

	FILE* f = platformOpenFile(path, PlatformFileModeRB);
	if (f != NULL) {
		DoOnReturn closeFile([f]() {fclose(f); });
		yoyoTexHeader header;

		fread(&header, 12, 1, f);
		if (memcmp(header.magic, "fioq", 4) == 0) {
			//uncompressed qoif

            fseek(f, 0, SEEK_END);
            u64 fileSize = ftell(f);
            fseek(f, 0, SEEK_SET);

            std::vector<u8> data;
            data.resize(fileSize);
            fread(data.data(), 1, fileSize, f);

            return readQoifFromMem(data);
		}
		else if (memcmp(header.magic, "2zoq", 4) == 0) {
			//compressed yoyotex, bz2 starts now at 0x0C
			std::vector<u8> decompressedBuffer;
			decompressedBuffer.resize(header.len);

            std::vector<u8> compressedBuffer;
            fseek(f, 0, SEEK_END);
            u64 fileSize = ftell(f);
            fseek(f, 12, SEEK_SET);
            compressedBuffer.resize(fileSize - 12);
            fread(compressedBuffer.data(), 1, fileSize - 12, f);
            u32 bufferSize = decompressedBuffer.size();

            int result = BZ2_bzBuffToBuffDecompress((char*)decompressedBuffer.data(), &bufferSize, (char*)compressedBuffer.data(), compressedBuffer.size(), 0, 0);

            if (result == BZ_MEM_ERROR) {
                g_addNotificationFromThread(NOTIF_MALLOC_FAIL);
                return NULL;
            }
            else if (result == BZ_OK) {
                return readQoifFromMem(decompressedBuffer);
            }
            else {
                logerr(frmt("bzip2 error {}", result));
                return NULL;
            }
		}
		else {
			//invalid file
            logerr("invalid yytex file");
			return NULL;
		}
	}
	return NULL;
}

bool writeYoYoTex(PlatformNativePathString path, Layer* data, OperationProgressReport* progress, ParameterStore* params)
{
    bool compressed = (params == NULL || !params->hasParam("yytex.compressed")) ? true : params->getBool("yytex.compressed");
    FILE* f = platformOpenFile(path, PlatformFileModeWB);
    if (f != NULL) {
        DoOnReturn closeF([f]() {fclose(f); });
        std::vector<u8> qoifData = writeQoifToMem(data);
        if (!compressed) {
            fwrite(qoifData.data(), 1, qoifData.size(), f);
            return true;
        }
        else {
            yoyoTexHeader header = yoyoTexHeader {
                .w = (u16)data->w,
                .h = (u16)data->h,
                .len = (u32)qoifData.size()
            };
            memcpy(&header.magic, "2zoq", 4);
            std::vector<u8> compressedBuffer;
            //as per bzip2 documentation...
            u64 bufferSize = (u64)((double)qoifData.size() * 1.01) + 600;
            if (bufferSize >= 0xFFFFFFFFU) {
                //uh oh
                logerr("file too large for bzip2");
                return false;
            }
            compressedBuffer.resize(bufferSize);
            u32 len = qoifData.size();
            int result = BZ2_bzBuffToBuffCompress((char*)compressedBuffer.data(), (u32*)&bufferSize, (char*)qoifData.data(), len, 9, 0, 1);
            if (result == BZ_OK) {
                fwrite(&header, 12, 1, f);
                fwrite(compressedBuffer.data(), 1, bufferSize, f);
                return true;
            }
            else {
                logerr(frmt("bz2 returned {}", result));
                return false;
            }
            //BZ2_bzBuffToBuffCompress()
        }
    }
    return false;
}

// adapted from C# code:
// https://github.com/FeiZhaixiage/yytexUnpacker/blob/master/Program.cs
std::vector<u8> writeQoifToMem(Layer* l) {
    yoyoTexHeader header = {
        .w = (u16)l->w,
        .h = (u16)l->h,
        .len = 0
    };
    memcpy(&header.magic, "fioq", 4);

    std::vector<u8> encData;
    u8 r = 0, g = 0, b = 0, a = 255;
    int run = 0;
    int colPrev = 0xff;
    u32 index[64];
    memset(index, 0, 4 * 64);

    u32* ppx = l->pixels32();
    u64 i = 0;
    u64 pixelCount = (u64)l->w * l->h;
    while (i < pixelCount) {
        u32 color = ppx[i++];
        SDL_Color col = uint32ToSDLColor(color);
        r = col.r;
        g = col.g;
        b = col.b;
        a = col.a;
        u32 colorRRGGBBAA = (color << 8) | col.a;
        if (colorRRGGBBAA == colPrev) {
            run++;
        }
        if (run > 0 &&
            (run == 0x2020 || colorRRGGBBAA != colPrev || i == pixelCount)) 
        {
            if (run < 33) {
                run--;
                encData.push_back(QOI_RUN_8 | run);
            }
            else {
                run -= 33;
                encData.push_back(QOI_RUN_16 | (run >> 8));
                encData.push_back(run);
            }
            run = 0;
        }
        if (colorRRGGBBAA != colPrev) {
            int indexPos = (r ^ g ^ b ^ a) & 63;
            if (index[indexPos] == colorRRGGBBAA) {
                encData.push_back(QOI_INDEX | indexPos);
            }
            else {
                index[indexPos] = colorRRGGBBAA;

                int vr = r - (u8)(colPrev >> 24);
                int vg = g - (u8)(colPrev >> 16);
                int vb = b - (u8)(colPrev >> 8);
                int va = a - (u8)(colPrev);
                if (vr > -17 && vr < 16
                    && vg > -17 && vg < 16
                    && vb > -17 && vb < 16
                    && va > -17 && va < 16) 
                {
                    if (va == 0
                        && vr > -3 && vr < 2
                        && vg > -3 && vg < 2
                        && vb > -3 && vb < 2)
                    {
                        encData.push_back(QOI_DIFF_8 | (vr << 4 & 48) | (vg << 2 & 12) | (vb & 3));
                    }
                    else if (va == 0
                        && vg > -9 && vg < 8
                        && vb > -9 && vb < 8)
                    {
                        encData.push_back(QOI_DIFF_16 | (vr & 31));
                        encData.push_back((vg << 4 & 240) | (vb & 15));
                    }
                    else {
                        encData.push_back(QOI_DIFF_24 | (vr >> 1 & 15));
                        encData.push_back((vr << 7 & 128) | (vg << 2 & 124) | (vb >> 3 & 3));
                        encData.push_back((vb << 5 & 224) | (va & 31));
                    }
                }
                else {
                    encData.push_back(QOI_COLOR | (vr != 0 ? 8 : 0) | (vg != 0 ? 4 : 0) | (vb != 0 ? 2 : 0) | (va != 0 ? 1 : 0));
                    if (vr != 0) {
                        encData.push_back(r);
                    }
                    if (vg != 0) {
                        encData.push_back(g);
                    }
                    if (vb != 0) {
                        encData.push_back(b);
                    }
                    if (va != 0) {
                        encData.push_back(a);
                    }
                }
            }
        }
        colPrev = colorRRGGBBAA;
    }
    encData.push_back(0);
    encData.push_back(0);
    encData.push_back(0);
    encData.push_back(0);

    header.len = encData.size();

    std::vector<u8> headerBytes;
    headerBytes.resize(12);
    *(yoyoTexHeader*)headerBytes.data() = header;
    return joinVectors({ headerBytes, encData });
}

Layer* readQoifFromMem(std::vector<u8>& bytes)
{
    yoyoTexHeader header = *(yoyoTexHeader*)bytes.data();

    u8 index[256];
    memset(index, 0, 256);
    u8 r = 0, g = 0, b = 0, a = 255;
    int currentRLERun = 0;
    u64 pos = 0;
    u64 filePtr = 12;

    Layer* l = Layer::tryAllocLayer(header.w, header.h);
    if (l != NULL) {
        u32* ppx = l->pixels32();
        l->name = "QOIF Layer";
        while (filePtr < bytes.size() && pos < (u64)l->w * l->h) {
            if (currentRLERun > 0) {
                ppx[pos++] = PackRGBAtoARGB(r, g, b, a);
                currentRLERun--;
            }
            else {
                u8 cmd = bytes[filePtr++];

                if ((cmd & QOI_MASK_2) == QOI_INDEX)
                {
                    int indexPos = (cmd ^ QOI_INDEX) << 2;
                    r = index[indexPos];
                    g = index[indexPos + 1];
                    b = index[indexPos + 2];
                    a = index[indexPos + 3];
                }
                else if ((cmd & QOI_MASK_3) == QOI_RUN_8)
                {
                    currentRLERun = cmd & 0x1f;
                }
                else if ((cmd & QOI_MASK_3) == QOI_RUN_16)
                {
                    u8 b2 = bytes[filePtr++];
                    currentRLERun = (((cmd & 0x1f) << 8) | b2) + 32;
                }
                else if ((cmd & QOI_MASK_2) == QOI_DIFF_8)
                {
                    r += (u8)(((cmd & 48) << 26 >> 30) & 0xff);
                    g += (u8)(((cmd & 12) << 28 >> 22 >> 8) & 0xff);
                    b += (u8)(((cmd & 3) << 30 >> 14 >> 16) & 0xff);
                }
                else if ((cmd & QOI_MASK_3) == QOI_DIFF_16)
                {
                    u8 b2 = bytes[filePtr++];
                    u16 merged = cmd << 8 | b2;
                    r += (u8)(((merged & 7936) << 19 >> 27) & 0xff);
                    g += (u8)(((merged & 240) << 24 >> 20 >> 8) & 0xff);
                    b += (u8)(((merged & 15) << 28 >> 12 >> 16) & 0xff);
                }
                else if ((cmd & QOI_MASK_4) == QOI_DIFF_24)
                {
                    u8 b2 = bytes[filePtr++];
                    u8 b3 = bytes[filePtr++];
                    int merged = cmd << 16 | b2 << 8 | b3;
                    r += (u8)(((merged & 1015808) << 12 >> 27) & 0xff);
                    g += (u8)(((merged & 31744) << 17 >> 19 >> 8) & 0xff);
                    b += (u8)(((merged & 992) << 22 >> 11 >> 16) & 0xff);
                    a += (u8)(((merged & 31) << 27 >> 3 >> 24) & 0xff);
                }
                else if ((cmd & QOI_MASK_4) == QOI_COLOR)
                {
                    if ((cmd & 8) != 0)
                        r = bytes[filePtr++];
                    if ((cmd & 4) != 0)
                        g = bytes[filePtr++];
                    if ((cmd & 2) != 0)
                        b = bytes[filePtr++];
                    if ((cmd & 1) != 0)
                        a = bytes[filePtr++];
                }
                u8 indexPos2 = ((r ^ g ^ b ^ a) & 63) << 2;
                index[indexPos2] = r;
                index[indexPos2 + 1] = g;
                index[indexPos2 + 2] = b;
                index[indexPos2 + 3] = a;

                ppx[pos++] = PackRGBAtoARGB(r, g, b, a);
            }

        }
        return l;
    }
    else {
        g_addNotificationFromThread(NOTIF_MALLOC_FAIL);
        return NULL;
    }
}
