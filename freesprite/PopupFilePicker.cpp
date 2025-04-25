#include "PopupFilePicker.h"
#include "FontRenderer.h"
#include "Notification.h"
#include "PopupYesNo.h"
#include "UIDropdown.h"

PopupFilePicker::PopupFilePicker(FilePickerMode m, std::string title, std::vector<std::pair<std::string, std::string>> ftypes) : mode(m), fileTypes(ftypes) {
    setSize({ 960, 540 });

    rootDirs = platformListRootDirectories();
    currentDir = rootDirs[0].path;

    driveList = new ScrollingPanel();
    driveList->position = { 3, 65 };
    driveList->wxWidth = 200;
    driveList->wxHeight = wxHeight - 180;
    driveList->bgColor = Fill::Gradient(0xFF101010, 0xFF101010, 0xFF101010, 0xFF202020);
    wxsManager.addDrawable(driveList);

    fileList = new ScrollingPanel();
    fileList->position = { 10 + driveList->wxWidth, 65 };
    fileList->wxWidth = wxWidth - 20 - driveList->wxWidth;
    fileList->wxHeight = wxHeight - 180;
    fileList->bgColor = Fill::Gradient(0xFF101010, 0xFF101010, 0xFF101010, 0xFF202020);
    wxsManager.addDrawable(fileList);

    currentDirLabel = new UILabel("--file path");
    currentDirLabel->position = { 10 + driveList->wxWidth, 40 };
    currentDirLabel->fontsize = 16;
    wxsManager.addDrawable(currentDirLabel);

    UILabel* fileNameLabel = new UILabel(TL("vsp.filepicker.filename"));
    fileNameLabel->position = xyAdd(fileList->position, { 0, fileList->wxHeight + 10 });
    wxsManager.addDrawable(fileNameLabel);
    XY ep = xyAdd(fileNameLabel->calcEndpoint(), {10,0});

    currentFileName = new UITextField();
    currentFileName->position = ep;
    currentFileName->wxWidth = fileList->wxWidth - 10 - fileNameLabel->getDimensions().x;
    currentFileName->wxHeight = 30;
    wxsManager.addDrawable(currentFileName);

    std::vector<std::pair<std::string, std::string>> typesInDropdown;
    for (auto& fileType : fileTypes) {
        typesInDropdown.push_back({fileType.first + " (" + fileType.second + ")", ""});
    }
    fileTypeDropdown = new UIDropdown(typesInDropdown);
    fileTypeDropdown->position = xyAdd(fileNameLabel->position, { 0, currentFileName->wxHeight + 5 });
    fileTypeDropdown->wxWidth = wxWidth - fileTypeDropdown->position.x - 10;
    fileTypeDropdown->setTextToSelectedItem = true;
    fileTypeDropdown->text = typesInDropdown[currentFileTypeIndex].first;
    fileTypeDropdown->onDropdownItemSelectedCallback = [this](UIDropdown* dropdown, int index, std::string item) {
        currentFileTypeIndex = index;
        currentFileName->setText("");
        populateRootAndFileList();
    };
    wxsManager.addDrawable(fileTypeDropdown);

    UIButton* cancelButton = actionButton(TL("vsp.cmn.cancel"));
    cancelButton->onClickCallback = [this](UIButton* btn) {
        this->closePopup();
    };
    UIButton* okButton = actionButton(TL("vsp.cmn.confirm"));
    okButton->onClickCallback = [this](UIButton* btn) {
        PlatformNativePathString fullFilePath = appendPath(currentDir, convertStringOnWin32(currentFileName->getText()));
        if (mode == FILEPICKER_OPENFILE) {
            if (currentFileName->getText().size() == 0 || !std::filesystem::exists(fullFilePath)) {
                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.filepicker.filenotfound")));
            } else {
                if (callback != NULL) {
                    callback->eventFileOpen(callback_id, fullFilePath, -1);
                }
                this->closePopup();
            }
        } else if (mode == FILEPICKER_SAVEFILE) {
            std::string targetExtension = fileTypes[currentFileTypeIndex].first;
            if (!stringEndsWithIgnoreCase(currentFileName->getText(), targetExtension)) {
                fullFilePath += convertStringOnWin32(targetExtension);
            }
            if (currentFileName->getText().size() > 0) {
                if (std::filesystem::exists(fullFilePath)) {
                    PopupYesNo* popup = new PopupYesNo(TL("vsp.filepicker.overwrite"), TL("vsp.filepicker.overwriteconfirm"));
                    popup->onFinishCallback = [this, fullFilePath](PopupYesNo* popup, bool yes) {
                        if (yes) {
                            if (callback != NULL) {
                                callback->eventFileSaved(callback_id, fullFilePath, currentFileTypeIndex + 1);
                            }
                            this->closePopup();
                        }
                    };
                    g_addPopup(popup);
                } else {
                    if (callback != NULL) {
                        callback->eventFileSaved(callback_id, fullFilePath, currentFileTypeIndex + 1);
                    }
                    this->closePopup();
                }
            } else {
                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.filepicker.nofilename")));
            }
        }
    };

    makeTitleAndDesc("voidsprite: " + title, "");

    populateRootAndFileList();
}

