#include "json/json.hpp"

#include "globals.h"
#include "FileIO.h"
#include "colors.h"
#include "background_operation.h"
#include "Notification.h"
#include "io/io_voidsprite.h"

std::vector<NamedColorPalette> g_baseNamedColorMap = {
    {
        "Base Colors",
        {
            {"r", 0xFFFF0000},
            {"g", 0xFF00FF00},
            {"b", 0xFF0000FF},
            {"c", 0xFF00FFFF},
            {"m", 0xFFFF00FF},
            {"y", 0xFFFFFF00},
            {"k", 0xFF000000},
            {"w", 0xFFFFFFFF}
        }
    },
    {
        "CSS",
        {
            {"aliceblue", 0xFFF0F8FF},
            {"antiquewhite", 0xFFFAEBD7},
            {"aqua", 0xFF00FFFF},
            {"aquamarine", 0xFF7FFFD4},
            {"azure", 0xFFF0FFFF},
            {"beige", 0xFFF5F5DC},
            {"bisque", 0xFFFFE4C4},
            {"black", 0xFF000000},
            {"blanchedalmond", 0xFFFFEBCD},
            {"blue", 0xFF0000FF},
            {"blueviolet", 0xFF8A2BE2},
            {"brown", 0xFFA52A2A},
            {"burlywood", 0xFFDEB887},
            {"cadetblue", 0xFF5F9EA0},
            {"chartreuse", 0xFF7FFF00},
            {"chocolate", 0xFFD2691E},
            {"coral", 0xFFFF7F50},
            {"cornflowerblue", 0xFF6495ED},
            {"cornsilk", 0xFFFFF8DC},
            {"crimson", 0xFFDC143C},
            {"cyan", 0xFF00FFFF},
            {"darkblue", 0xFF00008B},
            {"darkcyan", 0xFF008B8B},
            {"darkgoldenrod", 0xFFB8860B},
            {"darkgray", 0xFFA9A9A9},
            {"darkgrey", 0xFFA9A9A9},
            {"darkgreen", 0xFF006400},
            {"darkkhaki", 0xFFBDB76B},
            {"darkmagenta", 0xFF8B008B},
            {"darkolivegreen", 0xFF556B2F},
            {"darkorange", 0xFFFF8C00},
            {"darkorchid", 0xFF9932CC},
            {"darkred", 0xFF8B0000},
            {"darksalmon", 0xFFE9967A},
            {"darkseagreen", 0xFF8FBC8F},
            {"darkslateblue", 0xFF483D8B},
            {"darkslategray", 0xFF2F4F4F},
            {"darkslategrey", 0xFF2F4F4F},
            {"darkturquoise", 0xFF00CED1},
            {"darkviolet", 0xFF9400D3},
            {"deeppink", 0xFFFF1493},
            {"deepskyblue", 0xFF00BFFF},
            {"dimgray", 0xFF696969},
            {"dimgrey", 0xFF696969},
            {"dodgerblue", 0xFF1E90FF},
            {"firebrick", 0xFFB22222},
            {"floralwhite", 0xFFFFFAF0},
            {"forestgreen", 0xFF228B22},
            {"fuchsia", 0xFFFF00FF},
            {"gainsboro", 0xFFDCDCDC},
            {"ghostwhite", 0xFFF8F8FF},
            {"gold", 0xFFFFD700},
            {"goldenrod", 0xFFDAA520},
            {"gray", 0xFF808080},
            {"grey", 0xFF808080},
            {"green", 0xFF008000},
            {"greenyellow", 0xFFADFF2F},
            {"honeydew", 0xFFF0FFF0},
            {"hotpink", 0xFFFF69B4},
            {"indianred", 0xFFCD5C5C},
            {"indigo", 0xFF4B0082},
            {"ivory", 0xFFFFFFF0},
            {"khaki", 0xFFF0E68C},
            {"lavender", 0xFFE6E6FA},
            {"lavenderblush", 0xFFFFF0F5},
            {"lawngreen", 0xFF7CFC00},
            {"lemonchiffon", 0xFFFFFACD},
            {"lightblue", 0xFFADD8E6},
            {"lightcoral", 0xFFF08080},
            {"lightcyan", 0xFFE0FFFF},
            {"lightgoldenrodyellow", 0xFFFAFAD2},
            {"lightgray", 0xFFD3D3D3},
            {"lightgrey", 0xFFD3D3D3},
            {"lightgreen", 0xFF90EE90},
            {"lightpink", 0xFFFFB6C1},
            {"lightsalmon", 0xFFFFA07A},
            {"lightseagreen", 0xFF20B2AA},
            {"lightskyblue", 0xFF87CEFA},
            {"lightslategray", 0xFF778899},
            {"lightslategrey", 0xFF778899},
            {"lightsteelblue", 0xFFB0C4DE},
            {"lightyellow", 0xFFFFFFE0},
            {"lime", 0xFF00FF00},
            {"limegreen", 0xFF32CD32},
            {"linen", 0xFFFAF0E6},
            {"magenta", 0xFFFF00FF},
            {"maroon", 0xFF800000},
            {"mediumaquamarine", 0xFF66CDAA},
            {"mediumblue", 0xFF0000CD},
            {"mediumorchid", 0xFFBA55D3},
            {"mediumpurple", 0xFF9370DB},
            {"mediumseagreen", 0xFF3CB371},
            {"mediumslateblue", 0xFF7B68EE},
            {"mediumspringgreen", 0xFF00FA9A},
            {"mediumturquoise", 0xFF48D1CC},
            {"mediumvioletred", 0xFFC71585},
            {"midnightblue", 0xFF191970},
            {"mintcream", 0xFFF5FFFA},
            {"mistyrose", 0xFFFFE4E1},
            {"moccasin", 0xFFFFE4B5},
            {"navajowhite", 0xFFFFDEAD},
            {"navy", 0xFF000080},
            {"oldlace", 0xFFFDF5E6},
            {"olive", 0xFF808000},
            {"olivedrab", 0xFF6B8E23},
            {"orange", 0xFFFFA500},
            {"orangered", 0xFFFF4500},
            {"orchid", 0xFFDA70D6},
            {"palegoldenrod", 0xFFEEE8AA},
            {"palegreen", 0xFF98FB98},
            {"paleturquoise", 0xFFAFEEEE},
            {"palevioletred", 0xFFDB7093},
            {"papayawhip", 0xFFFFEFD5},
            {"peachpuff", 0xFFFFDAB9},
            {"peru", 0xFFCD853F},
            {"pink", 0xFFFFC0CB},
            {"plum", 0xFFDDA0DD},
            {"powderblue", 0xFFB0E0E6},
            {"purple", 0xFF800080},
            {"rebeccapurple", 0xFF663399},
            {"red", 0xFFFF0000},
            {"rosybrown", 0xFFBC8F8F},
            {"royalblue", 0xFF4169E1},
            {"saddlebrown", 0xFF8B4513},
            {"salmon", 0xFFFA8072},
            {"sandybrown", 0xFFF4A460},
            {"seagreen", 0xFF2E8B57},
            {"seashell", 0xFFFFF5EE},
            {"sienna", 0xFFA0522D},
            {"silver", 0xFFC0C0C0},
            {"skyblue", 0xFF87CEEB},
            {"slateblue", 0xFF6A5ACD},
            {"slategray", 0xFF708090},
            {"slategrey", 0xFF708090},
            {"snow", 0xFFFFFAFA},
            {"springgreen", 0xFF00FF7F},
            {"steelblue", 0xFF4682B4},
            {"tan", 0xFFD2B48C},
            {"teal", 0xFF008080},
            {"thistle", 0xFFD8BFD8},
            {"tomato", 0xFFFF6347},
            {"turquoise", 0xFF40E0D0},
            {"violet", 0xFFEE82EE},
            {"wheat", 0xFFF5DEB3},
            {"white", 0xFFFFFFFF},
            {"whitesmoke", 0xFFF5F5F5},
            {"yellow", 0xFFFFFF00},
            {"yellowgreen", 0xFF9ACD32}
        }
    },
    {
        "NES/Famicom",
        {
            {"nes00", 0xFF757575}, {"nes01", 0xFF271B8F}, {"nes02", 0xFF0000AB}, {"nes03", 0xFF47009F},
            {"nes04", 0xFF8F0077}, {"nes05", 0xFFAB0013}, {"nes06", 0xFFA70000}, {"nes07", 0xFF7F0B00},
            {"nes08", 0xFF432F00}, {"nes09", 0xFF004700}, {"nes0a", 0xFF005100}, {"nes0b", 0xFF003f17},
            {"nes0c", 0xFF1b3f5f},
            HINT_NEXT_LINE_HERE,
            {"nes10", 0xFFbcbcbc}, {"nes11", 0xFF0073ef}, {"nes12", 0xFF233bef}, {"nes13", 0xFF8300f3},
            {"nes14", 0xFFbf00bf}, {"nes15", 0xFFe7005b}, {"nes16", 0xFFdb2b00}, {"nes17", 0xFFcb4f0f},
            {"nes18", 0xFF8b7300}, {"nes19", 0xFF009700}, {"nes1a", 0xFF00ab00}, {"nes1b", 0xFF00933b},
            {"nes1c", 0xFF00838b},
            HINT_NEXT_LINE_HERE,
            {"nes20", 0xFFffffff}, {"nes21", 0xFF3fbfff}, {"nes22", 0xFF5f97ff}, {"nes23", 0xFFa78bfd},
            {"nes24", 0xFFf77bff}, {"nes25", 0xFFff77b7}, {"nes26", 0xFFff7763}, {"nes27", 0xFFff9b3b},
            {"nes28", 0xFFf3bf3f}, {"nes29", 0xFF83d313}, {"nes2a", 0xFF4fdf4b}, {"nes2b", 0xFF58f898},
            {"nes2c", 0xFF00ebdb},
            HINT_NEXT_LINE_HERE,
            {"nes30", 0xFFffffff}, {"nes31", 0xFFabe7ff}, {"nes32", 0xFFc7d7ff}, {"nes33", 0xFFd7cbff},
            {"nes34", 0xFFffc7ff}, {"nes35", 0xFFffc7db}, {"nes36", 0xFFffbfb3}, {"nes37", 0xFFffdbab},
            {"nes38", 0xFFffe7a3}, {"nes39", 0xFFe3ffa3}, {"nes3a", 0xFFabf3bf}, {"nes3b", 0xFFb3ffcf},
            {"nes3c", 0xFF9ffff3},
            HINT_NEXT_LINE_HERE,
            {"nes0d", 0xFF000000}, {"nes0e", 0xFF000000}, {"nes0f", 0xFF000000}, {"nes1d", 0xFF000000},
            {"nes1e", 0xFF000000}, {"nes1f", 0xFF000000}, {"nes2d", 0xFF000000}, {"nes2e", 0xFF000000},
            {"nes2f", 0xFF000000}, {"nes3d", 0xFF000000}, {"nes3e", 0xFF000000}, {"nes3f", 0xFF000000},
        }
    },
    {
        "Game Boy/GB Pocket/GB Light",
        {
            {"dmg0", 0xFF294139}, {"dmg1", 0xFF39594A}, {"dmg2", 0xFF5A7942}, {"dmg3", 0xFF7B8210},
            HINT_NEXT_LINE_HERE,
            {"gbp0", 0xFF181818}, {"gbp1", 0xFF4a5138}, {"gbp2", 0xFF8c926b}, {"gbp3", 0xFFc5caa4},
            HINT_NEXT_LINE_HERE,
            {"gbl0", 0xFF004f3a}, {"gbl1", 0xFF00694a}, {"gbl2", 0xFF009a70}, {"gbl3", 0xFF00b582},
        }
    },
    {
        "Windows / OS/2 16 Color mode",
        {
            {"9x00", 0xFF000000}, {"9x01", 0xFF800000}, {"9x02", 0xFF008000}, {"9x03", 0xFF808000}, 
            {"9x04", 0xFF000080}, {"9x05", 0xFF800080}, {"9x06", 0xFF008080}, {"9x07", 0xFFC0C0C0},
            HINT_NEXT_LINE_HERE,
            {"9x08", 0xFF808080}, {"9x09", 0xFFFF0000}, {"9x0a", 0xFF00FF00}, {"9x0b", 0xFFFFFF00}, 
            {"9x0c", 0xFF0000FF}, {"9x0d", 0xFFFF00FF}, {"9x0e", 0xFF00FFFF}, {"9x0f", 0xFFFFFFFF}
        }
    },
    {
        "Color Graphics Adapter [CGA]",
        {
            {"cga0", 0xFF000000}, {"cga1", 0xFF0000AA}, {"cga2", 0xFF00AA00}, {"cga3", 0xFF00AAAA}, 
            {"cga4", 0xFFAA0000}, {"cga5", 0xFFAA00AA}, {"cga6", 0xFFAA5500}, {"cga7", 0xFFAAAAAA},
            HINT_NEXT_LINE_HERE,
            {"cga8", 0xFF555555}, {"cga9", 0xFF5555FF}, {"cga10", 0xFF55FF55}, {"cga11", 0xFF55FFFF}, 
            {"cga12", 0xFFFF5555}, {"cga13", 0xFFFF55FF}, {"cga14", 0xFFFFFF55}, {"cga15", 0xFFFFFFFF}
        }
    },
    {
        "Enhanced Graphics Adapter [EGA]",
        {
            {"ega00", 0xFF000000}, {"ega01", 0xFF0000AA}, {"ega02", 0xFF00AA00}, {"ega03", 0xFF00AAAA},
            {"ega04", 0xFFAA0000}, {"ega05", 0xFFAA00AA}, {"ega06", 0xFFAAAA00}, {"ega07", 0xFFAAAAAA},
            HINT_NEXT_LINE_HERE,
            {"ega08", 0xFF000055}, {"ega09", 0xFF0000FF}, {"ega0a", 0xFF00AA55}, {"ega0b", 0xFF00AAFF},
            {"ega0c", 0xFFAA0055}, {"ega0d", 0xFFAA00FF}, {"ega0e", 0xFFAAAA55}, {"ega0f", 0xFFAAAAFF},
            HINT_NEXT_LINE_HERE,
            {"ega10", 0xFF005500}, {"ega11", 0xFF0055AA}, {"ega12", 0xFF00FF00}, {"ega13", 0xFF00FFAA},
            {"ega14", 0xFFAA5500}, {"ega15", 0xFFAA55AA}, {"ega16", 0xFFAAFF00}, {"ega17", 0xFFAAFFAA},
            HINT_NEXT_LINE_HERE,
            {"ega18", 0xFF005555}, {"ega19", 0xFF0055FF}, {"ega1a", 0xFF00FF55}, {"ega1b", 0xFF00FFFF},
            {"ega1c", 0xFFAA5555}, {"ega1d", 0xFFAA55FF}, {"ega1e", 0xFFAAFF55}, {"ega1f", 0xFFAAFFFF},
            HINT_NEXT_LINE_HERE,
            {"ega20", 0xFF550000}, {"ega21", 0xFF5500AA}, {"ega22", 0xFF55AA00}, {"ega23", 0xFF55AAAA},
            {"ega24", 0xFFFF0000}, {"ega25", 0xFFFF00AA}, {"ega26", 0xFFFFAA00}, {"ega27", 0xFFFFAAAA},
            HINT_NEXT_LINE_HERE,
            {"ega28", 0xFF550055}, {"ega29", 0xFF5500FF}, {"ega2a", 0xFF55AA55}, {"ega2b", 0xFF55AAFF},
            {"ega2c", 0xFFFF0055}, {"ega2d", 0xFFFF00FF}, {"ega2e", 0xFFFFAA55}, {"ega2f", 0xFFFFAAFF},
            HINT_NEXT_LINE_HERE,
            {"ega30", 0xFF555500}, {"ega31", 0xFF5555AA}, {"ega32", 0xFF55FF00}, {"ega33", 0xFF55FFAA},
            {"ega34", 0xFFFF5500}, {"ega35", 0xFFFF55AA}, {"ega36", 0xFFFFFF00}, {"ega37", 0xFFFFFFAA},
            HINT_NEXT_LINE_HERE,
            {"ega38", 0xFF555555}, {"ega39", 0xFF5555FF}, {"ega3a", 0xFF55FF55}, {"ega3b", 0xFF55FFFF},
            {"ega3c", 0xFFFF5555}, {"ega3d", 0xFFFF55FF}, {"ega3e", 0xFFFFFF55}, {"ega3f", 0xFFFFFFFF}
        }
    },
};

