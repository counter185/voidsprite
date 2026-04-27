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

// adapted from C# code:
// https://github.com/FeiZhaixiage/yytexUnpacker/blob/master/Program.cs
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
