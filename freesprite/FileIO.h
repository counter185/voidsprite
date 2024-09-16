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
Layer* readTGA(std::string path, uint64_t seek = 0);
Layer* readBMP(PlatformNativePathString path, uint64_t seek = 0);
Layer* readAETEX(PlatformNativePathString path, uint64_t seek = 0);
Layer* readSDLImage(std::string path, uint64_t seek = 0);
Layer* readWiiGCTPL(PlatformNativePathString path, uint64_t seek = 0);
Layer* readNES(PlatformNativePathString path, uint64_t seek = 0);
Layer* readDDS(PlatformNativePathString path, uint64_t seek = 0);
Layer* readVTF(PlatformNativePathString path, uint64_t seek = 0);
Layer* readGCI(PlatformNativePathString path, uint64_t seek = 0);
Layer* readMSP(PlatformNativePathString path, uint64_t seek = 0);
Layer* readMarioPaintSRM(PlatformNativePathString path, uint64_t seek = 0);
Layer* readXComSPK(PlatformNativePathString path, uint64_t seek = 0);
Layer* readXComBDY(PlatformNativePathString path, uint64_t seek = 0);
MainEditor* readOpenRaster(PlatformNativePathString path);
MainEditor* readVOIDSN(PlatformNativePathString path);

MainEditor* loadAnyIntoSession(std::string utf8path);

bool writePNG(PlatformNativePathString path, Layer* data);
bool writeVOIDSNv1(PlatformNativePathString, XY projDimensions, std::vector<Layer*> data);
bool writeVOIDSNv2(PlatformNativePathString path, MainEditor* editor);
bool writeVOIDSNv3(PlatformNativePathString path, MainEditor* editor);
bool writeOpenRaster(PlatformNativePathString path, MainEditor* data);
bool writeXYZ(PlatformNativePathString path, Layer* data);
bool writeBMP(PlatformNativePathString path, Layer* data);
bool writeTGA(PlatformNativePathString path, Layer* data);
bool writeCaveStoryPBM(PlatformNativePathString path, Layer* data);
bool writeCHeader(PlatformNativePathString path, Layer* data);
bool writePythonNPArray(PlatformNativePathString path, Layer* data);
bool writeHTMLBase64(PlatformNativePathString path, Layer* data);
bool writeJavaBufferedImage(PlatformNativePathString path, Layer* data);

struct FileSessionImportNPath {
	std::string name;
	std::string extension;
	MainEditor* (*importFunction)(PlatformNativePathString);
	bool (*canImport)(PlatformNativePathString) =
		[](PlatformNativePathString p) {
			return true;
		};
};
struct FileImportNPath {
	std::string name;
	std::string extension;
	Layer* (*importFunction)(PlatformNativePathString, uint64_t);
	bool (*canImport)(PlatformNativePathString) =
		[](PlatformNativePathString p) {
			return true;
		};
};
struct FileImportUTF8Path {
	std::string name;
	std::string extension;
	Layer* (*importFunction)(std::string, uint64_t);
	bool (*canImport)(std::string) =
		[](std::string p) {
			return true;
		};
};

struct FileExportMultiLayerNPath {
	std::string name;
	std::string extension;
	bool (*exportFunction)(PlatformNativePathString, MainEditor*);
	int exportFormats = FORMAT_RGB;
	bool (*canExport)(Layer*) =
		[](Layer* p) {
		return true;
		};
};
struct FileExportFlatNPath {
	std::string name;
	std::string extension;
	bool (*exportFunction)(PlatformNativePathString, Layer*);
	int exportFormats = FORMAT_RGB;
	bool (*canExport)(Layer*) =
		[](Layer* p) {
		return true;
		};
};

class FileExporter {

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
	virtual std::string name() { return _name; }
	virtual std::string extension() { return _extension; }
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
	std::string _name = "File type (library)";
	std::string _extension = "";
	int _formatFlags = FORMAT_RGB;
	bool _isSessionExporter = false;

	bool (*_sessionExportFunction)(PlatformNativePathString, MainEditor*) = NULL;
	bool (*_sessionCheckExportFunction)(MainEditor*) = NULL;

	bool (*_flatExportFunction)(PlatformNativePathString, Layer*) = NULL;
	bool (*_flatCheckExportFunction)(Layer*) = NULL;
};

inline std::vector<FileExporter*> g_fileExporters;
inline std::vector<FileExporter*> g_palettizedFileExporters;