void g_generateColorMap() {
    g_colors.clear();
    for (NamedColorPalette& palette : g_namedColorMap) {
        for (auto& color : palette.colorMap) {
            if (color.first != HINT_NEXT_LINE) {
                g_colors[color.first] = color.second;
            }
        }
    }
}

void g_reloadColorMap() {
    g_namedColorMap.clear();
    g_namedColorMap.insert(g_namedColorMap.end(), g_baseNamedColorMap.begin(), g_baseNamedColorMap.end());

    for (auto& palImport : g_paletteImporters) {
        auto fileList = platformListFilesInDir(platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("palettes"), palImport->extension());
        for (auto& file : fileList) {
            std::string fileName = fileNameFromPath(convertStringToUTF8OnWin32(file));
            if (palImport->canImport(file)) {
                auto result = palImport->importPalette(file);
                if (result.first) {
                    auto colorList = result.second;
                    NamedColorPalette ncp;
                    ncp.name = fileName;
                    int i = 0;
                    for (auto& color : colorList) {
                        ncp.colorMap.push_back({ std::format("{}:{:02x}", fileName, i++), color });
                    }
                    g_namedColorMap.push_back(ncp);
                }
                else {
                    logerr(std::format("Failed to load palette: {}", fileName));
                }
            }
        }
    }

    g_generateColorMap();
}

