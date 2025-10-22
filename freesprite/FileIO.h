#pragma once
#include "globals.h"
#include "Layer.h"
#include "iostructs.h"

#define FORMAT_RGB			0b01
#define FORMAT_PALETTIZED	0b10

std::string getAllLibsVersions();

std::map<std::string, std::string> parseINI(PlatformNativePathString path);

uint8_t* DecompressMarioPaintSRM(FILE* f);

void DeXT1(Layer* ret, int width, int height, FILE* infile);
void DeXT23(Layer* ret, int width, int height, FILE* infile);
void DeXT45(Layer* ret, int width, int height, FILE* infile);

int DeASTC(Layer* ret, int width, int height, uint64_t fileLength, FILE* infile, int blockWidth = 8, int blockHeight = 8);

LayerPalettized* De4BPPBitplane(int width, int height, uint8_t* input);

Layer* _VTFseekToLargestMipmapAndRead(FILE* infile, int width, int height, int mipmapCount, int frames, int imageFormat);

std::vector<u8> decompressZlibWithoutUncompressedSize(u8* data, size_t dataSize);
std::vector<u8> compressZlib(u8* data, size_t dataSize);
std::vector<u8> base64ToBytes(std::string b64);

void zlibFile(PlatformNativePathString path);
void unZlibFile(PlatformNativePathString path);

std::function<bool(PlatformNativePathString)> magicVerify(u64 at, std::string header);

PlatformNativePathString newTempFile();

#include "io/io_png.h"
#include "io/io_aseprite.h"
#include "io/io_piskel.h"
#include "io/io_pixil.h"
#include "io/io_lpe.h"
#include "io/io_pix2d.h"
#include "io/io_gim.h"
#include "io/io_rpgm.h"
#include "io/io_jxl.h"
#include "io/io_dibv5.h"
#include "io/io_valve_spr.h"
#include "io/io_voidsprite.h"
#include "io/io_openraster.h"
#include "io/io_pixelstudio.h"

Layer* readTGA(PlatformNativePathString path, uint64_t seek = 0);
Layer* readBMP(PlatformNativePathString path, uint64_t seek = 0);
Layer* readAETEX(PlatformNativePathString path, uint64_t seek = 0);
Layer* readSDLImage(PlatformNativePathString path, uint64_t seek = 0);
Layer* readWiiGCTPL(PlatformNativePathString path, uint64_t seek = 0);
Layer* readNES(PlatformNativePathString path, uint64_t seek = 0);
Layer* readDDS(PlatformNativePathString path, uint64_t seek = 0);
Layer* readVTF(PlatformNativePathString path, uint64_t seek = 0);
Layer* readGCI(PlatformNativePathString path, uint64_t seek = 0);
Layer* readMSP(PlatformNativePathString path, uint64_t seek = 0);
Layer* readMarioPaintSRM(PlatformNativePathString path, uint64_t seek = 0);
Layer* readXComSPK(PlatformNativePathString path, uint64_t seek = 0);
Layer* readXComBDY(PlatformNativePathString path, uint64_t seek = 0);
Layer* readXComSCR(PlatformNativePathString path, uint64_t seek = 0);
Layer* readAnymapPBM(PlatformNativePathString path, uint64_t seek = 0);
Layer* readAnymapPGM(PlatformNativePathString path, uint64_t seek = 0);
Layer* readAnymapPPM(PlatformNativePathString path, uint64_t seek = 0);
Layer* readXBM(PlatformNativePathString path, uint64_t seek = 0);
Layer* readSR8(PlatformNativePathString path, uint64_t seek = 0);
Layer* readVOID9SP(PlatformNativePathString path, uint64_t seek = 0);
Layer* readPS2ICN(PlatformNativePathString path, uint64_t seek = 0);
Layer* readNDSBanner(PlatformNativePathString path, uint64_t seek = 0);
Layer* read3DSCXIIcon(PlatformNativePathString path, uint64_t seek = 0);
Layer* readGIF(PlatformNativePathString path, u64 seek = 0);
Layer* readGXT(PlatformNativePathString path, u64 seek = 0);
Layer* readWinSHS(PlatformNativePathString path, u64 seek = 0);

