#include "io_base.h"
#include "io_xna.h"

s32 XNA_Read7BE(FILE* f)
{
    s32 ret = 0;
    int bitsRead = 0;
    u8 v = 0;

    do {
        fread(&v, 1, 1, f);
        ret |= (v & 0x7f) << bitsRead;
        bitsRead += 7;
    } while (v & 0x80);

    return ret;
}

std::string XNA_ReadNullTerminatedString(FILE* f)
{
    std::string r = "";
    char next = ' ';

    while (next != '\0') {
        if (fread(&next, 1, 1, f) > 0 && next != '\0') {
            r += next;
        }
        else {
            break;
        }
    }
	fseek(f, -1, SEEK_CUR);
    return r;
}

Layer* readXNB(PlatformNativePathString path, uint64_t seek)
{
    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {
        DoOnReturn d([f]() {fclose(f); });

        XNBHeader header;
        fread(&header, sizeof(header), 1, f);
        loginfo(frmt("[XNB] Target platform: {}",
            header.targetPlatform == 'w' ? "Windows"
            : header.targetPlatform == 'm' ? "Windows Phone 7"
            : header.targetPlatform == 'x' ? "Xbox 360"
            : "???"));
        loginfo(frmt("[XNB] Version: {}", header.xnbVersion));

        if (header.flags & 0x80) {
            logerr("[XNB] Compressed file not supported");
            return NULL;
        }

        s32 typeReaderCount = XNA_Read7BE(f);
        std::vector<std::string> typeReaders;
        for (int i = 0; i < typeReaderCount; i++) {
            std::string readerType = XNA_ReadNullTerminatedString(f);
            typeReaders.push_back(readerType);
            s32 readerVersion;
            fread(&readerVersion, 4, 1, f);
            loginfo(frmt("[XNB] Type reader {}: {}, v{}", i, readerType, readerVersion));
        }

		s32 sharedResourceCount = XNA_Read7BE(f);
		loginfo(frmt("[XNB] Shared resources: {}", sharedResourceCount));

		s32 primaryObjectTypeIndex = XNA_Read7BE(f);
		loginfo(frmt("[XNB] Primary object type index: {}", primaryObjectTypeIndex));

        if (typeReaders.size() >= primaryObjectTypeIndex 
            && typeReaders[primaryObjectTypeIndex-1] == "/Microsoft.Xna.Framework.Content.Texture2DReader") 
        {
			XNATexture2DHeader texHeader;
			fread(&texHeader, sizeof(texHeader), 1, f);
            loginfo(frmt("[XNB] Texture2D - Format: {}, Dimensions: {}x{}, MipCount: {}",
                texHeader.surfaceFormat, texHeader.w, texHeader.h, texHeader.mipCount));

            Layer* l = Layer::tryAllocLayer(texHeader.w, texHeader.h);
            if (l != NULL) {
                u32* ppx = l->pixels32();
                switch (texHeader.surfaceFormat-1) {
                    case XNATexture_Color:  //RRGGBBAA
                        l->name = "XNA Color Layer";
                        for (u64 i = 0; i < (u64)texHeader.w * texHeader.h; i++) {
                            XNAColor c;
							fread(&c, sizeof(c), 1, f);
							*(ppx++) = PackRGBAtoARGB(c.r, c.g, c.b, c.a);
                        }
                        break;
                    case XNATexture_BGR565:
                        l->name = "XNA BGR565 Layer";
                        for (u64 i = 0; i < (u64)texHeader.w * texHeader.h; i++) {
                            u16 raw;
                            fread(&raw, 2, 1, f);
                            *(ppx++) = BGR565toARGB8888(raw);
                        }
                        break;
                    default:
						logerr(frmt("[XNB] Unsupported Texture2D format: {}", texHeader.surfaceFormat));
                        delete l;
                        l = NULL;
                        break;
                }
            }

            return l;
        }
        else {
			logerr("[XNB] Not a Texture2D");
        }

    }
    return NULL;
}
