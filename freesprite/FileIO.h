#pragma once
#include "globals.h"
#include "Layer.h"

#define FORMAT_RGB			0b01
#define FORMAT_PALETTIZED	0b10


uint8_t* DecompressMarioPaintSRM(FILE* f);

void DeXT1(Layer* ret, int width, int height, FILE* infile);
void DeXT23(Layer* ret, int width, int height, FILE* infile);
void DeXT45(Layer* ret, int width, int height, FILE* infile);

int DeASTC(Layer* ret, int width, int height, uint64_t fileLength, FILE* infile, int blockWidth = 8, int blockHeight = 8);

LayerPalettized* De4BPPBitplane(int width, int height, uint8_t* input);

Layer* _VTFseekToLargestMipmapAndRead(FILE* infile, int width, int height, int mipmapCount, int frames, int imageFormat);

//void _parseORAStacksRecursively(MainEditor* editor, pugi::xml_node rootNode, zip_t* zip, XY offset = {0,0});
Layer* readPNGFromMem(uint8_t* data, size_t dataSize);

Layer* readXYZ(PlatformNativePathString path, uint64_t seek = 0);
Layer* readPNG(PlatformNativePathString path, uint64_t seek = 0);
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
Layer* readXBM(PlatformNativePathString path, uint64_t seek = 0);
Layer* readSR8(PlatformNativePathString path, uint64_t seek = 0);
MainEditor* readLMU(PlatformNativePathString path);
MainEditor* readOpenRaster(PlatformNativePathString path);
MainEditor* readVOIDSN(PlatformNativePathString path);

MainEditor* loadAnyIntoSession(std::string utf8path);

bool writePNG(PlatformNativePathString path, Layer* data);
bool writeVOIDSNv1(PlatformNativePathString, XY projDimensions, std::vector<Layer*> data);
bool writeVOIDSNv2(PlatformNativePathString path, MainEditor* editor);
bool writeVOIDSNv3(PlatformNativePathString path, MainEditor* editor);
bool writeVOIDSNv4(PlatformNativePathString path, MainEditor* editor);
bool writeVOIDSNv5(PlatformNativePathString path, MainEditor* editor);
bool writeOpenRaster(PlatformNativePathString path, MainEditor* data);
bool writeXYZ(PlatformNativePathString path, Layer* data);
bool writeBMP(PlatformNativePathString path, Layer* data);
bool writeTGA(PlatformNativePathString path, Layer* data);
bool writeCaveStoryPBM(PlatformNativePathString path, Layer* data);
bool writeXBM(PlatformNativePathString path, Layer* data);
bool writeCHeader(PlatformNativePathString path, Layer* data);
bool writePythonNPArray(PlatformNativePathString path, Layer* data);
bool writeHTMLBase64(PlatformNativePathString path, Layer* data);
bool writeJavaBufferedImage(PlatformNativePathString path, Layer* data);
bool writeAnymapTextPBM(PlatformNativePathString path, Layer* data);
bool writeSR8(PlatformNativePathString path, Layer* data);

std::pair<bool, std::vector<uint32_t>> readPltVOIDPLT(PlatformNativePathString name);
std::pair<bool, std::vector<uint32_t>> readPltJASCPAL(PlatformNativePathString name);

class FileOperation {
public:
    virtual std::string name() { return _name; }
    virtual std::string extension() { return _extension; }
protected:
    std::string _name = "Palette type";
    std::string _extension = "";
};

class FileExporter : public FileOperation {

public:
    static FileExporter* sessionExporter(std::string name, std::string extension, bool (*exportFunction)(PlatformNativePathString, MainEditor*), int formatflags = FORMAT_RGB, bool (*canExport)(MainEditor*) = NULL) {
        FileExporter* ret = new FileExporter();
        ret->_name = name;
        ret->_extension = extension;
        ret->_formatFlags = formatflags;
        ret->_isSessionExporter = true;
        ret->_sessionExportFunction = exportFunction;
        ret->_sessionCheckExportFunction = canExport;
        return ret;
    }
    static FileExporter* flatExporter(std::string name, std::string extension, bool (*exportFunction)(PlatformNativePathString, Layer*), int formatflags = FORMAT_RGB, bool (*canExport)(Layer*) = NULL) {
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
        if (exportsWholeSession()) {
            return _sessionExportFunction(path, (MainEditor*)data);
        }
        else {
            return _flatExportFunction(path, (Layer*)data);
        }
    }
protected:
    int _formatFlags = FORMAT_RGB;
    bool _isSessionExporter = false;

