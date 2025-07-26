#include "io_base.h"
#include "io_lpe.h"
#include "io_png.h"

#include "../json/json.hpp"
#include "../base64/base64.hpp"

using namespace nlohmann;

MainEditor* readLPE(PlatformNativePathString path)
{
    std::ifstream f(path);
    if (f.is_open()) {
        json j = json::parse(f);
        f.close();

        int width = j["canvasWidth"];
        int height = j["canvasHeight"];
        //auto colors = j["colors"].array();
        int selLayer = j["selectedLayer"];

        std::vector<Layer*> layers;
        for (auto& layer : j["layers"]) {
            bool visible = layer["isVisible"];
            std::string name = layer["name"];
            std::string base64image = layer["src"];
            Layer* l = readPNGFromBase64String(base64image);
            if (l != NULL) {
                l->hidden = !visible;
                l->name = name;
                layers.insert(layers.begin(), l);
            }
        }
        if (layers.size() > 0) {
            MainEditor* editor = new MainEditor(layers);
            editor->selLayer = selLayer;
            return editor;
        }

    }
    return NULL;
}

bool writeLPE(PlatformNativePathString path, MainEditor* editor)
{
    return false;
}
