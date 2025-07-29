#include "io_base.h"
#include "io_pix2d.h"
#include "io_png.h"

#include "../zip/zip.h"
#include "../json/json.hpp"

using namespace nlohmann;

MainEditor* readPix2D(PlatformNativePathString path)
{
	FILE* f = platformOpenFile(path, PlatformFileModeRB);
	if (f != NULL) {
		DoOnReturn closeFile([f]() {fclose(f); });

		zip_t* zip = zip_cstream_open(f, ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
		if (zip == NULL) {
			return NULL;
		}

		json projectJson;

		zip_entry_open(zip, "project.json");
		{
			DoOnReturn closeEntry([zip]() { zip_entry_close(zip); });

			try {
				u8* jsonData = NULL;
				size_t jsonSize;
				zip_entry_read(zip, (void**)&jsonData, &jsonSize);
				std::string jsonString;
				jsonString.resize(jsonSize);
				memcpy(jsonString.data(), jsonData, jsonSize);
				projectJson = json::parse(jsonString);
			}
			catch (std::exception& e) {
				logerr(std::format("[Pix2D] Failed to parse project.json:\n {}", e.what()));
			}
		}

		try {
			std::vector<Layer*> layers;
			auto nodes = projectJson["nodes"][0]["nodes"];
			//all children of `nodes` are layers
			for (auto& layerNode : nodes) {
				
				std::string name = layerNode["name"];
				double opacity = layerNode["opacity"];
				bool hidden = layerNode["isVisible"] == false;

				//all children of `frameNodes` are frames
				auto frameNodes = layerNode["nodes"];
				auto firstFrame = frameNodes[0];
				std::string bitmapFile = firstFrame["bitmap"]["id"];

				if (zip_entry_open(zip, bitmapFile.c_str()) == 0) {
					DoOnReturn closeBitmapEntry([zip]() { zip_entry_close(zip); });
					uint8_t* pngData = NULL;
					size_t pngSize;
					zip_entry_read(zip, (void**)&pngData, &pngSize);

					Layer* nlayer = readPNGFromMem(pngData, pngSize);
					if (nlayer != NULL) {
						nlayer->name = name;
						nlayer->layerAlpha = (u8)(255 * opacity);
						nlayer->hidden = hidden;
						layers.push_back(nlayer);
					}
					else {
						logerr(std::format("[Pix2D] Failed to read layer data:\n {}", bitmapFile));
					}
				}
				else {
					logerr(std::format("[Pix2D] Failed to read layer data:\n {}", bitmapFile));
				}
			}

			if (layers.size() > 0) {
				return new MainEditor(layers);
			}
		}
		catch (std::exception& e) {
			logerr(std::format("[Pix2D] Failed to read project.json:\n {}", e.what()));
			return NULL;
		}

	}
	return NULL;
}
