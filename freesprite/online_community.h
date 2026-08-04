#pragma once
#include "globals.h"
#include "mathops.h"
#include "io/io_png.h"
#include "background_operation.h"

#include "json/json.hpp"

using namespace nlohmann;


struct FeaturedImage {
    Layer* image = NULL;
    std::string title = "";
    std::string artistName = "---";
    std::string artistLinks = "";
    std::string imageUrl = "";
};

inline std::string g_motd = "";
inline bool g_featuredImagesLoaded = false;
inline std::vector<FeaturedImage> g_featured;

inline void g_loadFeaturedImages() {
    try {
        std::string url = "https://counter185.github.io/voidsprite-community/featured.json";
        std::string featuredJson =
            //uncomment to test locally
            /*"{\"motd\": \"welcome to best battlefield server! crossbow = instant ban\","
            "\"images\": [ "
            "{ \"url\": \"https://raw.githubusercontent.com/counter185/voidsprite/ee517fbbcd6e8e36badeac73bc6bf0f376319e83/freesprite/astc_dec/downloadfile-1-1-1-1-1-1-1-2-1-1-1-1-3-1-2-1-1-2-11-2-1-3-1-1-1-4.png\", \"title\": \"kaosekai.png\", \"by\": \"me\", \"links\": \"social links here\\nmore social links here idk\" }, "
            "{ \"url\": \"http://localhost/image.png\", \"title\": \"image\", \"by\": \"cntrpl\" }, "
            "{ \"url\": \"http://localhost/nonexistent.png\", \"title\": \"this one shouldn't exist\", \"by\": \"cntrpl\", \"links\": \"more links idk\" } "
            "]}";*/
            platformFetchTextFile(url);
        json featured = json::parse(featuredJson);

        if (featured.contains("motd")) {
            g_motd = featured["motd"];
        }

        std::vector<FeaturedImage> imgs;

        auto& imagesArray = featured["images"];
        for (auto& f : imagesArray) {
            try {
                FeaturedImage newImg{};
                newImg.imageUrl = f["url"];
                newImg.title = f.contains("title") ? f["title"] : "";
                newImg.artistName = f.contains("by") ? f["by"] : "---";
                newImg.artistLinks = f.contains("links") ? f["links"] : "";
                imgs.push_back(newImg);
            }
            catch (std::exception&) {}
        }

        for (int x = 0; x < 10 && !imgs.empty(); x++) {
            int index = randomInt(0, imgs.size());
            try {
                FeaturedImage thisImg = imgs[index];
                auto data = platformFetchBinFile(thisImg.imageUrl);
                if (data.size() < 8 || memcmp(data.data(), "\x89PNG\xD\xA\x1A\x0A", 8) != 0) {
                    throw std::runtime_error("");
                }
                Layer* l = readPNGFromMem(data.data(), data.size());
                if (l != NULL) {
                    thisImg.image = l;
                    g_featured.push_back(thisImg);
                }
                else {
                    throw std::runtime_error("");
                }
            }
            catch (std::exception&) {
                x--;
            }

            imgs.erase(imgs.begin() + index);
        }

        g_featuredImagesLoaded = true;
    }
    catch (std::exception& e) {
        logerr(frmt("[loadFeaturedImages] error:\n {}", e.what()));
    }
}


inline void g_loadCommunityContent() {
#if VSP_PLATFORM != VSP_PLATFORM_EMSCRIPTEN
    g_startNewAsyncOperation([]() {
        g_loadFeaturedImages();
    });
#endif
}