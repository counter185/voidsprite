#include "CustomTemplate.h"
#include "FileIO.h"

CustomTemplate* CustomTemplate::tryLoad(PlatformNativePathString path)
{
    MainEditor* ssn = loadAnyIntoSession(convertStringToUTF8OnWin32(path));
    if (ssn == NULL) {
        return NULL;
    }
    else {
        CustomTemplate* t = new CustomTemplate();
        //todo get name from file name
        PlatformNativePathString fileNameWithNoPathAndExtension = path.substr(path.find_last_of(convertStringOnWin32("/\\")) + 1);
        t->name = convertStringToUTF8OnWin32(fileNameWithNoPathAndExtension.substr(0, fileNameWithNoPathAndExtension.find_last_of(convertStringOnWin32("."))));
        t->image = ssn->flattenImage();
        t->tilesize = ssn->tileDimensions;
        t->tilepadding = ssn->tileGridPaddingBottomRight;
        t->comments = ssn->comments;
        delete ssn;
        return t;
    }
}

Layer* CustomTemplate::generate()
{
    Layer* ret = image->copyScaled({image->w, image->h}); //tell noone
    ret->name = "Template Layer";
    return ret;
}

std::vector<CommentData> CustomTemplate::placeComments()
{
    return comments;
}
