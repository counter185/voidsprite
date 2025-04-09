#include "CustomTemplate.h"
#include "FileIO.h"

Layer* CustomTemplate::generate()
{
    MainEditor* ssn = loadAnyIntoSession(pathToFile);
    if (ssn == NULL) {
        return NULL;
    } else {
        Layer* l = ssn->flattenImage();

        tilesize = ssn->tileDimensions;
        tilepadding = ssn->tileGridPaddingBottomRight;
        comments = ssn->comments;

        delete ssn;
        return l;
    }
}

std::vector<CommentData> CustomTemplate::placeComments()
{
    return comments;
}
