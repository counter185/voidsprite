#include "io_base.h"
#include "io_pixil.h"
#include "io_png.h"

#include "../json/json.hpp"
#include "../base64/base64.hpp"

using namespace nlohmann;

MainEditor* readPIXIL(PlatformNativePathString path)
{
    std::ifstream f(path);
    if (f.is_open()) {
        try {
            json j = json::parse(f);
            f.close();

            std::string antiReverseStr1 = "/sfR5H8Fkddasdmnacvx/";
            //* there's also "p98kjasdnasd983/24kasdjasd" but it gets inserted between data:image/png and base64,
            //  which voidsprite completely skips (correct behavior is to replace this string with ; symbol)
            //* there's also "/8745jkhasdASD945kjknhj/" marked in the code as "this.importExportIsPhotoUpload"

            int canvasW = j["width"];
            int canvasH = j["height"];
            std::string name = j["name"];
            auto frameData = j["frames"];
            //todo:animations when we get there
            auto firstFrame = frameData[0];
            int selectedLayer = firstFrame["selectedLayer"];

            std::vector<Layer*> layers;
            for (auto& layer : firstFrame["layers"]) {
                std::string layerName = layer["name"];
                double opacity = 1.0;
                try {
                    double opacity = std::stod(layer["opacity"].get<std::string>());
                }
                catch (std::exception&) {
                    double opacity = layer["opacity"].get<double>();
                }
                std::string base64Data = layer["src"];

                auto findAntiReStr1 = base64Data.find(antiReverseStr1);
                while (findAntiReStr1 != std::string::npos) {
                    base64Data.erase(findAntiReStr1, antiReverseStr1.size());
                    findAntiReStr1 = base64Data.find(antiReverseStr1);
                }

                Layer* l = readPNGFromBase64String(base64Data);
                if (l != NULL) {
                    l->name = layerName;
                    l->layerAlpha = (u8)(opacity * 255);
                    layers.push_back(l);
                }
                else {
                    logerr("Failed to read layer from pixil file");
                }
            }

            return !layers.empty() ? new MainEditor(layers) : NULL;
        }
        catch (std::exception&) {
            return NULL;
        }
    }
    return NULL;
}

bool writePIXIL(PlatformNativePathString path, MainEditor* editor)
{
    std::ofstream outfile(path);

    if (outfile.is_open()) {
        json o = json::object();
        

        outfile << o.dump();
        outfile.close();
        return true;
    }
    return false;
}