inline void g_setupIO() {
	g_fileExporters.push_back(FileExporter::sessionExporter("voidsprite Session version 3", ".voidsn", &writeVOIDSNv3));
	g_fileExporters.push_back(FileExporter::sessionExporter("voidsprite Session version 2", ".voidsnv2", &writeVOIDSNv2));
	g_fileExporters.push_back(FileExporter::sessionExporter("OpenRaster", ".ora", &writeOpenRaster));

	g_fileExporters.push_back(FileExporter::flatExporter("PNG (libpng)", ".png", &writePNG, FORMAT_RGB | FORMAT_PALETTIZED));
	g_fileExporters.push_back(FileExporter::flatExporter("RPG2000/2003 XYZ (voidsprite custom)", ".xyz", &writeXYZ, FORMAT_RGB | FORMAT_PALETTIZED));
	g_fileExporters.push_back(FileExporter::flatExporter("BMP (EasyBMP)", ".bmp", &writeBMP));
	g_fileExporters.push_back(FileExporter::flatExporter("TGA (voidsprite custom)", ".tga", &writeTGA));
	g_fileExporters.push_back(FileExporter::flatExporter("CaveStory PBM (EasyBMP)", ".pbm", &writeCaveStoryPBM));
	g_fileExporters.push_back(FileExporter::flatExporter("C Header (voidsprite custom)", ".h", &writeCHeader));
	g_fileExporters.push_back(FileExporter::flatExporter("Python NumPy array (voidsprite custom)", ".py", &writePythonNPArray));
	g_fileExporters.push_back(FileExporter::flatExporter("HTML Base64 image (base64)", ".html", &writeHTMLBase64));
	g_fileExporters.push_back(FileExporter::flatExporter("Java Buffered Image (voidsprite custom)", ".java", &writeJavaBufferedImage));

	for (auto& exporter : g_fileExporters) {
		if (exporter->formatFlags() & FORMAT_PALETTIZED) {
			g_palettizedFileExporters.push_back(exporter);
		}
	}
}

//todo: do the same to importers at some point
inline std::vector<FileSessionImportNPath> g_fileSessionImportersNPaths = {
	{
		"voidsprite Session", ".voidsn", &readVOIDSN
	},
	{
		"voidsprite Session v3", ".voidsnv3", &readVOIDSN
	},
	{
		"voidsprite Session v2", ".voidsnv2", &readVOIDSN
	},
	{
		"voidsprite Session v1", ".voidsnv1", &readVOIDSN
	},
	{
		"OpenRaster", ".ora", &readOpenRaster
	}
};
inline std::vector<FileImportNPath> g_fileImportersNPaths = {
	{
		"PNG (libpng)", ".png", &readPNG
	},	
	{
		"BMP (EasyBMP)", ".bmp", &readBMP
	},	
	{
		"CaveStory PBM (EasyBMP)", ".pbm", &readBMP,
		[](PlatformNativePathString path) {
			FILE* f = platformOpenFile(path, PlatformFileModeRB);
			//check if file starts with BM
			char c[2];
			fread(c, 2, 1, f);
			fclose(f);
			return c[0] == 'B' && c[1] == 'M';
		}
	},
	{
		"RPG2000/2003 XYZ (voidsprite custom)", ".xyz", &readXYZ
	},
	{
		"Atrophy Engine AETEX v1/v2 (SDL_Image:tga, DDS[fallthrough])", ".aetex", &readAETEX
	},
	{
		"Wii/GC TPL (voidsprite custom)", ".tpl", &readWiiGCTPL
	},
	{
		"NES: dump CHR-ROM (voidsprite custom)", ".nes", &readNES
	},
	{
		"DDS (ddspp+s3tc open source+voidsprite custom)", ".dds", &readDDS
	},
	{
		"VTF (voidsprite custom)", ".vtf", &readVTF
	},
	{
		"MSP (voidsprite custom)", ".msp", &readMSP
	},
	{
		"Mario Paint save file (voidsprite custom)", ".srm", &readMarioPaintSRM
	},
	{
		"X-Com SPK file (voidsprite custom)", ".spk", &readXComSPK
	},
	{
		"X-Com BDY file (voidsprite custom)", ".bdy", &readXComBDY
	}
};

inline std::vector<FileImportUTF8Path> g_fileImportersU8Paths = {
	{ 
		"SDL_Image", "", &readSDLImage
	}
};