#pragma once

//todo: get rid of this, move the structs to individual `io_<whatever>.h/.cpp`

#pragma pack(push, 1)
struct TPLImageOffset {
    uint32_t headerOffset;
    uint32_t paletteHeader;
};
struct TPLImageHeader {
    uint16_t height;
    uint16_t width;
    uint32_t format;
    uint32_t imageDataAddress;
    uint32_t wrapS;
    uint32_t wrapT;
    uint32_t minFilter;
    uint32_t magFilter;
    float LODBias;
    uint8_t edgeLODEnable;
    uint8_t minLOD;
    uint8_t maxLOD;
    uint8_t unpacked;
};
struct TPLPaletteHeader {
    uint16_t entryCount;
    uint8_t unpacked;
    uint8_t paddingByte;
    uint32_t paletteFormat;
    uint32_t paletteDataAddress;

    bool set;
};
struct TPLImage {
    TPLImageHeader imgHdr;
    TPLPaletteHeader pltHdr;
};

struct MSPHeader
{
    uint16_t  Key1;             /* Magic number    */
    uint16_t  Key2;             /* Magic number    */
    uint16_t  Width;            /* Width of the bitmap in pixels   */
    uint16_t  Height;           /* Height of the bitmap in pixels   */
    uint16_t  XARBitmap;        /* X Aspect ratio of the bitmap   */
    uint16_t  YARBitmap;        /* Y Aspect ratio of the bitmap   */
    uint16_t  XARPrinter;       /* X Aspect ratio of the printer   */
    uint16_t  YARPrinter;       /* Y Aspect ratio of the printer   */
    uint16_t  PrinterWidth;     /* Width of the printer in pixels   */
    uint16_t  PrinterHeight;    /* Height of the printer in pixels   */
    uint16_t  XAspectCorr;      /* X aspect correction (unused)     */
    uint16_t  YAspectCorr;      /* Y aspect correction (unused)     */
    uint16_t  Checksum;         /* Checksum of previous 24 bytes   */
    uint16_t  Padding[3];       /* Unused padding    */
};

struct NCCHHeader {
    u8 rsaSignature[0x100];
    u32 magic;
    u32 contentSize;
    u64 partitionID;
    u16 makerCode;
    u16 version;
    u32 flag;
    u64 programID;
    u8 reserved[0x10];
    u8 logoRegionHash[0x20];
    u8 productCode[0x10];
    u8 extHeaderHash[0x20];
    u32 extHeaderSize;
    u32 reserved2;
    u64 flags;
    u32 plainRegionOffset;
    u32 plainRegionSize;
    u32 logoRegionOffset;
    u32 logoRegionSize;
    u32 exefsOffset;
    u32 exefsSize;
    u32 exefsHashRegionSize;
};

struct ExeFSFileHeader {
    char fileName[0x8];
    u32 fileOffset;
    u32 fileSize;
};
struct ExeFSHeader {
    ExeFSFileHeader fileHeaders[0xa];
    u8 reserved[0x20];
    u8 fileHashes[0x140];
};

#pragma pack(pop)