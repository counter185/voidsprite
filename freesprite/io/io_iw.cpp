#include "io_base.h"
#include "io_iw.h"

void DeXT1(Layer* ret, int width, int height, FILE* infile);
void DeXT23(Layer* ret, int width, int height, FILE* infile);
void DeXT45(Layer* ret, int width, int height, FILE* infile);

const u8 IWI_VERSION_COD2 = 0x05;
const u8 IWI_VERSION_COD4 = 0x06;
const u8 IWI_VERSION_COD5 = 0x06;
const u8 IWI_VERSION_CODMW2 = 0x08;
const u8 IWI_VERSION_CODMW3 = 0x08;
const u8 IWI_VERSION_CODBO1 = 0x0D;
const u8 IWI_VERSION_CODBO2 = 0x1B;

const u8 IWI_FORMAT_ARGB32 = 0x01;
const u8 IWI_FORMAT_RGB24 = 0x02;
const u8 IWI_FORMAT_GA16 = 0x03;
const u8 IWI_FORMAT_A8 = 0x04;
const u8 IWI_FORMAT_DXT1 = 0x0B;
const u8 IWI_FORMAT_DXT3 = 0x0C;
const u8 IWI_FORMAT_DXT5 = 0x0D;

Layer* readIWI(PlatformNativePathString path, u64 seek)
{
    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {

        u64 filesize;
		fseek(f, 0, SEEK_END);
		filesize = ftell(f);
		fseek(f, 0, SEEK_SET);

        IWIFile iwi;
        IWIInfo iwiinf;
        fread(&iwi, sizeof(IWIFile), 1, f);

        loginfo(frmt("[IWI] version: {:02X}", iwi.version));

        if (iwi.version == IWI_VERSION_CODMW2) {
            fseek(f, 0x8, SEEK_SET);
        }
        fread(&iwiinf, sizeof(IWIInfo), 1, f);
        loginfo(frmt("[IWI] format: {}\n"
                     "  usage: {}\n"
                     "  w,h: {} {}\n"
                     "  depth: {}", 
            iwiinf.format, iwiinf.usage, iwiinf.w, iwiinf.h, iwiinf.depth));

        struct mipOffsetEntry {
            u32 offset;
            u32 calcSize;
        };

        std::vector<u32> rawMipOffsetTable;
        int nOffsets = 4;
        if (iwi.version == IWI_VERSION_CODBO1) {
            fseek(f, 0x10, SEEK_SET);
            nOffsets = 8;
        }
        else if (iwi.version == IWI_VERSION_CODBO2) {
            fseek(f, 0x20, SEEK_SET);
            nOffsets = 8;
        }

        rawMipOffsetTable.resize(nOffsets);
        fread(&rawMipOffsetTable[0], sizeof(u32), nOffsets, f);
		std::sort(rawMipOffsetTable.begin(), rawMipOffsetTable.end());
		std::vector<mipOffsetEntry> mipOffsetTable;
        for (int i = 0; i < nOffsets; i++) {
			u32 thisOffset = rawMipOffsetTable[i];
			u32 nextOffset = (i + 1 < nOffsets) ? rawMipOffsetTable[i + 1] : filesize;
            u32 size = nextOffset - thisOffset;
            if (size != 0) {
                mipOffsetTable.push_back(mipOffsetEntry{
                    thisOffset,
                    size
                });
            }
        }
        //sort by largest size
        std::sort(mipOffsetTable.begin(), mipOffsetTable.end(), [](const mipOffsetEntry& a, const mipOffsetEntry& b) {
            return a.calcSize > b.calcSize;
		});
        loginfo("mip offsets:");
        for (auto& offs : mipOffsetTable) {
            loginfo(frmt("  {:08X} size {:08X}", offs.offset, offs.calcSize));
        }
        //sometimes they just all have the same offset that points to end of file
        //this somehow corrects this
        if (mipOffsetTable.empty()) {
            mipOffsetTable.push_back(mipOffsetEntry{
                (u32)ftell(f),
                (u32)filesize
            });
        }

        //let's assume first mipmap is the largest
		fseek(f, mipOffsetTable[0].offset, SEEK_SET);
        Layer* ret = Layer::tryAllocLayer(iwiinf.w, iwiinf.h);
        if (ret != NULL) {
            switch (iwiinf.format) {
                case IWI_FORMAT_DXT1:
                    ret->name = "IWI DXT1 Layer";
					DeXT1(ret, iwiinf.w, iwiinf.h, f);
                    break;
                case IWI_FORMAT_DXT3:
                    ret->name = "IWI DXT3 Layer";
					DeXT23(ret, iwiinf.w, iwiinf.h, f);
                    break;
                case IWI_FORMAT_DXT5:
                    ret->name = "IWI DXT5 Layer";
                    DeXT45(ret, iwiinf.w, iwiinf.h, f);
                    break;
                default:
                    delete ret;
					ret = NULL;
                    logerr("[IWI] unsupported format");
                    break;
            }
        }

        fclose(f);
        return ret;
    }
    return NULL;
}