Layer* loadAnyIntoFlat(std::string utf8path, FileImporter** outputFoundImporter = NULL);
MainEditor* loadAnyIntoSession(std::string utf8path, FileImporter** outputFoundImporter = NULL);

bool writeBMP(PlatformNativePathString path, Layer* data);
bool writeJPEG(PlatformNativePathString path, Layer* data);
bool writeAVIF(PlatformNativePathString path, Layer* data);
bool writeTGA(PlatformNativePathString path, Layer* data);
bool writeCaveStoryPBM(PlatformNativePathString path, Layer* data);
bool writeXBM(PlatformNativePathString path, Layer* data);
bool writeCHeader(PlatformNativePathString path, Layer* data);
bool writePythonNPArray(PlatformNativePathString path, Layer* data);
bool writeHTMLBase64(PlatformNativePathString path, Layer* data);
bool writeJavaBufferedImage(PlatformNativePathString path, Layer* data);
bool writeAnymapTextPBM(PlatformNativePathString path, Layer* data);
bool writeAnymapTextPGM(PlatformNativePathString path, Layer* data);
bool writeAnymapTextPPM(PlatformNativePathString path, Layer* data);
bool writeSR8(PlatformNativePathString path, Layer* data);
bool writeCUR(PlatformNativePathString path, Layer* data);
bool writeVTF(PlatformNativePathString path, Layer* data);

std::pair<bool, std::vector<uint32_t>> readPltJASCPAL(PlatformNativePathString name);
std::pair<bool, std::vector<uint32_t>> readPltGIMPGPL(PlatformNativePathString name);
std::pair<bool, std::vector<uint32_t>> readPltHEX(PlatformNativePathString name);
std::pair<bool, std::vector<uint32_t>> readPltPDNTXT(PlatformNativePathString name);
std::pair<bool, std::vector<uint32_t>> readPltPixelStudioPALETTE(PlatformNativePathString name);

std::pair<bool, NineSegmentPattern> read9SegmentPattern(PlatformNativePathString path);
bool write9SegmentPattern(PlatformNativePathString path, Layer* data, XY point1, XY point2);

//SplitSessionData loadSplitSessionData(PlatformNativePathString path);

MainEditor* loadSplitSession(PlatformNativePathString path);
bool saveSplitSession(PlatformNativePathString path, MainEditor* data);

class FileOperation {
public:
    virtual std::string name() { return _name; }
    virtual std::string extension() { return _extension; }
protected:
    std::string _name = "Palette type";
    std::string _extension = "";
};

//absolute mess
class FileExporter : public FileOperation {

public:
    static FileExporter* sessionExporter(std::string name, std::string extension, std::function<bool(PlatformNativePathString, MainEditor*)> exportFunction, int formatflags = FORMAT_RGB, std::function<bool(MainEditor*)> canExport = NULL) {
        FileExporter* ret = new FileExporter();
        ret->_name = name;
        ret->_extension = extension;
        ret->_formatFlags = formatflags;
        ret->_isSessionExporter = true;
        ret->_sessionExportFunction = exportFunction;
        ret->_sessionCheckExportFunction = canExport;
        return ret;
    }
    static FileExporter* flatExporter(std::string name, std::string extension, std::function<bool(PlatformNativePathString, Layer*)> exportFunction, int formatflags = FORMAT_RGB, std::function<bool(Layer*)> canExport = NULL) {
        FileExporter* ret = new FileExporter();
        ret->_name = name;
        ret->_extension = extension;
        ret->_formatFlags = formatflags;
        ret->_isSessionExporter = false;
        ret->_flatExportFunction = exportFunction;
        ret->_flatCheckExportFunction = canExport;
        return ret;
    }

