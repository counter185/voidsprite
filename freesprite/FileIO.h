#pragma once
#include "globals.h"
#include "Layer.h"

Layer* readXYZ(PlatformNativePathString path);
Layer* readPNG(PlatformNativePathString path);
Layer* readTGA(std::string path);
Layer* readBMP(PlatformNativePathString path);
Layer* readAETEX(PlatformNativePathString path);
Layer* readSDLImage(std::string path);
Layer* readWiiGCTPL(PlatformNativePathString path);
Layer* readDDS(PlatformNativePathString path);
MainEditor* readVOIDSN(PlatformNativePathString path);

bool writePNG(PlatformNativePathString path, Layer* data);
bool writeVOIDSNv1(PlatformNativePathString, XY projDimensions, std::vector<Layer*> data);
bool writeXYZ(PlatformNativePathString path, Layer* data);
bool writeBMP(PlatformNativePathString path, Layer* data);
bool writeCaveStoryPBM(PlatformNativePathString path, Layer* data);

struct FileImportNPath {
	std::string name;
	std::string extension;
	Layer* (*importFunction)(PlatformNativePathString);
	bool (*canImport)(PlatformNativePathString) =
		[](PlatformNativePathString p) {
			return true;
		};
};
struct FileImportUTF8Path {
	std::string name;
	std::string extension;
	Layer* (*importFunction)(std::string);
	bool (*canImport)(std::string) =
		[](std::string p) {
			return true;
		};
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
		"Atrophy Engine AETEX v1 (SDL_Image:tga)", ".aetex", &readAETEX
	},
	{
		"Wii/GC TPL (voidsprite custom)", ".tpl", &readWiiGCTPL
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