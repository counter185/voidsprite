#pragma once

#include <fstream>

#include "../globals.h"
#include "../mathops.h"
#include "../maineditor.h"
#include "../Notification.h"
#include "../MainEditorPalettized.h"
#include "../LayerPalettized.h"

class RIFFChunk {
public:
    std::string id = "    ";
    std::vector<u8> data;
    u64 writtenAt = 0;

    RIFFChunk(std::string chunkID) : id(chunkID) {}
    RIFFChunk(std::string chunkID, u8* fromData, u64 dataSize) : id(chunkID) {
        data.resize(dataSize);
        memcpy(data.data(), fromData, dataSize);
    }

    virtual u64 write(FILE* f) {
        writtenAt = ftell(f);
        u32 size = (u32)data.size();
        fwrite(id.c_str(), 1, 4, f);
        fwrite(&size, 4, 1, f);
        if (size > 0) {
            fwrite(data.data(), 1, size, f);
        }
        if (size % 2 == 1) {
            fwrite("\0", 1, 1, f);
            size++;
        }
        return 8 + size;
    }
};

class RIFFLargeDataChunk : public RIFFChunk {
public:
    std::function<std::vector<u8>()> loadFunction;

    RIFFLargeDataChunk(std::string chunkID, std::function<std::vector<u8>()> fn) : RIFFChunk(chunkID), loadFunction(fn) { }

    u64 write(FILE* f) override {
        data = loadFunction();
        u64 res = RIFFChunk::write(f);
        data.clear();
        return res;
    }
};
class RIFFJUNKChunk : public RIFFChunk {
public:
    int alignTo = 512;

    RIFFJUNKChunk(int align = 512) : RIFFChunk("JUNK"), alignTo(align) {}

    u64 write(FILE* f) override {
        writtenAt = ftell(f);
        fwrite(id.c_str(), 1, 4, f);
        u32 rqBytes = alignTo - ((ftell(f) + 4) % 512);
        fwrite(&rqBytes, 4, 1, f);
        std::vector<u8> empty;
        empty.resize(rqBytes);
        fwrite(empty.data(), rqBytes, 1, f);
        if (rqBytes % 2 == 1) {
            fwrite("\0", 1, 1, f);
            rqBytes++;
        }
        return rqBytes + 8;
    }
};
class RIFFListChunk : public RIFFChunk {
public:
    std::string listType = "    ";
    std::vector<RIFFChunk*> otherChunks;

    RIFFListChunk(std::string id, std::string type, std::vector<RIFFChunk*> chunks) : RIFFChunk(id), listType(type), otherChunks(chunks) {}
    ~RIFFListChunk() {
        for (auto& c : otherChunks) {
            delete c;
        }
    }

    u64 write(FILE* f) override {
        writtenAt = ftell(f);
        fwrite(id.c_str(), 1, 4, f);
        u64 sizePos = ftell(f);
        u32 size = 4;
        fwrite("0000", 1, 4, f);
        fwrite(listType.c_str(), 1, 4, f);
        for (auto& chunk : otherChunks) {
            size += chunk->write(f);
        }
        fseek(f, sizePos, SEEK_SET);
        fwrite(&size, 4, 1, f);
        fseek(f, 0, SEEK_END);
        return size + 8;
    }
};

u8* decompressZlib(u8* data, u64 compressedSize, u64 decompressedSize);