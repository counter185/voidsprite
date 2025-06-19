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
    UITextField* currentDirField = NULL;
    UITextField* currentFileName = NULL;
protected:
    ScrollingPanel* driveList = NULL;
    ScrollingPanel* fileList = NULL;
    std::vector<RootDirInfo> rootDirs;
    UIDropdown* fileTypeDropdown = NULL;
    //first: extension, second: name
    std::vector<std::pair<std::string,std::string>> fileTypes;
    int currentFileTypeIndex = 0;
public:
    PopupFilePicker(FilePickerMode mode, std::string title, std::vector<std::pair<std::string,std::string>> fileTypes);

    static PopupFilePicker* OpenFile(std::string title, std::vector<std::pair<std::string,std::string>> fileTypes) {
        return new PopupFilePicker(FILEPICKER_OPENFILE, title, fileTypes);
    }
    static PopupFilePicker* SaveFile(std::string title, std::vector<std::pair<std::string,std::string>> fileTypes) {
        return new PopupFilePicker(FILEPICKER_SAVEFILE, title, fileTypes);
    }

    static void PlatformAnyImageImportDialog(EventCallbackListener* callback, std::string title, int callback_id);

    static void PlatformAnyImageWithMatchingExporterImportDialog(EventCallbackListener* callback, std::string title, int callback_id);

    void populateRootAndFileList();
};