    virtual int formatFlags() { return _formatFlags; }
    virtual bool exportsWholeSession() { return _isSessionExporter; }
    virtual bool canExport(void* data) { 
        if (exportsWholeSession()) {
            return _sessionCheckExportFunction != NULL ? _sessionCheckExportFunction((MainEditor*)data) : true;
        }
        else {
            return _flatCheckExportFunction != NULL ? _flatCheckExportFunction((Layer*)data) : true;
        }
    }
    virtual bool exportData(PlatformNativePathString path, void* data) {
        try {
            if (exportsWholeSession()) {
                return _sessionExportFunction(path, (MainEditor*)data);
            }
            else {
                return _flatExportFunction(path, (Layer*)data);
            }
        }
        catch (std::exception& e) {
            logerr(frmt("Data export failed:\n {}", e.what()));
            return false;
        }
    }
protected:
    int _formatFlags = FORMAT_RGB;
    bool _isSessionExporter = false;

    std::function<bool(PlatformNativePathString, MainEditor*)> _sessionExportFunction = NULL;
    std::function<bool(MainEditor*)> _sessionCheckExportFunction = NULL;

    std::function<bool(PlatformNativePathString, Layer*)> _flatExportFunction = NULL;
    std::function<bool(Layer*)> _flatCheckExportFunction = NULL;
};
class FileImporter : public FileOperation {

public:
    static FileImporter* sessionImporter(std::string name, std::string extension, std::function<MainEditor*(PlatformNativePathString)> importFunction, FileExporter* reverse = NULL, int formatflags = FORMAT_RGB, std::function<bool(PlatformNativePathString)> canImport = NULL) {
        FileImporter* ret = new FileImporter();
        ret->_name = name;
        ret->_extension = extension;
        ret->_formatFlags = formatflags;
        ret->_isSessionImporter = true;
        ret->_correspondingExporter = reverse;
        ret->_sessionImportFunction = importFunction;
        ret->_checkImportFunction = canImport;
        return ret;
    }
    static FileImporter* flatImporter(std::string name, std::string extension, std::function<Layer*(PlatformNativePathString, u64)> importFunction, FileExporter* reverse = NULL, int formatflags = FORMAT_RGB, std::function<bool(PlatformNativePathString)> canImport = NULL) {
        FileImporter* ret = new FileImporter();
        ret->_name = name;
        ret->_extension = extension;
        ret->_formatFlags = formatflags;
        ret->_isSessionImporter = false;
        ret->_correspondingExporter = reverse;
        ret->_flatImportFunction = importFunction;
        ret->_checkImportFunction = canImport;
        return ret;
    }

    virtual int formatFlags() { return _formatFlags; }
    virtual bool importsWholeSession() { return _isSessionImporter; }
    virtual FileExporter* getCorrespondingExporter() { return _correspondingExporter; }
    bool hasCheckFunction() { return _checkImportFunction != NULL; }
    virtual bool canImport(PlatformNativePathString path) {
        return _checkImportFunction != NULL ? _checkImportFunction(path) : true;
    }
    virtual void* importData(PlatformNativePathString path) {
        try {
            if (importsWholeSession()) {
                return _sessionImportFunction(path);
            }
            else {
                return _flatImportFunction(path, 0);
            }
        }
        catch (std::exception e) {
            return NULL;
        }
    }
protected:
    int _formatFlags = FORMAT_RGB;
    bool _isSessionImporter = false;
    FileExporter* _correspondingExporter = NULL;

    std::function<bool(PlatformNativePathString)> _checkImportFunction = NULL;

    std::function<MainEditor*(PlatformNativePathString)> _sessionImportFunction = NULL;
    std::function<Layer*(PlatformNativePathString, u64)> _flatImportFunction = NULL;
};

class PaletteImporter : public FileOperation {
public:
    static PaletteImporter* paletteImporter(std::string name, std::string extension, 
        std::function<std::pair<bool, std::vector<uint32_t>>(PlatformNativePathString)> importFunction,
        std::function<bool(PlatformNativePathString)> canImport = NULL, PaletteExporter* reverse = NULL) {

        PaletteImporter* ret = new PaletteImporter();
        ret->_name = name;
        ret->_extension = extension;
        ret->_importFunction = importFunction;
        ret->_canImport = canImport;
        ret->_correspondingExporter = reverse;
        return ret;
    }
    
    virtual PaletteExporter* getCorrespondingExporter() { return _correspondingExporter; }