void g_downloadAndInstallPaletteFromLospec(std::string url)
{
    if (stringStartsWithIgnoreCase(url, "lospec-palette://")) {
        url = url.substr(17);

        //windows for some reason
        while (stringEndsWithIgnoreCase(url, "/")) { url.pop_back(); }

        g_startNewAsyncOperation([url]() {
            std::string dlUrl = std::format("https://lospec.com/palette-list/{}.json", url);
            auto outputFile = platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32(std::format("palettes/{}.voidplt", url));
            try {
                std::string response = platformFetchTextFile(dlUrl);
                nlohmann::json j = nlohmann::json::parse(response);
                std::string name = j["name"].get<std::string>();
                std::vector<std::string> colors;
                for (auto& color : j["colors"]) {
                    colors.push_back(color.get<std::string>());
                }
                std::vector<u32> colorsU32;
                std::transform(colors.begin(), colors.end(), std::back_inserter(colorsU32), [](const std::string& color) {
                    return 0xFF000000 | std::stoul(color, nullptr, 16);
                });
                if (writePltVOIDPLT(outputFile, colorsU32)) {
                    g_startNewMainThreadOperation([]() {
                        g_addNotification(SuccessNotification(TL("vsp.success.lospecpaletteinstalled"), ""));
                        g_reloadColorMap();
                    });
                }
                else {
                    throw std::runtime_error("Failed to write palette file");
                }
            }
            catch (std::exception& e) {
                g_startNewMainThreadOperation([e]() {
                    logerr(std::format("Failed to download palette from lospec:\n {}", e.what()));
                    g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.error.lospecdlfail")));
                });
            }
        });
    }
    else {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.error.invalidlospecpaletteurl")));
    }
}
