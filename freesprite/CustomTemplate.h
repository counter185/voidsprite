#pragma once
#include "BaseTemplate.h"
class CustomTemplate :
    public BaseTemplate
{
private:
    CustomTemplate() {}
public:
    std::string name = "";
    std::string description = "";
    Layer* image = NULL;

    std::string pathToFile;

    CustomTemplate(PlatformNativePathString path) : pathToFile(convertStringToUTF8OnWin32(path)) {
        name = fileNameFromPath(pathToFile);
    }
    
    std::string getName() override { return name; };
    std::string getDescription() override { return description; }
    MainEditor* generateSession() override;
};

