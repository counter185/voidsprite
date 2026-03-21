#include "CustomTemplate.h"
#include "FileIO.h"
#include "io/io_voidsprite.h"

CustomTemplate::CustomTemplate(PlatformNativePathString path) : pathToFile(convertStringToUTF8OnWin32(path)) {
    name = fileNameFromPath(pathToFile);
    
    try {
        auto keyvals = voidsnReadKeyVals(path);
        if (keyvals.contains("template.name")) {
            name = keyvals["template.name"];
            description = keyvals["template.desc"];
        }
    }
    catch (...) {}
}

MainEditor* CustomTemplate::generateSession()
{
    MainEditor* ssn = loadAnyIntoSession(pathToFile);
    if (ssn == NULL) {
        return NULL;
    }
    else {
        ssn->lastConfirmedSave = false;
        ssn->lastConfirmedExporter = NULL;
        ssn->lastConfirmedSavePath.clear();
        return ssn;
    }
}