    virtual bool canImport(PlatformNativePathString path) {
        return _canImport != NULL ? _canImport(path) : true;
    }
    virtual std::pair<bool, std::vector<uint32_t>> importPalette(PlatformNativePathString path) {
        return _importFunction(path);
    };
protected:
    std::function<std::pair<bool, std::vector<uint32_t>>(PlatformNativePathString)> _importFunction = NULL;
    std::function<bool(PlatformNativePathString)> _canImport = NULL;
    PaletteExporter* _correspondingExporter = NULL;
};

class PaletteExporter : public FileOperation {

public:
    static PaletteExporter* paletteExporter(std::string name, std::string extension, std::function<bool(PlatformNativePathString, std::vector<u32>)> exportFunction) {
        PaletteExporter* ret = new PaletteExporter();
        ret->_name = name;
        ret->_extension = extension;
        ret->_exportFunction = exportFunction;
        return ret;
    }

    virtual bool canExport(void* data) {
        return true;
    }
    // pass pointer to std::vector<u32>
    virtual bool exportData(PlatformNativePathString path, void* data) {
        try {
            std::vector<u32> d = *(std::vector<u32>*)data;
            return _exportFunction(path, d);
        }
        catch (std::exception& e) {
            logerr(frmt("Data export failed:\n {}", e.what()));
            return false;
        }
    }
protected:
    std::function<bool(PlatformNativePathString, std::vector<u32>)> _exportFunction = NULL;
};

inline std::vector<FileExporter*> g_fileExporters;
inline std::vector<FileExporter*> g_palettizedFileExporters;

inline std::vector<FileImporter*> g_fileImporters;

inline std::vector<FileImporter*> g_pluginRegisteredFileImporters;
inline std::vector<FileExporter*> g_pluginRegisteredFileExporters;

inline std::vector<PaletteImporter*> g_paletteImporters;
inline std::vector<PaletteExporter*> g_paletteExporters;

//used for autosave
inline FileExporter* voidsnExporter = NULL;

