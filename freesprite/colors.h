#pragma once

#define HINT_NEXT_LINE "hint:newline"
#define HINT_NEXT_LINE_HERE {HINT_NEXT_LINE,0}

//filled out at runtime in main.cpp
inline std::map<std::string, u32> g_colors;

class IPalette {
protected:
    std::vector<std::pair<std::string, u32>> placeholderColorList;
public:
    virtual ~IPalette() = default;

    virtual std::string getName() { return "INVALID PALETTE"; };
    virtual std::vector<std::pair<std::string, u32>>& getColorList() { 
        logerr("accessed placeholder color list. this must not happen"); 
        return placeholderColorList; 
    };

    //if false, palette will not be modifiable
    virtual bool canSave() { return false; }
    virtual bool save() { return false; }

    virtual void addColor(std::string name, u32 color) { 
        if (canSave()) {
            getColorList().push_back({name, color});
            save();
        } 
    }
    virtual void removeColor(int index) {
        if (canSave() && getColorList().size() > index) {
            getColorList().erase(getColorList().begin() + index);
            save();
        }
    }
    virtual void moveColor(int index, bool right = true) {
        if (canSave() && ((!right && index > 0) || (right && index < getColorList().size() - 1))) {
            auto& list = getColorList();
            auto v = list[index];
            list.erase(list.begin() + index);
            list.insert(list.begin() + index + (right ? 1 : -1), v);
            save();
        }
    }
};

class NamedColorPalette : public IPalette {
public:
    std::string name = "Palette";
    std::vector<std::pair<std::string, u32>> colorMap;

    PlatformNativePathString path;
    PaletteExporter* correspondingExporter = NULL;

    NamedColorPalette() {}
    NamedColorPalette(std::string name, std::vector<std::pair<std::string, u32>> colors) : name(name), colorMap(colors) {}

    std::string getName() override { return name; }
    std::vector<std::pair<std::string, u32>>& getColorList() override { return colorMap; }

    bool canSave() override { return correspondingExporter != NULL && !path.empty(); }
    bool save() override;
};

inline std::vector<NamedColorPalette> g_namedColorMap = {};

void g_generateColorMap();
void g_reloadColorMap();
void g_downloadAndInstallPaletteFromLospec(std::string url);