    bool (*_sessionExportFunction)(PlatformNativePathString, MainEditor*) = NULL;
    bool (*_sessionCheckExportFunction)(MainEditor*) = NULL;

    bool (*_flatExportFunction)(PlatformNativePathString, Layer*) = NULL;
    bool (*_flatCheckExportFunction)(Layer*) = NULL;
};
class FileImporter : public FileOperation {

public:
    static FileImporter* sessionImporter(std::string name, std::string extension, MainEditor* (*importFunction)(PlatformNativePathString), FileExporter* reverse = NULL, int formatflags = FORMAT_RGB, bool (*canImport)(PlatformNativePathString) = NULL) {
        FileImporter* ret = new FileImporter();
        ret->_name = name;
        ret->_extension = extension;
        ret->_formatFlags = formatflags;
        ret->_isSessionImporter = true;
        ret->_correspondingExporter = reverse;
        ret->_sessionImportFunction = importFunction;
        ret->_sessionCheckImportFunction = canImport;
        return ret;
    }
    static FileImporter* flatImporter(std::string name, std::string extension, Layer* (*importFunction)(PlatformNativePathString, uint64_t), FileExporter* reverse = NULL, int formatflags = FORMAT_RGB, bool (*canImport)(PlatformNativePathString) = NULL) {
        FileImporter* ret = new FileImporter();
        ret->_name = name;
        ret->_extension = extension;
        ret->_formatFlags = formatflags;
        ret->_isSessionImporter = false;
        ret->_correspondingExporter = reverse;
        ret->_flatImportFunction = importFunction;
        ret->_flatCheckImportFunction = canImport;
        return ret;
    }

    virtual int formatFlags() { return _formatFlags; }
    virtual bool importsWholeSession() { return _isSessionImporter; }
    virtual FileExporter* getCorrespondingExporter() { return _correspondingExporter; }
    virtual bool canImport(PlatformNativePathString path) {
        if (importsWholeSession()) {
            return _sessionCheckImportFunction != NULL ? _sessionCheckImportFunction(path) : true;
        }
        else {
            return _flatCheckImportFunction != NULL ? _flatCheckImportFunction(path) : true;
        }
    }
    virtual void* importData(PlatformNativePathString path) { 
        if (importsWholeSession()) {
            return _sessionImportFunction(path);
        }
        else {
            return _flatImportFunction(path,0);
        }
    }
protected:
    int _formatFlags = FORMAT_RGB;
    bool _isSessionImporter = false;
    FileExporter* _correspondingExporter = NULL;

    MainEditor* (*_sessionImportFunction)(PlatformNativePathString) = NULL;
    bool (*_sessionCheckImportFunction)(PlatformNativePathString) = NULL;

    Layer* (*_flatImportFunction)(PlatformNativePathString, uint64_t) = NULL;
    bool (*_flatCheckImportFunction)(PlatformNativePathString) = NULL;
};
class PaletteImporter : public FileOperation {
public:
    static PaletteImporter* paletteImporter(std::string name, std::string extension, std::pair<bool, std::vector<uint32_t>>(*importFunction)(PlatformNativePathString), bool (*canImport)(PlatformNativePathString) = NULL) {
        PaletteImporter* ret = new PaletteImporter();
        ret->_name = name;
        ret->_extension = extension;
        ret->_importFunction = importFunction;
        ret->_canImport = canImport;
        return ret;
    }
    
    virtual bool canImport(PlatformNativePathString path) {
        return _canImport != NULL ? _canImport(path) : true;
    }
    virtual std::pair<bool, std::vector<uint32_t>> importPalette(PlatformNativePathString path) {
        return _importFunction(path);
    };
protected:
    std::pair<bool, std::vector<uint32_t>> (*_importFunction)(PlatformNativePathString) = NULL;
    bool (*_canImport)(PlatformNativePathString) = NULL;
};

inline std::vector<FileExporter*> g_fileExporters;
inline std::vector<FileExporter*> g_palettizedFileExporters;

inline std::vector<FileImporter*> g_fileImporters;

inline std::vector<PaletteImporter*> g_paletteImporters;