inline void g_setupIO() {
    FileExporter
        *exVOIDSNv6,
        *exVOIDSNv5,
        *exVOIDSNv4,
        *exVOIDSNv3,
        *exVOIDSNv2,
        *exPixelStudioPSP,
        *exPixelStudioPSX,
        *exPiskel,
        *exLPE,
        *exAsepriteASE,
        *exORA,
        *exPNG,
        *exBMP,
        *exCaveStoryPBM,
        *exXYZ,
        *exAnymapPBM,
        *exAnymapPGM,
        *exAnymapPPM,
        *exXBM,
        *exSR8,
        *exVTF,
        *exDIBv5,
        *exJPEG,
        *exAVIF
        ;

    g_fileExporters.push_back( exVOIDSNv6 = FileExporter::sessionExporter("voidsprite Session", ".voidsn", &writeVOIDSNv6, FORMAT_RGB | FORMAT_PALETTIZED) );
    g_fileExporters.push_back( exVOIDSNv5 = FileExporter::sessionExporter("voidsprite Session version 5", ".voidsnv5", &writeVOIDSNv5, FORMAT_RGB | FORMAT_PALETTIZED) );
    g_fileExporters.push_back( exVOIDSNv4 = FileExporter::sessionExporter("voidsprite Session version 4", ".voidsnv4", &writeVOIDSNv4, FORMAT_RGB | FORMAT_PALETTIZED) );
    g_fileExporters.push_back( exVOIDSNv3 = FileExporter::sessionExporter("voidsprite Session version 3", ".voidsnv3", &writeVOIDSNv3) );
    g_fileExporters.push_back( exVOIDSNv2 = FileExporter::sessionExporter("voidsprite Session version 2", ".voidsnv2", &writeVOIDSNv2) );
    g_fileExporters.push_back( exORA = FileExporter::sessionExporter("OpenRaster", ".ora", &writeOpenRaster) );
    g_fileExporters.push_back( exPixelStudioPSP = FileExporter::sessionExporter("Pixel Studio PSP", ".psp", &writePixelStudioPSP) );
    g_fileExporters.push_back( exPixelStudioPSX = FileExporter::sessionExporter("Pixel Studio (compressed) PSX", ".psx", &writePixelStudioPSX) );
    g_fileExporters.push_back( exLPE = FileExporter::sessionExporter("Lospec Pixel Editor", ".lpe", &writeLPE) );
    g_fileExporters.push_back( exPiskel = FileExporter::sessionExporter("Piskel", ".piskel", &writePISKEL) );
    g_fileExporters.push_back( exAsepriteASE = FileExporter::sessionExporter("Aseprite Sprite", ".aseprite", &writeAsepriteASE, FORMAT_RGB | FORMAT_PALETTIZED) );

    g_fileExporters.push_back( exPNG = FileExporter::flatExporter("PNG", ".png", &writePNG, FORMAT_RGB | FORMAT_PALETTIZED) );

    g_fileExporters.push_back( exXYZ = FileExporter::flatExporter("RPG2000/2003 XYZ", ".xyz", &writeXYZ, FORMAT_RGB | FORMAT_PALETTIZED) );
    g_fileExporters.push_back( exBMP = FileExporter::flatExporter("BMP", ".bmp", &writeBMP) );
#if VOIDSPRITE_JXL_ENABLED
    FileExporter* exJXL;
    g_fileExporters.push_back(exJXL = FileExporter::flatExporter("JPEG XL", ".jxl", &writeJpegXL, FORMAT_RGB));
#endif
    g_fileExporters.push_back( exJPEG = FileExporter::flatExporter("JPEG", ".jpeg", &writeJPEG));
    g_fileExporters.push_back( exAVIF = FileExporter::flatExporter("AVIF", ".avif", &writeAVIF));
    g_fileExporters.push_back(FileExporter::flatExporter("TGA", ".tga", &writeTGA));
    g_fileExporters.push_back( exCaveStoryPBM = FileExporter::flatExporter("CaveStory PBM", ".pbm", &writeCaveStoryPBM) );
    g_fileExporters.push_back( exAnymapPBM = FileExporter::flatExporter("Portable Bitmap (text) PBM", ".pbm", &writeAnymapTextPBM, FORMAT_RGB | FORMAT_PALETTIZED) );
    g_fileExporters.push_back( exAnymapPGM = FileExporter::flatExporter("Portable Graymap (text) PGM", ".pgm", &writeAnymapTextPGM, FORMAT_RGB | FORMAT_PALETTIZED) );
    g_fileExporters.push_back( exAnymapPPM = FileExporter::flatExporter("Portable Pixmap (text) PPM", ".ppm", &writeAnymapTextPPM, FORMAT_RGB) );
    g_fileExporters.push_back( exXBM = FileExporter::flatExporter("X Bitmap", ".xbm", &writeXBM, FORMAT_RGB | FORMAT_PALETTIZED) );
    g_fileExporters.push_back( exVTF = FileExporter::flatExporter("VTF", ".vtf", &writeVTF, FORMAT_RGB) );
    g_fileExporters.push_back( exSR8 = FileExporter::flatExporter("Slim Render (8-bit)", ".sr8", &writeSR8, FORMAT_PALETTIZED) );
    g_fileExporters.push_back( exDIBv5 = FileExporter::flatExporter("DIBv5 Clipboard Dump", ".dibv5", &writeDIBV5, FORMAT_RGB) );
    g_fileExporters.push_back(FileExporter::flatExporter("Windows Cursor", ".cur", &writeCUR, FORMAT_RGB | FORMAT_PALETTIZED));
    g_fileExporters.push_back(FileExporter::flatExporter("C Header", ".h", &writeCHeader, FORMAT_RGB | FORMAT_PALETTIZED));
    g_fileExporters.push_back(FileExporter::flatExporter("Python NumPy array", ".py", &writePythonNPArray));
    g_fileExporters.push_back(FileExporter::flatExporter("HTML Base64 image (base64)", ".html", &writeHTMLBase64));
    g_fileExporters.push_back(FileExporter::flatExporter("Java Buffered Image", ".java", &writeJavaBufferedImage));

    for (FileExporter* i : g_pluginRegisteredFileExporters) {
        g_fileExporters.push_back(i);
    }


    voidsnExporter = exVOIDSNv6;

    g_fileImporters.push_back(FileImporter::sessionImporter("voidsprite Session", ".voidsn", &readVOIDSN, exVOIDSNv6, FORMAT_RGB | FORMAT_PALETTIZED,
        [](PlatformNativePathString p) {
            return magicVerify(1, "voidsprite")(p) || magicVerify(9, "/VOIDSN.META/")(p)
                   || magicVerify(0, "\x01")(p) || magicVerify(0, "\x02")(p);
        }));
    g_fileImporters.push_back(FileImporter::sessionImporter("voidsprite Session v6", ".voidsnv6", &readVOIDSN, exVOIDSNv6, FORMAT_RGB | FORMAT_PALETTIZED,
        magicVerify(1, "voidsprite")));
    g_fileImporters.push_back(FileImporter::sessionImporter("voidsprite Session v5", ".voidsnv5", &readVOIDSN, exVOIDSNv5, FORMAT_RGB | FORMAT_PALETTIZED,
        magicVerify(9, "/VOIDSN.META/")));
    g_fileImporters.push_back(FileImporter::sessionImporter("voidsprite Session v4", ".voidsnv4", &readVOIDSN, exVOIDSNv4, FORMAT_RGB | FORMAT_PALETTIZED,
        magicVerify(9, "/VOIDSN.META/")));
    g_fileImporters.push_back(FileImporter::sessionImporter("voidsprite Session v3", ".voidsnv3", &readVOIDSN, exVOIDSNv3, FORMAT_RGB | FORMAT_PALETTIZED,
        magicVerify(9, "/VOIDSN.META/")));
    g_fileImporters.push_back(FileImporter::sessionImporter("voidsprite Session v2", ".voidsnv2", &readVOIDSN, exVOIDSNv2));
    g_fileImporters.push_back(FileImporter::sessionImporter("voidsprite Session v1", ".voidsnv1", &readVOIDSN, exVOIDSNv3));
    g_fileImporters.push_back(FileImporter::sessionImporter("voidsprite Split Session", ".voidspsn", &loadSplitSession, NULL));
    g_fileImporters.push_back(FileImporter::sessionImporter("OpenRaster", ".ora", &readOpenRaster, exORA));
    g_fileImporters.push_back(FileImporter::sessionImporter("Pixel Studio", ".psp", &readPixelStudioPSP, exPixelStudioPSP));
    g_fileImporters.push_back(FileImporter::sessionImporter("Pixel Studio (compressed)", ".psx", &readPixelStudioPSX, exPixelStudioPSX));
    g_fileImporters.push_back(FileImporter::sessionImporter("Lospec Pixel Editor", ".lpe", &readLPE, exLPE));
    g_fileImporters.push_back(FileImporter::sessionImporter("Pix2D", ".pix2d", &readPix2D, NULL));
    g_fileImporters.push_back(FileImporter::sessionImporter("Piskel", ".piskel", &readPISKEL, exPiskel));
    g_fileImporters.push_back(FileImporter::sessionImporter("Pixil", ".pixil", &readPIXIL, NULL));
    g_fileImporters.push_back(FileImporter::sessionImporter("Aseprite Sprite", ".aseprite", &readAsepriteASE, exAsepriteASE, FORMAT_RGB | FORMAT_PALETTIZED,
        magicVerify(4, "\xE0\xA5")));
    g_fileImporters.push_back(FileImporter::sessionImporter("Aseprite Sprite", ".ase", &readAsepriteASE, exAsepriteASE, FORMAT_RGB | FORMAT_PALETTIZED,
        magicVerify(4, "\xE0\xA5")));
#if VSP_USE_LIBLCF
    g_fileImporters.push_back(FileImporter::sessionImporter("RPG Maker 2000/2003 map (load chipset + preview map)", ".lmu", &readLMU));
#endif

    g_fileImporters.push_back(FileImporter::flatImporter("voidsprite 9-segment pattern", ".void9sp", &readVOID9SP, NULL));
    g_fileImporters.push_back(FileImporter::flatImporter("PNG", ".png", &readPNG, exPNG, FORMAT_RGB | FORMAT_PALETTIZED,
        magicVerify(0, "\x89PNG\x0D\x0A")));
    g_fileImporters.push_back(FileImporter::flatImporter("JPEG", ".jpeg", &readSDLImage, exJPEG, FORMAT_RGB, magicVerify(0, "\xFF\xD8")));
    g_fileImporters.push_back(FileImporter::flatImporter("AVIF", ".avif", &readSDLImage, exAVIF));
    g_fileImporters.push_back(FileImporter::flatImporter("BMP", ".bmp", &readBMP, exBMP, FORMAT_RGB, magicVerify(0, "BM")));
    //g_fileImporters.push_back(FileImporter::flatImporter("GIF", ".gif", &readGIF, NULL));
#if VOIDSPRITE_JXL_ENABLED
    g_fileImporters.push_back(FileImporter::flatImporter("JPEG XL", ".jxl", &readJpegXL, exJXL));
#endif
    g_fileImporters.push_back(FileImporter::flatImporter("CaveStory PBM", ".pbm", &readBMP, exCaveStoryPBM, FORMAT_RGB,
        magicVerify(0, "BM")));
    g_fileImporters.push_back(FileImporter::flatImporter("RPG2000/2003 XYZ", ".xyz", &readXYZ, exXYZ, FORMAT_PALETTIZED));
    g_fileImporters.push_back(FileImporter::flatImporter("Atrophy Engine AETEX v1/v2", ".aetex", &readAETEX));
    g_fileImporters.push_back(FileImporter::flatImporter("PS Graphic Image Map GIM", ".gim", &readGIM));
    g_fileImporters.push_back(FileImporter::flatImporter("PS2 Icon ICN", ".icn", &readPS2ICN));
    g_fileImporters.push_back(FileImporter::flatImporter("PS2 Icon ICO", ".ico", &readPS2ICN, NULL, FORMAT_RGB, 
        [](PlatformNativePathString path) {
            FILE* f = platformOpenFile(path, PlatformFileModeRB);
            fseek(f, 12, SEEK_SET);
            u32 num;
            fread(&num, 4, 1, f);
            fclose(f);
            return num == 0x3f800000;
        }));
    g_fileImporters.push_back(FileImporter::flatImporter("Wii/GC TPL", ".tpl", &readWiiGCTPL));
    g_fileImporters.push_back(FileImporter::flatImporter("NES: dump CHR-ROM", ".nes", &readNES));
    g_fileImporters.push_back(FileImporter::flatImporter("DDS", ".dds", &readDDS, NULL, FORMAT_RGB, magicVerify(0, "DDS")));
    g_fileImporters.push_back(FileImporter::flatImporter("VTF", ".vtf", &readVTF, exVTF));
    g_fileImporters.push_back(FileImporter::flatImporter("Valve SPR", ".spr", &readValveSPR, NULL, FORMAT_PALETTIZED,
        magicVerify(0, "IDSP")));
    g_fileImporters.push_back(FileImporter::flatImporter("MSP", ".msp", &readMSP));
    g_fileImporters.push_back(FileImporter::flatImporter("X Bitmap", ".xbm", &readXBM, exXBM, FORMAT_PALETTIZED));
    g_fileImporters.push_back(FileImporter::flatImporter("Slim Render (8-bit) SR8", ".sr8", &readSR8, exSR8, FORMAT_PALETTIZED));
    g_fileImporters.push_back(FileImporter::flatImporter("Windows Shell Scrap SHS", ".shs", &readWinSHS, NULL, FORMAT_RGB));
    g_fileImporters.push_back(FileImporter::flatImporter("DIBv5 Clipboard Dump", ".dibv5", &readDIBV5, exDIBv5, FORMAT_RGB));
    g_fileImporters.push_back(FileImporter::flatImporter("Portable bitmap PBM", ".pbm", &readAnymapPBM, exAnymapPBM, FORMAT_PALETTIZED,
        [](PlatformNativePathString path) {
            FILE* f = platformOpenFile(path, PlatformFileModeRB);
            //check if file starts with P1 or P4
            char c[2];
            fread(c, 2, 1, f);
            fclose(f);
            return c[0] == 'P' && (c[1] == '1' || c[1] == '4');
        }));
    g_fileImporters.push_back(FileImporter::flatImporter("Portable graymap PGM", ".pgm", &readAnymapPGM, exAnymapPGM, FORMAT_PALETTIZED,
        [](PlatformNativePathString path) {
            FILE* f = platformOpenFile(path, PlatformFileModeRB);
            //check if file starts with P2 or P5
            char c[2];
            fread(c, 2, 1, f);
            fclose(f);
            return c[0] == 'P' && (c[1] == '2' || c[1] == '5');
        }));
    g_fileImporters.push_back(FileImporter::flatImporter("Portable pixmap PPM", ".ppm", &readAnymapPPM, exAnymapPPM, FORMAT_RGB,
        [](PlatformNativePathString path) {
            FILE* f = platformOpenFile(path, PlatformFileModeRB);
            //check if file starts with P3 or P6
            char c[2];
            fread(c, 2, 1, f);
            fclose(f);
            return c[0] == 'P' && (c[1] == '3' || c[1] == '6');
        }));
    g_fileImporters.push_back(FileImporter::flatImporter("Mario Paint save file SRM", ".srm", &readMarioPaintSRM, NULL, FORMAT_PALETTIZED));
    g_fileImporters.push_back(FileImporter::flatImporter("Nintendo DS rom (dump banner)", ".nds", &readNDSBanner, NULL));
    g_fileImporters.push_back(FileImporter::flatImporter("X-Com SPK file", ".spk", &readXComSPK, NULL, FORMAT_PALETTIZED));
    g_fileImporters.push_back(FileImporter::flatImporter("X-Com BDY file", ".bdy", &readXComBDY, NULL, FORMAT_PALETTIZED));
    g_fileImporters.push_back(FileImporter::flatImporter("X-Com SCR file", ".scr", &readXComSCR, NULL, FORMAT_PALETTIZED,
        [](PlatformNativePathString path) {
            FILE* f = platformOpenFile(path, PlatformFileModeRB);
            //check if file isn't an exe
            char c[2];
            fread(c, 2, 1, f);
            fseek(f, 0, SEEK_END);
            uint64_t len = ftell(f);
            fclose(f);
            return !(c[0] == 'M' && c[1] == 'Z') && len == (320 * 200);
        }));
    g_fileImporters.push_back(FileImporter::flatImporter("PSP/Vita GXT", ".gxt", &readGXT, NULL));
    g_fileImporters.push_back(FileImporter::flatImporter("Nintendo 3DS CXI (dump icon)", ".cxi", &read3DSCXIIcon, NULL));
    for (FileImporter* i : g_pluginRegisteredFileImporters) {
        g_fileImporters.push_back(i);
    }
    g_fileImporters.push_back(FileImporter::flatImporter("SDL_Image", "", &readSDLImage, NULL, FORMAT_RGB, 
        [](PlatformNativePathString p) {return true; }));


    PaletteExporter* exVOIDPLT;
    g_paletteExporters.push_back( exVOIDPLT = PaletteExporter::paletteExporter("voidsprite palette", ".voidplt", &writePltVOIDPLT));

    g_paletteImporters.push_back(PaletteImporter::paletteImporter("voidsprite palette", ".voidplt", &readPltVOIDPLT, NULL, exVOIDPLT));
    g_paletteImporters.push_back(PaletteImporter::paletteImporter("Hex palette", ".hex", &readPltHEX));
    g_paletteImporters.push_back(PaletteImporter::paletteImporter("paint.net palette", ".txt", &readPltPDNTXT, NULL));
    g_paletteImporters.push_back(PaletteImporter::paletteImporter("JASC-PAL palette", ".pal", &readPltJASCPAL, 
        magicVerify(0, "JASC-PAL")));
    g_paletteImporters.push_back(PaletteImporter::paletteImporter("GIMP GPL palette", ".gpl", &readPltGIMPGPL, 
        magicVerify(0, "GIMP Palette")));
    g_paletteImporters.push_back(PaletteImporter::paletteImporter("Pixel Studio palette", ".palette", &readPltPixelStudioPALETTE));



    for (auto& exporter : g_fileExporters) {
        if (exporter->formatFlags() & FORMAT_PALETTIZED) {
            g_palettizedFileExporters.push_back(exporter);
        }
    }
}