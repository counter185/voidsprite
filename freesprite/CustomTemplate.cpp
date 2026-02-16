#include "CustomTemplate.h"
#include "FileIO.h"

Layer* CustomTemplate::generate()
{
    MainEditor* ssn = loadAnyIntoSession(pathToFile);
    if (ssn == NULL) {
        return NULL;
    } else {
        Layer* l = ssn->flattenImage();

        tilesize = ssn->ssne.tileDimensions;
        tilepadding = ssn->ssne.tileGridPaddingBottomRight;
        comments = ssn->getCommentStack();

        delete ssn;
        return l;
    }
}

std::vector<CommentData> CustomTemplate::placeComments()
{
    return comments;
}
