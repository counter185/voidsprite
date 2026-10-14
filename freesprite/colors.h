#pragma once

#define HINT_NEXT_LINE "hint:newline"
#define HINT_NEXT_LINE_HERE {HINT_NEXT_LINE,0}
#define PALETTE_DEFAULT "Grayscale"

struct NamedColor {
    std::string name;
    u32 value;
};
struct PaletteRow {
    std::string name = "";
    std::vector<NamedColor> colors;
};
#define PaletteRows std::vector<PaletteRow>

//filled out at runtime in main.cpp
inline std::map<std::string, u32> g_colors;

class IPalette {
protected:
    PaletteRows placeholderColors;
public:
    virtual ~IPalette() = default;

    virtual std::string getName() { return "INVALID PALETTE"; };
    virtual std::string getDescription() { return ""; };
    virtual std::string getExternalUrl() { return ""; };
    virtual PaletteRows& getColorList() {
        logerr("accessed placeholder color list. this must not happen"); 
        return placeholderColors;
    };

    //if false, palette will not be modifiable
    virtual bool canSave() { return false; }
    virtual bool save() { return false; }
    virtual bool shouldStartCollapsed() { return false; }
    virtual void deletePalette() {}
    virtual bool isBuiltIn() { return true; }

    virtual void addColor(std::string name, u32 color, int row = 0) { 
        if (canSave()) {
            getColorList()[row].colors.push_back({name, color});
            save();
        } 
    }
    virtual void removeColor(int index, int row = 0) {
        auto& r = getColorList()[row];
        if (canSave() && r.colors.size() > index) {
            r.colors.erase(r.colors.begin() + index);
            save();
        }
    }
    virtual void moveColor(int index, bool right = true, int row = 0) {
        auto& list = getColorList()[row].colors;
        if (canSave() && ((!right && index > 0) || (right && index < list.size() - 1))) {
            auto v = list[index];
            list.erase(list.begin() + index);
            list.insert(list.begin() + index + (right ? 1 : -1), v);
            save();
        }
    }

    virtual std::vector<NamedColor> toFlatColorList() {
        std::vector<NamedColor> ret;
        for (auto& row : getColorList()) {
            for (auto& c : row.colors) {
                ret.push_back(c);
            }
        }
        return ret;
    }

    virtual std::vector<u32> toFlatColorValues() {
        std::vector<u32> ret;
        for (auto& row : getColorList()) {
            for (auto& c : row.colors) {
                ret.push_back(c.value);
            }
        }
        return ret;
    }
};

class ColorPalette : public IPalette {
public:
    std::string name = "Palette";
    std::string description = "";
    std::string extUrl = "";
    PaletteRows colorMap;

    bool hideByDefault = false;

    ColorPalette() {}

    ColorPalette(std::string name, PaletteRows rows, bool hideByDefault = false) 
        : name(name), colorMap(rows), hideByDefault(hideByDefault) {}

    ColorPalette(std::string name, std::vector<NamedColor> colors, bool hideByDefault = false)
        : ColorPalette(name, PaletteRows{ {"", colors} }, hideByDefault) {}

    ColorPalette(std::string name, std::vector<u32> colors, bool hideByDefault = false)
        : name(name), hideByDefault(hideByDefault) 
    {
        colorMap = { {"",{}} };
        for (int i = 0; i < colors.size(); i++) {
            NamedColor c = NamedColor{ /*frmt("{}:{:03x}", name, i)*/ "", colors[i]};
            colorMap[0].colors.push_back(c);
        }
    }

    std::string getName() override { return name; }
    std::string getDescription() override { return description; }
    std::string getExternalUrl() override { return extUrl; }
    PaletteRows& getColorList() override { return colorMap; }

    bool shouldStartCollapsed() override { return hideByDefault; }
};

class ImportedPalette : public ColorPalette {
public:
    PlatformNativePathString path;
    PaletteExporter* correspondingExporter = NULL;

    ImportedPalette(std::string name, PaletteRows rows)
        : ColorPalette(name, rows, false) {}

    ImportedPalette(std::string name, std::vector<u32> colors)
        : ColorPalette(name, colors, false) {}

    bool canSave() override { return correspondingExporter != NULL && !path.empty(); }
    bool save() override;

    bool isBuiltIn() override { return false; }
};

class GenAtRuntimePalette : public ColorPalette {
public:
    GenAtRuntimePalette(
        std::string name, 
        std::function<PaletteRows()> genFunction,
        bool hideByDefault = false)
    {
        this->name = name;
        this->hideByDefault = hideByDefault;
        colorMap = genFunction();
    }
};

inline std::vector<IPalette*> g_loadedPalettes = {};

void g_generateColorMap();
void g_reloadColorMap();
IPalette* g_paletteByName(std::string name);
void g_downloadAndInstallPaletteFromLospec(std::string url);
void g_promptCreateNewPalette(std::vector<u32> newPalette, std::function<void()> onSuccessCallback);