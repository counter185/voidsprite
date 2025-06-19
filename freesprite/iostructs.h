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

struct VTFHEADER
{
    char            signature[4];       // File signature ("VTF\0"). (or as little-endian integer, 0x00465456)
    unsigned int    version[2];         // version[0].version[1] (currently 7.2).
    unsigned int    headerSize;         // Size of the header struct  (16 byte aligned; currently 80 bytes) + size of the resources dictionary (7.3+).
    unsigned short  width;              // Width of the largest mipmap in pixels. Must be a power of 2.
    unsigned short  height;             // Height of the largest mipmap in pixels. Must be a power of 2.
    unsigned int    flags;              // VTF flags.
    unsigned short  frames;             // Number of frames, if animated (1 for no animation).
    unsigned short  firstFrame;         // First frame in animation (0 based). Can be -1 in environment maps older than 7.5, meaning there are 7 faces, not 6.
    unsigned char   padding0[4];        // reflectivity padding (16 byte alignment).
    float           reflectivity[3];    // reflectivity vector.
    unsigned char   padding1[4];        // reflectivity padding (8 byte packing).
    float           bumpmapScale;       // Bumpmap scale.
    int             highResImageFormat; // High resolution image format.
    unsigned char   mipmapCount;        // Number of mipmaps.
    int             lowResImageFormat;  // Low resolution image format (Usually DXT1).
    unsigned char   lowResImageWidth;   // Low resolution image width.
    unsigned char   lowResImageHeight;  // Low resolution image height.

    // 7.2+
    unsigned short  depth;              // Depth of the largest mipmap in pixels. Must be a power of 2. Is 1 for a 2D texture.

    // 7.3+
    unsigned char   padding2[3];        // depth padding (4 byte alignment).
    unsigned int    numResources;       // Number of resources this vtf has. The max appears to be 32.

    unsigned char   padding3[8];        // Necessary on certain compilers
};
struct VTF_RESOURCE_ENTRY
{
    unsigned char	tag[3]; 		// A three-byte "tag" that identifies what this resource is.
    unsigned char	flags;			// Resource entry flags. The only known flag is 0x2, which indicates that no data chunk corresponds to this resource.
    unsigned int	offset;			// The offset of this resource's data in the file. 
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