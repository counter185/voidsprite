#include "CustomTemplate.h"
#include "FileIO.h"

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