inline void g_setupIO() {
    FileExporter
        *exVOIDSNv5,
        *exVOIDSNv4,
        *exVOIDSNv3,
        *exVOIDSNv2,
        *exORA,
        *exPNG,
        *exBMP,
        *exCaveStoryPBM,
        *exXYZ,
        *exAnymapPBM,
        *exXBM,
        *exSR8
        ;

    g_fileExporters.push_back( exVOIDSNv5 = FileExporter::sessionExporter("voidsprite Session", ".voidsn", &writeVOIDSNv5, FORMAT_RGB | FORMAT_PALETTIZED) );
    g_fileExporters.push_back( exVOIDSNv4 = FileExporter::sessionExporter("voidsprite Session version 4", ".voidsnv4", &writeVOIDSNv4, FORMAT_RGB | FORMAT_PALETTIZED) );
    g_fileExporters.push_back( exVOIDSNv3 = FileExporter::sessionExporter("voidsprite Session version 3", ".voidsnv3", &writeVOIDSNv3) );
    g_fileExporters.push_back( exVOIDSNv2 = FileExporter::sessionExporter("voidsprite Session version 2", ".voidsnv2", &writeVOIDSNv2) );
    g_fileExporters.push_back( exORA = FileExporter::sessionExporter("OpenRaster", ".ora", &writeOpenRaster) );

    g_fileExporters.push_back( exPNG = FileExporter::flatExporter("PNG (libpng)", ".png", &writePNG, FORMAT_RGB | FORMAT_PALETTIZED) );
    g_fileExporters.push_back( exXYZ = FileExporter::flatExporter("RPG2000/2003 XYZ", ".xyz", &writeXYZ, FORMAT_RGB | FORMAT_PALETTIZED) );
    g_fileExporters.push_back( exBMP = FileExporter::flatExporter("BMP (EasyBMP)", ".bmp", &writeBMP) );
    g_fileExporters.push_back(FileExporter::flatExporter("TGA", ".tga", &writeTGA));
    g_fileExporters.push_back( exCaveStoryPBM = FileExporter::flatExporter("CaveStory PBM (EasyBMP)", ".pbm", &writeCaveStoryPBM) );
    g_fileExporters.push_back( exAnymapPBM = FileExporter::flatExporter("Portable Bitmap (text) PBM", ".pbm", &writeAnymapTextPBM, FORMAT_RGB | FORMAT_PALETTIZED) );
    g_fileExporters.push_back( exXBM = FileExporter::flatExporter("X Bitmap", ".xbm", &writeXBM, FORMAT_RGB | FORMAT_PALETTIZED) );
    g_fileExporters.push_back( exSR8 = FileExporter::flatExporter("Slim Render (8-bit)", ".sr8", &writeSR8, FORMAT_PALETTIZED) );
    g_fileExporters.push_back(FileExporter::flatExporter("C Header", ".h", &writeCHeader, FORMAT_RGB | FORMAT_PALETTIZED));
    g_fileExporters.push_back(FileExporter::flatExporter("Python NumPy array", ".py", &writePythonNPArray));
    g_fileExporters.push_back(FileExporter::flatExporter("HTML Base64 image (base64)", ".html", &writeHTMLBase64));
    g_fileExporters.push_back(FileExporter::flatExporter("Java Buffered Image", ".java", &writeJavaBufferedImage));


    g_fileImporters.push_back(FileImporter::sessionImporter("voidsprite Session", ".voidsn", &readVOIDSN, exVOIDSNv5, FORMAT_RGB | FORMAT_PALETTIZED));
    g_fileImporters.push_back(FileImporter::sessionImporter("voidsprite Session v5", ".voidsnv5", &readVOIDSN, exVOIDSNv5, FORMAT_RGB | FORMAT_PALETTIZED));
    g_fileImporters.push_back(FileImporter::sessionImporter("voidsprite Session v4", ".voidsnv4", &readVOIDSN, exVOIDSNv4, FORMAT_RGB | FORMAT_PALETTIZED));
    g_fileImporters.push_back(FileImporter::sessionImporter("voidsprite Session v3", ".voidsnv3", &readVOIDSN, exVOIDSNv3, FORMAT_RGB | FORMAT_PALETTIZED));
    g_fileImporters.push_back(FileImporter::sessionImporter("voidsprite Session v2", ".voidsnv2", &readVOIDSN, exVOIDSNv2));
    g_fileImporters.push_back(FileImporter::sessionImporter("voidsprite Session v1", ".voidsnv1", &readVOIDSN, exVOIDSNv3));
    g_fileImporters.push_back(FileImporter::sessionImporter("OpenRaster", ".ora", &readOpenRaster, exORA));
    g_fileImporters.push_back(FileImporter::sessionImporter("RPG Maker 2000/2003 map (load chipset + preview map)", ".lmu", &readLMU));

    g_fileImporters.push_back(FileImporter::flatImporter("PNG (libpng)", ".png", &readPNG, exPNG));
    g_fileImporters.push_back(FileImporter::flatImporter("BMP (EasyBMP)", ".bmp", &readBMP, exBMP));
    g_fileImporters.push_back(FileImporter::flatImporter("CaveStory PBM (EasyBMP)", ".pbm", &readBMP, exCaveStoryPBM, FORMAT_RGB,
        [](PlatformNativePathString path) {
            FILE* f = platformOpenFile(path, PlatformFileModeRB);
            //check if file starts with BM
            char c[2];
            fread(c, 2, 1, f);
            fclose(f);
            return c[0] == 'B' && c[1] == 'M';
        }));
    g_fileImporters.push_back(FileImporter::flatImporter("RPG2000/2003 XYZ", ".xyz", &readXYZ, exXYZ, FORMAT_PALETTIZED));
    g_fileImporters.push_back(FileImporter::flatImporter("Atrophy Engine AETEX v1/v2 (SDL_Image, DDS)", ".aetex", &readAETEX));
    g_fileImporters.push_back(FileImporter::flatImporter("Wii/GC TPL", ".tpl", &readWiiGCTPL));
    g_fileImporters.push_back(FileImporter::flatImporter("NES: dump CHR-ROM", ".nes", &readNES));
    g_fileImporters.push_back(FileImporter::flatImporter("DDS (ddspp+s3tc open source+voidsprite custom)", ".dds", &readDDS));
    g_fileImporters.push_back(FileImporter::flatImporter("VTF", ".vtf", &readVTF));
    g_fileImporters.push_back(FileImporter::flatImporter("MSP", ".msp", &readMSP));
    g_fileImporters.push_back(FileImporter::flatImporter("X Bitmap", ".xbm", &readXBM, exXBM, FORMAT_PALETTIZED));
    g_fileImporters.push_back(FileImporter::flatImporter("Slim Render (8-bit)", ".sr8", &readSR8, exSR8, FORMAT_RGB));
    g_fileImporters.push_back(FileImporter::flatImporter("Portable anymap PBM", ".pbm", &readAnymapPBM, exAnymapPBM, FORMAT_PALETTIZED,
        [](PlatformNativePathString path) {
            FILE* f = platformOpenFile(path, PlatformFileModeRB);
            //check if file starts with P1 or P4
            char c[2];
            fread(c, 2, 1, f);
            fclose(f);
            return c[0] == 'P' && (c[1] == '1' || c[1] == '4');
        }));
    g_fileImporters.push_back(FileImporter::flatImporter("Mario Paint save file", ".srm", &readMarioPaintSRM, NULL, FORMAT_PALETTIZED));
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
            return !(c[0] == 'M' && c[1] == 'Z') || len == (320 * 200);
        }));
    g_fileImporters.push_back(FileImporter::flatImporter("SDL_Image", "", &readSDLImage));


    g_paletteImporters.push_back(PaletteImporter::paletteImporter("voidsprite palette", ".voidplt", &readPltVOIDPLT));
    g_paletteImporters.push_back(PaletteImporter::paletteImporter("JASC-PAL palette", ".pal", &readPltJASCPAL, 
        [](PlatformNativePathString path) { 
            FILE* f = platformOpenFile(path, PlatformFileModeRB);
            char headerBytes[9];
            memset(headerBytes, 0, 9);
            fread(headerBytes, 8, 1, f);
            fclose(f);
            return std::string(headerBytes) == "JASC-PAL";
        }));



    for (auto& exporter : g_fileExporters) {
        if (exporter->formatFlags() & FORMAT_PALETTIZED) {
            g_palettizedFileExporters.push_back(exporter);
        }
    }
}