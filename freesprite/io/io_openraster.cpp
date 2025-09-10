#include "io_base.h"
#include "io_openraster.h"
#include "io_png.h"

#include "../zip/zip.h"
#include "../pugixml/pugixml.hpp"

void _parseORAStacksRecursively(std::vector<Layer*>* layers, XY dimensions, pugi::xml_node rootNode, zip_t* zip, XY offset = { 0,0 }) {
    
    for (pugi::xml_node layerNode : rootNode.children()) {
        std::string nodeName = layerNode.name();
        if (nodeName == "stack") {
            _parseORAStacksRecursively(layers, dimensions, layerNode, zip, xyAdd(offset, XY{ layerNode.attribute("x").as_int(), layerNode.attribute("y").as_int() }));
        }
        else if (nodeName == "layer") {
            XY layerOffset = xyAdd(offset, XY{ layerNode.attribute("x").as_int(), layerNode.attribute("y").as_int() });
            const char* pngPath = layerNode.attribute("src").as_string();
            zip_entry_open(zip, pngPath);
            uint8_t* pngData = NULL;
            size_t pngSize;
            zip_entry_read(zip, (void**)&pngData, &pngSize);

            Layer* nlayer = readPNGFromMem(pngData, pngSize);
            if (nlayer != NULL) {
                Layer* sizeCorrectLayer = new Layer(dimensions.x, dimensions.y);
                sizeCorrectLayer->blit(nlayer, layerOffset);
                delete nlayer;

                sizeCorrectLayer->hidden = std::string(layerNode.attribute("visibility").as_string()) != "visible";
                sizeCorrectLayer->name = std::string(layerNode.attribute("name").as_string());
                layers->insert(layers->begin(), sizeCorrectLayer);
            }
            else {
                logerr(frmt("[OpenRaster] Failed to read layer: {}", pngPath));
            }

            tracked_free(pngData);
            zip_entry_close(zip);
        }
    }
}

MainEditor* readOpenRaster(PlatformNativePathString path)
{
    FILE* f = platformOpenFile(path, PlatformFileModeRB);
    if (f != NULL) {

        MainEditor* ret = NULL;
        //read .ora file using zip
        zip_t* zip = zip_cstream_open(f, ZIP_DEFAULT_COMPRESSION_LEVEL, 'r');
        if (zip == NULL) {
            fclose(f);
            return NULL;
        }

        {
            pugi::xml_document doc;

            zip_entry_open(zip, "stack.xml");
            {
                char* zipBuffer;
                size_t zipBufferSize;
                zip_entry_read(zip, (void**)&zipBuffer, &zipBufferSize);

                //read xml from zipBuffer
                doc.load_string(zipBuffer);

            }
            zip_entry_close(zip);

            pugi::xml_node imgNode = doc.child("image");
            int w = imgNode.attribute("w").as_int();
            int h = imgNode.attribute("h").as_int();

            std::vector<Layer*> layers;
            _parseORAStacksRecursively(&layers, XY{ w,h }, imgNode.child("stack"), zip);
            ret = new MainEditor(layers);
        }

        fclose(f);
        return ret;
    }
    return NULL;
}

bool writeOpenRaster(PlatformNativePathString path, MainEditor* editor)
{
    if (editor->isPalettized) {
        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Palettized image export not implemented"));
        return false;
    }

    std::vector<Layer*> data = editor->layers;
    char* zipBuffer;
    size_t zipBufferSize;

    zip_t* zip = zip_stream_open(NULL, 0, ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');
    {
        zip_entry_open(zip, "mimetype");
        {
            zip_entry_write(zip, "image/openraster", sizeof("image/openraster"));
        }
        zip_entry_close(zip);

        zip_entry_open(zip, "stack.xml");
        {
            std::string xmls = "";
            xmls += frmt("<image version=\"0.0.1\" xres=\"72\" h=\"{}\" w=\"{}\" yres=\"72\">\n", data[0]->h, data[0]->w);
            xmls += frmt(" <stack opacity=\"1\" x=\"0\" name=\"root\" y=\"0\" isolation=\"isolate\" composite-op=\"svg:src-over\" visibility=\"visible\">\n");
            int i = 0;
            for (auto l = data.rbegin(); l != data.rend(); l++) {
                xmls += frmt("  <layer opacity=\"{}\" x=\"0\" name=\"{}\" y=\"0\" src=\"data/layer{}.png\" composite-op=\"svg:src-over\" visibility=\"{}\"/>\n", (*l)->layerAlpha / 255.0f, (*l)->name, i++, (*l)->hidden ? "hidden" : "visible");
            }
            xmls += "  </stack>\n";
            xmls += "</image>\n";
            zip_entry_write(zip, xmls.c_str(), xmls.size());
        }
        zip_entry_close(zip);

        zip_entry_open(zip, "mergedimage.png");
        {
            Layer* flat = editor->flattenImage();
            std::vector<u8> pngData = writePNGToMem(flat);
            zip_entry_write(zip, pngData.data(), pngData.size());
            delete flat;
        }
        zip_entry_close(zip);

        //todo: quantize it to 256 colors and save it as an 8bit png
        zip_entry_open(zip, "Thumbnails/thumbnail.png");
        {
            Layer* flat = editor->flattenImage();
            Layer* flatScaled = flat->copyCurrentVariantScaled(XY{ 255,255 });
            delete flat;
            std::vector<u8> pngData = writePNGToMem(flatScaled);
            zip_entry_write(zip, pngData.data(), pngData.size());
            delete flatScaled;
        }
        zip_entry_close(zip);

        int i = 0;
        for (auto l = data.rbegin(); l != data.rend(); l++) {
            std::vector<u8> pngData = writePNGToMem(*l);
            std::string fname = frmt("data/layer{}.png", i++);
            zip_entry_open(zip, fname.c_str());
            zip_entry_write(zip, pngData.data(), pngData.size());
            zip_entry_close(zip);
        }

        zip_stream_copy(zip, (void**)&zipBuffer, &zipBufferSize);
    }
    zip_close(zip);

    FILE* f = platformOpenFile(path, PlatformFileModeWB);
    if (f != NULL) {

        fwrite(zipBuffer, zipBufferSize, 1, f);
        fclose(f);
        tracked_free(zipBuffer);
        return true;
    }
    tracked_free(zipBuffer);
    return false;
}