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

std::vector<u8> decompressZlibWithoutUncompressedSize(u8* data, u64 dataSize);
std::vector<u8> compressZlib(u8* data, size_t dataSize);
std::vector<u8> base64ToBytes(std::string b64);
std::vector<u8> decompressGzip(u8* data, size_t dataSize);

void zlibFile(PlatformNativePathString path);
void unZlibFile(PlatformNativePathString path);

std::function<bool(PlatformNativePathString)> magicVerify(u64 at, std::string header);

PlatformNativePathString newTempFile();

#if VSP_PLATFORM == VSP_PLATFORM_EMSCRIPTEN
void emDownloadFile(PlatformNativePathString path);
#endif

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
#include "io/io_valve.h"
#include "io/io_voidsprite.h"
#include "io/io_openraster.h"
#include "io/io_pixelstudio.h"
#include "io/io_iw.h"
#include "io/io_xna.h"
#include "io/io_flipnote.h"
#include "io/io_gif.h"
#include "io/io_avi.h"
#include "io/io_godot.h"
#include "io/io_resprite.h"

#include "io/io_palettes.h"

Layer* readTGA(PlatformNativePathString path, uint64_t seek = 0);
Layer* readBMP(PlatformNativePathString path, uint64_t seek = 0);
Layer* readAETEX(PlatformNativePathString path, uint64_t seek = 0);
Layer* readSDLImage(PlatformNativePathString path, uint64_t seek = 0);
Layer* readWiiGCTPL(PlatformNativePathString path, uint64_t seek = 0);
Layer* readNES(PlatformNativePathString path, uint64_t seek = 0);
Layer* readDDS(PlatformNativePathString path, uint64_t seek = 0);
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
Layer* readGXT(PlatformNativePathString path, u64 seek = 0);
Layer* readWinSHS(PlatformNativePathString path, u64 seek = 0);

Layer* loadAnyIntoFlat(std::string utf8path, FileImporter** outputFoundImporter = NULL, OperationProgressReport* progressReport = NULL);
MainEditor* loadAnyIntoSession(std::string utf8path, FileImporter** outputFoundImporter = NULL, OperationProgressReport* progressReport = NULL);

bool tryInstallPalette(PlatformNativePathString path);

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

std::pair<bool, NineSegmentPattern> read9SegmentPattern(PlatformNativePathString path);
bool write9SegmentPattern(PlatformNativePathString path, Layer* data, XY point1, XY point2);

//SplitSessionData loadSplitSessionData(PlatformNativePathString path);

MainEditor* loadSplitSession(PlatformNativePathString path);
bool saveSplitSession(PlatformNativePathString path, MainEditor* data);

class FileOperation {
public:
    virtual std::string name() { return _name; }
    virtual std::string extension() { return _extension; }
    std::string description = "";
protected:
    std::string _name = "Palette type";
    std::string _extension = "";
};

//absolute mess
class FileExporter : public FileOperation {

public:
    static FileExporter* sessionExporter(std::string name, std::string extension, std::string description, std::function<bool(PlatformNativePathString, MainEditor*)> exportFunction, int formatflags = FORMAT_RGB, std::function<bool(MainEditor*)> canExport = NULL) {
        FileExporter* ret = new FileExporter();
        ret->_name = name;
        ret->_extension = extension;
        ret->description = description;
        ret->_formatFlags = formatflags;
        ret->_isSessionExporter = true;
        ret->_sessionExportFunction = exportFunction;
        ret->_sessionCheckExportFunction = canExport;
        return ret;
    }
    static FileExporter* flatExporter(std::string name, std::string extension, std::string description, std::function<bool(PlatformNativePathString, Layer*)> exportFunction, int formatflags = FORMAT_RGB, std::function<bool(Layer*)> canExport = NULL) {
        FileExporter* ret = new FileExporter();
        ret->_name = name;
        ret->_extension = extension;
        ret->description = description;
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
    static FileImporter* sessionImporter(
            std::string name, 
            std::string extension, 
            std::function<MainEditor*(PlatformNativePathString, OperationProgressReport*)> importFunction, 
            FileExporter* reverse = NULL, 
            int formatflags = FORMAT_RGB, 
            std::function<bool(PlatformNativePathString)> 
            canImport = NULL) {
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
    static FileImporter* sessionImporter(
            std::string name, 
            std::string extension, 
            std::function<MainEditor*(PlatformNativePathString)> importFunction, 
            FileExporter* reverse = NULL, 
            int formatflags = FORMAT_RGB, 
            std::function<bool(PlatformNativePathString)> 
            canImport = NULL) 
    {
        return sessionImporter(name, extension, [importFunction](PlatformNativePathString path, OperationProgressReport* progressReport) {
            progressReport->enterSection("Loading file...");
            return importFunction(path);
        }, reverse, formatflags, canImport);
    }
    static FileImporter* flatImporter(
            std::string name, 
            std::string extension, 
            std::function<Layer*(PlatformNativePathString, u64, OperationProgressReport*)> importFunction, 
            FileExporter* reverse = NULL, 
            int formatflags = FORMAT_RGB, 
            std::function<bool(PlatformNativePathString)> canImport = NULL) {
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
    static FileImporter* flatImporter(
            std::string name, 
            std::string extension, 
            std::function<Layer*(PlatformNativePathString, u64)> importFunction, 
            FileExporter* reverse = NULL, 
            int formatflags = FORMAT_RGB, 
            std::function<bool(PlatformNativePathString)> canImport = NULL) 
    {
        return flatImporter(name, extension, [importFunction](PlatformNativePathString path, u64 seek, OperationProgressReport* progressReport) {
            progressReport->enterSection("Loading file...");
            return importFunction(path, seek);
        }, reverse, formatflags, canImport);
    }

    virtual int formatFlags() { return _formatFlags; }
    virtual bool importsWholeSession() { return _isSessionImporter; }
    virtual FileExporter* getCorrespondingExporter() { return _correspondingExporter; }
    bool hasCheckFunction() { return _checkImportFunction != NULL; }
    virtual bool canImport(PlatformNativePathString path) {
        return _checkImportFunction != NULL ? _checkImportFunction(path) : true;
    }
    virtual void* importData(PlatformNativePathString path, OperationProgressReport* progressReport = NULL) {
        try {
            if (importsWholeSession()) {
                return _sessionImportFunction(path, progressReport);
            }
            else {
                return _flatImportFunction(path, 0, progressReport);
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

    std::function<MainEditor*(PlatformNativePathString, OperationProgressReport*)> _sessionImportFunction = NULL;
    std::function<Layer*(PlatformNativePathString, u64, OperationProgressReport*)> _flatImportFunction = NULL;
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

void g_setupIO();