#pragma once
#include "BasePopup.h"
class PopupListRecoveryAutosaves :
    public BasePopup
{
private:
    ScrollingPanel* scrollPanel = NULL;
    UILabel* allFilesSizeLabel = NULL;
public:
    PopupListRecoveryAutosaves();

    void refreshList();
    static u64 getAllRecoveryFilesSize();
};

