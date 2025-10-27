#include "io_base.h"
#include "io_palettes.h"

std::pair<bool, std::vector<uint32_t>> readPltJASCPAL(PlatformNativePathString name)
{
    std::ifstream f(std::filesystem::path(name), std::ios::in);
    if (f.is_open()) {
        std::vector<uint32_t> newPalette;
        std::string line;
        std::getline(f, line);
        if (line.substr(0, 8) == "JASC-PAL") {
            f >> line;
            //should be 0100
            int count;
            f >> count;
            for (int x = 0; x < count; x++) {
                int r, g, b;
                f >> r >> g >> b;
                newPalette.push_back((0xff << 24) | (r << 16) | (g << 8) | b);
            }
            f.close();
            return { true, newPalette };
        }
        f.close();
    }
    return { false, {} };
}

std::pair<bool, std::vector<uint32_t>> readPltGIMPGPL(PlatformNativePathString name)
{
    std::ifstream f(std::filesystem::path(name), std::ios::in);
    if (f.is_open()) {
        std::string magic = "";
        std::string name = "";
        int columns = 0;
        int columnNow = 0;
        int lineN = 0;
        bool oldFormat = false;
        std::vector<u32> ret;

        while (!f.eof()) {
            std::string line;
            std::getline(f, line);

            if (line.substr(0, 1) == "#") {
                continue;
            }
            lineN++;
            if (lineN == 1) {
                magic = line;
                if (magic != "GIMP Palette") {
                    g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Invalid GIMP palette file"));
                    f.close();
                    return { false, {} };
                }
            }
            else if (stringStartsWithIgnoreCase(line, "name: ")) {
                name = line.substr(6);
            }
            else if (lineN == 3 && stringStartsWithIgnoreCase(line, "columns: ")) {
                columns = std::stoi(line.substr(9));
            }
            else if (line.size() > 3) {
                int r, g, b;
                std::istringstream iss(line);
                iss >> r >> g >> b;
                ret.push_back(PackRGBAtoARGB(r, g, b, 255));
            }
        }
        f.close();
        return { true, ret };
    }
    return { false, {} };
}

std::pair<bool, std::vector<uint32_t>> readPltHEX(PlatformNativePathString name)
{
    std::ifstream f(std::filesystem::path(name), std::ios::in);
    if (f.is_open()) {
        std::pair<bool, std::vector<uint32_t>> ret;
        while (!f.eof()) {
            std::string line;
            std::getline(f, line);
            if (line.size() == 6 || line.size() == 8) {
                std::string r = line.substr(0, 2);
                std::string g = line.substr(2, 2);
                std::string b = line.substr(4, 2);
                std::string a = line.size() == 8 ? line.substr(6, 2) : "FF";
                int ri = std::stoi(r, 0, 16);
                int gi = std::stoi(g, 0, 16);
                int bi = std::stoi(b, 0, 16);
                int ai = std::stoi(a, 0, 16);
                ret.second.push_back(PackRGBAtoARGB(ri, gi, bi, ai));
            }
        }
        f.close();
        ret.first = ret.second.size() > 0;
        return ret;
    }
    return { false,{} };
}

std::pair<bool, std::vector<uint32_t>> readPltPDNTXT(PlatformNativePathString name)
{
    std::ifstream f(std::filesystem::path(name), std::ios::in);
    if (f.is_open()) {
        std::pair<bool, std::vector<uint32_t>> ret;
        while (!f.eof()) {
            std::string line;
            std::getline(f, line);
            if (line.size() > 0 && line[0] == ';') {
                continue;
            }
            if (line.size() == 6 || line.size() == 8) {
                int idx = 0;
                std::string a = line.size() == 8 ? line.substr(idx++ * 2, 2) : "FF";
                std::string r = line.substr(idx++ * 2, 2);
                std::string g = line.substr(idx++ * 2, 2);
                std::string b = line.substr(idx++ * 2, 2);

                int ri = std::stoi(r, 0, 16);
                int gi = std::stoi(g, 0, 16);
                int bi = std::stoi(b, 0, 16);
                int ai = std::stoi(a, 0, 16);
                ret.second.push_back(PackRGBAtoARGB(ri, gi, bi, ai));
            }
        }
        f.close();
        ret.first = ret.second.size() > 0;
        return ret;
    }
    return { false,{} };
}

bool writePltHEX(PlatformNativePathString path, std::vector<u32> palette)
{
    std::ofstream f(std::filesystem::path(path), std::ios::out);
    if (f.is_open()) {
        for (u32& col : palette) {
            f << frmt("{:06x}\n", col & 0xFFFFFF);
        }
        f.close();
        return true;
    }
    return false;
}
