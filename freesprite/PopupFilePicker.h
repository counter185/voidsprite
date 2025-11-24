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

struct FilePickerFileEntry {
    PlatformNativePathString realFileName;
    std::string displayFileName;
    bool isDirectory;
    bool matchesExtension;
    bool isLink;
};

class PopupFilePicker : public BasePopup
{
private:
    FilePickerMode mode = FILEPICKER_OPENFILE;
    PlatformNativePathString currentDir;
    UITextField* currentDirField = NULL;
    UITextField* currentFileName = NULL;
    UIButton* confirmButton = NULL;
protected:
    ScrollingPanel* driveList = NULL;
    ScrollingPanel* fileList = NULL;
    std::vector<RootDirInfo> rootDirs;
    UIDropdown* fileTypeDropdown = NULL;
    //first: extension, second: name
    std::vector<std::pair<std::string,std::string>> fileTypes;
    int currentFileTypeIndex = 0;

    std::vector<FilePickerFileEntry> filesInCurrentDir;
    bool filesInCurrentDirValid = false;
public:
    PopupFilePicker(FilePickerMode mode, std::string title, std::vector<std::pair<std::string,std::string>> fileTypes);

    static PopupFilePicker* OpenFile(std::string title, std::vector<std::pair<std::string,std::string>> fileTypes) {
        return new PopupFilePicker(FILEPICKER_OPENFILE, title, fileTypes);
    }
    static PopupFilePicker* SaveFile(std::string title, std::vector<std::pair<std::string,std::string>> fileTypes) {
        return new PopupFilePicker(FILEPICKER_SAVEFILE, title, fileTypes);
    }

    static void PlatformAnyImageImportDialog(EventCallbackListener* callback, std::string title, int callback_id, bool allowAny = false);

    static void PlatformAnyImageWithMatchingExporterImportDialog(EventCallbackListener* callback, std::string title, int callback_id);

    void populateFileList();
    void populateRootDirsList();

    void chooseDirectory(PlatformNativePathString dir);
    void getFilesInCurrentDir();
    UIButton* createFileButton(FilePickerFileEntry entry);;
};

