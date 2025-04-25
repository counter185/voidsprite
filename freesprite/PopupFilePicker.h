#pragma once
#include "BasePopup.h"
#include "UILabel.h"
#include "UITextField.h"
#include "UIButton.h"
#include "ScrollingPanel.h"
#include "EventCallbackListener.h"

enum FilePickerMode {
    FILEPICKER_OPENFILE,
    FILEPICKER_SAVEFILE,
    FILEPICKER_OPENFOLDER
};

class PopupFilePicker : public BasePopup
{
private:
    FilePickerMode mode = FILEPICKER_OPENFILE;
    PlatformNativePathString currentDir;
    UILabel* currentDirLabel = NULL;
    UITextField* currentFileName = NULL;
protected:
    ScrollingPanel* driveList = NULL;
    ScrollingPanel* fileList = NULL;
    std::vector<RootDirInfo> rootDirs;
public:
    PopupFilePicker(std::string title, std::vector<std::pair<std::string,std::string>> fileTypes);
    
    void populateRootAndFileList();
};

