#pragma once

#include <fstream>

#include "../globals.h"
#include "../mathops.h"
#include "../maineditor.h"
#include "../Notification.h"
#include "../MainEditorPalettized.h"
#include "../LayerPalettized.h"

class BitReader {
private:
    FILE* f = NULL;
    u8* data = NULL;
    u64 dataSize = 0;
    u64 dataPos = 0;
    bool usingFile = false;

    int bitsInBuffer = 0;
    u8 buffer = 0;
public:
    BitReader(FILE* file) : f(file), usingFile(true) {}
    BitReader(u8* inputData, u64 size) : data(inputData), dataSize(size), usingFile(false) {}
    u32 readBits(int n) {
        int result = 0;
        int bitsRead = 0;
        while (bitsRead < n) {
            if (bitsInBuffer == 0) {
                if (usingFile) {
                    fread(&buffer, 1, 1, f);
                }
                else {
                    if (dataPos >= dataSize) {
                        throw std::runtime_error("[BitReader] attempt to read past end of data");
                        return result;
                    }
                    buffer = *(data + dataPos++);
                }
                bitsInBuffer = 8;
            }
            //int bytesLeft = n - bitsRead;
            //int readBits = ixmin(bitsInBuffer, bytesLeft);
            result |= (buffer & 1) << bitsRead;
            buffer >>= 1;
            bitsInBuffer--;
            bitsRead++;
        }
        return result;
    }
};

class BitWriter {
private:
    FILE* f = NULL;
    std::vector<u8> data;
    bool usingFile = false;

    u8 buffer = 0;
    int bitsInBuffer = 0;
public:
    BitWriter(FILE* file) : f(file), usingFile(true) {}
    BitWriter() : usingFile(false) {}

    void writeBits(u32 bits, int n) {
        int bitsWritten = 0;
        while (bitsWritten < n) {
            buffer |= (bits & 1) << bitsInBuffer;
            bits >>= 1;
            bitsInBuffer++;
            bitsWritten++;
            if (bitsInBuffer == 8) {
                if (usingFile) {
                    fwrite(&buffer, 1, 1, f);
                }
                else {
                    data.push_back(buffer);
                }
                buffer = 0;
                bitsInBuffer = 0;
            }
        }
    }
    void flush() {
        if (bitsInBuffer > 0) {
            if (usingFile) {
                fwrite(&buffer, 1, 1, f);
            }
            else {
                data.push_back(buffer);
            }
            buffer = 0;
            bitsInBuffer = 0;
        }
    }

    std::vector<u8>& getData() {
        return data;
    }
};

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

/// <summary>
/// Flips the byte order if the machine isn't big-endian
/// </summary>
inline u32 beU32(u32 v) {
    //todo: do nothing on big-endian?
    return ((v & 0x000000FF) << 24) | ((v & 0x0000FF00) << 8) | ((v & 0x00FF0000) >> 8) | ((v & 0xFF000000) >> 24);
}

/// <summary>
/// Flips the byte order if the machine isn't big-endian
/// </summary>
inline u16 beU16(u16 v) {
    return ((v & 0x00FF) << 8) | ((v & 0xFF00) >> 8);
}


u8* decompressZlib(u8* data, u64 compressedSize, u64 decompressedSize);