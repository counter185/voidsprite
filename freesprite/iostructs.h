#pragma once

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

struct ASEPRITEHeader {
    u32 fileSize;
    u16 magic;  //0xA5E0
    u16 numFrames;
    u16 width;
    u16 height;
    u16 colorDepth; //32: RGBA, 16: grayscale ????, 8: indexed
    u32 flags;
    u16 speedDeprecated;
    u32 reserved0[2];
    u8 paletteEntryThatIsTransparent;
    u8 ignore[3];
    u16 numColors;  //0 is 256 for old files
    u8 pixelWidth;
    u8 pixelHeight;
    s16 gridX;
    s16 gridY;
    u16 gridWidth;
    u16 gridHeight;
    u8 reserved1[84];
};

struct ASEPRITEFrameHeader {
    u32 bytesInFrame;
    u16 magic;  //0xF1FA
    u16 numChunksOld;
    u16 frameDuration;
    u8 reserved[2];
    u32 numChunksNew;
};

struct ASEPRITECelChunkFragment0 {
    u16 layerIndex;
    s16 x;
    s16 y;
    u8 opacity;
    u16 type;
    s16 zIndex;
    u8 reserved[5];
};

struct ASEPRITECelChunkFragment03 {
    u16 widthInTiles;
    u16 heightInTiles;
    u16 bitsPerTile;    //always 32 right now?
    u32 tileIDBitmask;
    u32 xFlipBitmask;
    u32 yFlipBitmask;
    u32 diagonalFlipBitmask;
    u8 reserved[10];
};

struct ASEPRITELayerChunkFragment0 {
    u16 flags;
    u16 layerType;
    u16 layerChildLevel;
    u16 defaultWidth;   //ignored
    u16 defaultHeight;  //ignored
    u16 blendMode;
    u8 opacity;
    u8 reserved[3];
};

struct ASEPRITERGBAPixel {
    u8 r, g, b, a;
};
struct ASEPRITEIA88Pixel {
	u8 i, a;
};

#pragma pack(pop)