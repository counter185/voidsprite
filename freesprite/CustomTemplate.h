#pragma once
#include "BaseTemplate.h"
class CustomTemplate :
    public BaseTemplate
{
public:
    std::string name = "";
    Layer* image = NULL;
    XY tilesize = {0,0};
    XY tilepadding = { 0,0 };
    std::vector<CommentData> comments;

    static CustomTemplate* tryLoad(PlatformNativePathString path);

    CustomTemplate() {
    }
    std::string getName() override { return name; };
    Layer* generate() override;
    XY tileSize() override { return tilesize; }
    XY tilePadding() override { return tilepadding; }
    std::vector<CommentData> placeComments() override;
};

