#pragma once
#include "BaseTemplate.h"
class CustomTemplate :
    public BaseTemplate
{
private:
    CustomTemplate() {}
public:
    std::string name = "";
    Layer* image = NULL;
    XY tilesize = {0,0};
    XY tilepadding = { 0,0 };
    std::vector<CommentData> comments;

    std::string pathToFile;

    CustomTemplate(PlatformNativePathString path) : pathToFile(convertStringToUTF8OnWin32(path)) {
        name = fileNameFromPath(pathToFile);
    }
    
    std::string getName() override { return name; };
    Layer* generate() override;
    XY tileSize() override { return tilesize; }
    XY tilePadding() override { return tilepadding; }
    std::vector<CommentData> placeComments() override;
};

