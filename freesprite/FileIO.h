#pragma once
#include "globals.h"
#include "Layer.h"

Layer* readXYZ(PlatformNativePathString path, uint64_t seek = 0);
Layer* readPNG(PlatformNativePathString path, uint64_t seek = 0);
Layer* readTGA(std::string path, uint64_t seek = 0);
Layer* readBMP(PlatformNativePathString path, uint64_t seek = 0);
Layer* readAETEX(PlatformNativePathString path, uint64_t seek = 0);
Layer* readSDLImage(std::string path, uint64_t seek = 0);
Layer* readWiiGCTPL(PlatformNativePathString path, uint64_t seek = 0);
Layer* readNES(PlatformNativePathString path, uint64_t seek = 0);
Layer* readDDS(PlatformNativePathString path, uint64_t seek = 0);
MainEditor* readVOIDSN(PlatformNativePathString path);

bool writePNG(PlatformNativePathString path, Layer* data);
bool writeVOIDSNv1(PlatformNativePathString, XY projDimensions, std::vector<Layer*> data);
bool writeVOIDSNv2(PlatformNativePathString path, MainEditor* editor);
bool writeOpenRaster(PlatformNativePathString path, MainEditor* data);
bool writeXYZ(PlatformNativePathString path, Layer* data);
bool writeBMP(PlatformNativePathString path, Layer* data);
bool writeCaveStoryPBM(PlatformNativePathString path, Layer* data);
bool writeCHeader(PlatformNativePathString path, Layer* data);

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
	bool (*canExport)(Layer*) =
		[](Layer* p) {
		return true;
		};
};
struct FileExportFlatNPath {
	std::string name;
	std::string extension;
	bool (*exportFunction)(PlatformNativePathString, Layer*);
	bool (*canExport)(Layer*) =
		[](Layer* p) {
		return true;
		};
};

inline std::vector<FileExportFlatNPath> g_fileExportersFlatNPaths = {
	{
		"PNG (libpng)", ".png", &writePNG
	},
	{
		"RPG2000/2003 XYZ (voidsprite custom)", ".xyz", &writeXYZ
	},
	{
		"BMP (EasyBMP)", ".bmp", &writeBMP
	},
	{
		"CaveStory PBM (EasyBMP)", ".pbm", &writeCaveStoryPBM
	},
	{
		"C Header (voidsprite custom)", ".h", &writeCHeader
	}
};
inline std::vector<FileExportMultiLayerNPath> g_fileExportersMLNPaths = {
	{
		"voidsprite Session", ".voidsn", &writeVOIDSNv2
	},
	{
		"OpenRaster", ".ora", &writeOpenRaster
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
	}
};

inline std::vector<FileImportUTF8Path> g_fileImportersU8Paths = {
	{ 
		"SDL_Image", "", &readSDLImage
	}
};