void PopupFilePicker::populateRootAndFileList() {
    driveList->subWidgets.freeAllDrawables();
    fileList->subWidgets.freeAllDrawables();

    currentDirLabel->setText(convertStringToUTF8OnWin32(currentDir));
    currentFileName->setText("");

    int rootY = 0;
    for (auto& rootDir : rootDirs) {
        UIButton* btn = new UIButton(rootDir.friendlyName);
        btn->position = { 0, rootY };
        btn->wxWidth = driveList->wxWidth - 30;
        btn->wxHeight = 30;
        btn->onClickCallback = [this, rootDir](UIButton* btn) {
            currentDir = rootDir.path;
            populateRootAndFileList();
        };
        driveList->subWidgets.addDrawable(btn);
        rootY += btn->wxHeight;
    }

    int fileY = 5;
    UIButton* btn = new UIButton("..");
    btn->position = { 5, fileY };
    btn->wxWidth = g_fnt->StatStringDimensions("..").x + 20;
    btn->wxHeight = 30;
    btn->onClickCallback = [this](UIButton* btn) {
        currentDir = std::filesystem::path(currentDir).parent_path();
        populateRootAndFileList();
    };
    fileList->subWidgets.addDrawable(btn);
    fileY += btn->wxHeight;

    std::string targetExtension = fileTypes[currentFileTypeIndex].first;

    for (auto& file : std::filesystem::directory_iterator(currentDir)) {
        PlatformNativePathString wFileName = file.path().filename();
        std::string fileName = convertStringToUTF8OnWin32(wFileName);
        UIButton* btn = new UIButton( fileName + (file.is_directory() ? "/" : "") );
        btn->position = { 5, fileY };
        btn->wxWidth = g_fnt->StatStringDimensions(fileName).x + 20;
        btn->wxHeight = 30;

        btn->colorTextFocused = btn->colorTextUnfocused =
            file.is_directory() ? SDL_Color{ 0xFF, 0xFC, 0x7B, 0xFF } 
            : stringEndsWithIgnoreCase(fileName, targetExtension) ? SDL_Color{ 0xFF, 0xFF, 0xFF, 0xFF }
            : SDL_Color{ 0xFF, 0xFF, 0xFF, 0x80 };

        if (file.is_directory()) {
            btn->onClickCallback = [this, wFileName](UIButton* btn) {
                currentDir = appendPath(currentDir, wFileName);
                populateRootAndFileList();
            };
        } else {
            btn->onClickCallback = [this, fileName](UIButton* btn) {
                currentFileName->setText(fileName);
            };
        }
        fileList->subWidgets.addDrawable(btn);
        fileY += btn->wxHeight;
    }
}
