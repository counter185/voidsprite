#include "PopupFilePicker.h"
#include "FontRenderer.h"
#include "Notification.h"
#include "PopupYesNo.h"
#include "UIDropdown.h"
#include "FileIO.h"
#include "PopupContextMenu.h"
#include "PopupTextBox.h"

PopupFilePicker::PopupFilePicker(FilePickerMode m, std::string title, std::vector<std::pair<std::string, std::string>> ftypes) : mode(m), fileTypes(ftypes) {
    setSize({ 960, 540 });

    rootDirs = platformListRootDirectories();

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

    currentDirField = new UITextField("--file path");
    currentDirField->position = { 10 + driveList->wxWidth, 40 };
    currentDirField->fontsize = 16;
    currentDirField->wxHeight = 26;
    currentDirField->wxWidth = fileList->wxWidth - 30;
    currentDirField->onTextChangedConfirmCallback = [this](UITextField* f, std::string a) {
        PlatformNativePathString p = convertStringOnWin32(a);
        if (std::filesystem::exists(p)) {
            if (std::filesystem::is_directory(p)) {
                this->currentDir = p;
                this->populateFileList();
            } else {
                f->setText(convertStringToUTF8OnWin32(currentDir));
                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.filepicker.error.notadir")));
            }
        } else {
            f->setText(convertStringToUTF8OnWin32(currentDir));
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.filepicker.error.nodir")));
        }
    };
    wxsManager.addDrawable(currentDirField);

    //todo: icon
    UIButton* newFolderButton = new UIButton("+");
    newFolderButton->tooltip = TL("vsp.filepicker.newfolder");
    newFolderButton->position = xyAdd(currentDirField->position, { currentDirField->wxWidth, 0 });
    newFolderButton->wxWidth = 30;
    newFolderButton->wxHeight = currentDirField->wxHeight;
    newFolderButton->onClickCallback = [this](UIButton* btn) {
        PopupTextBox* popup = new PopupTextBox(TL("vsp.filepicker.newfolder"), TL("vsp.filepicker.newfolder.desc"), TL("vsp.filepicker.newfolder.default"), 300);
        popup->allowEmptyText = false;
        popup->onTextInputConfirmedCallback = [this](PopupTextBox* popup, std::string text) {
            PlatformNativePathString newFolderPath = appendPath(currentDir, convertStringOnWin32(popup->tbox->getText()));
            try {
                if (std::filesystem::exists(newFolderPath)) {
                    g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.filepicker.error.folderexists")));
                }
                else {
                    if (std::filesystem::create_directory(newFolderPath)) {
                        this->populateFileList();
                    }
                    else {
                        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.filepicker.error.cantcreatefolder")));
                    }
                }
            }
            catch (std::exception& e) {
                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.filepicker.error.cantcreatefolder")));
                logerr(frmt("can't create folder:\n {}", e.what()));
            }
        };
        g_addPopup(popup);
    };
    wxsManager.addDrawable(newFolderButton);

    UILabel* fileNameLabel = new UILabel(TL("vsp.filepicker.filename"));
    fileNameLabel->position = xyAdd(fileList->position, { 0, fileList->wxHeight + 10 });
    wxsManager.addDrawable(fileNameLabel);
    XY ep = xyAdd(fileNameLabel->calcEndpoint(), {10,0});

    currentFileName = new UITextField();
    currentFileName->position = ep;
    currentFileName->wxWidth = fileList->wxWidth - 10 - fileNameLabel->getDimensions().x;
    currentFileName->wxHeight = 30;
    currentFileName->onTextChangedConfirmCallback = [this](UITextField* tf, std::string text) {
        this->confirmButton->click();
    };
    currentFileName->onTextChangedCallback = [this](UITextField* tf, std::string text) {
        populateFileList();
    };
    wxsManager.addDrawable(currentFileName);

    std::vector<std::pair<std::string, std::string>> typesInDropdown;
    for (auto& fileType : fileTypes) {
        if (!fileType.first.empty()) {
            typesInDropdown.push_back({ fileType.second + " (" + fileType.first + ")", "" });
        }
        else {
            typesInDropdown.push_back({ fileType.second, "" });
        }
    }
    fileTypeDropdown = new UIDropdown(typesInDropdown);
    fileTypeDropdown->position = xyAdd(fileNameLabel->position, { 0, currentFileName->wxHeight + 5 });
    fileTypeDropdown->wxWidth = wxWidth - fileTypeDropdown->position.x - 10;
    fileTypeDropdown->setTextToSelectedItem = true;
    fileTypeDropdown->text = typesInDropdown[currentFileTypeIndex].first;
    fileTypeDropdown->onDropdownItemSelectedCallback = [this](UIDropdown* dropdown, int index, std::string item) {
        currentFileTypeIndex = index;
        filesInCurrentDirValid = false;
        currentFileName->setText("");
        populateFileList();
    };
    wxsManager.addDrawable(fileTypeDropdown);

    UIButton* cancelButton = actionButton(TL("vsp.cmn.cancel"));
    cancelButton->onClickCallback = [this](UIButton* btn) {
        this->closePopup();
    };
    confirmButton = actionButton(TL("vsp.cmn.confirm"));
    confirmButton->onClickCallback = [this](UIButton* btn) {
        PlatformNativePathString fullFilePath = appendPath(currentDir, convertStringOnWin32(currentFileName->getText()));
        if (mode == FILEPICKER_OPENFILE) {
            if (currentFileName->getText().size() == 0 || !std::filesystem::exists(fullFilePath)) {
                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.filepicker.filenotfound")));
            } else {
                if (callback != NULL) {
                    callback->eventFileOpen(callback_id, fullFilePath, currentFileTypeIndex + 1);
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

    populateRootDirsList();
    chooseDirectory(currentDir = rootDirs.empty() ? convertStringOnWin32("/") : rootDirs[0].path);
}

void PopupFilePicker::PlatformAnyImageImportDialog(EventCallbackListener* callback, std::string title, int callback_id, bool allowAny) {
    std::vector<std::pair<std::string, std::string>> filetypes;
    if (allowAny) {
        filetypes.push_back({ "", TL("vsp.filepicker.autofiletype") });
    }
    for (FileImporter*& f : g_fileImporters) {
        filetypes.push_back({ f->extension(), f->name() });
    }

    platformTryLoadOtherFile(callback, filetypes, title, callback_id);
}

void PopupFilePicker::PlatformAnyImageWithMatchingExporterImportDialog(EventCallbackListener* callback, std::string title, int callback_id) {
    std::vector<std::pair<std::string, std::string>> filetypes;
    for (FileImporter*& f : g_fileImporters) {
        if (f->getCorrespondingExporter() != NULL) {
            filetypes.push_back({ f->extension(), f->name() });
        }
    }

    platformTryLoadOtherFile(callback, filetypes, title, callback_id);
}

void PopupFilePicker::populateFileList() {
    
    fileList->subWidgets.freeAllDrawables();

    currentDirField->setText(convertStringToUTF8OnWin32(currentDir));

    //file buttons
    int fileY = 5;
    UIButton* btn = new UIButton("..");
    btn->position = { 5, fileY };
    btn->wxWidth = g_fnt->StatStringDimensions("..").x + 20;
    btn->wxHeight = 30;
    btn->onClickCallback = [this](UIButton* btn) {
        chooseDirectory(std::filesystem::path(currentDir).parent_path());
    };
    fileList->subWidgets.addDrawable(btn);
    fileY += btn->wxHeight;

    try {

        getFilesInCurrentDir();

        std::string searchQuery = stringToLower(currentFileName->getText());
        std::vector<FilePickerFileEntry> searchResults;
        for (auto& fileEntry : filesInCurrentDir) {
            std::string lowerFileName = stringToLower(fileEntry.displayFileName);
            if (lowerFileName.find(searchQuery) != std::string::npos) {
                searchResults.push_back(fileEntry);
            }
        }

        if (!searchQuery.empty()) {
            fileList->subWidgets.addDrawable(new UILabel(TL("vsp.filepicker.searchlist"), {5, fileY}));
            fileY += 30;

            for (auto& searchFileEntry : searchResults) {
                UIButton* btn = createFileButton(searchFileEntry);
                btn->position = { 15, fileY };
                fileList->subWidgets.addDrawable(btn);
                fileY += btn->wxHeight;
            }

            fileY += 20;
        }

        for (auto& fileEntry : filesInCurrentDir) {
            UIButton* btn = createFileButton(fileEntry);
            btn->position = { 5, fileY };
            fileList->subWidgets.addDrawable(btn);
            fileY += btn->wxHeight;
        }
    } catch (std::exception& e) {
        UILabel* l = new UILabel(frmt("{}\n  {}", TL("vsp.filepicker.folderreaderror"), e.what()));
        l->position = {5, fileY};
        fileList->subWidgets.addDrawable(l);
    }
}

void PopupFilePicker::chooseDirectory(PlatformNativePathString dir)
{
    filesInCurrentDirValid = false;
    currentDir = dir;
    currentFileName->setText("");
    populateFileList();
}

void PopupFilePicker::getFilesInCurrentDir()
{
    if (!filesInCurrentDirValid) {
        std::string targetExtension = fileTypes[currentFileTypeIndex].first;

        filesInCurrentDir.clear();
        for (auto& file : std::filesystem::directory_iterator(currentDir)) {
            std::string utf8name = convertStringToUTF8OnWin32(file.path().filename());
            filesInCurrentDir.push_back({
                file.path().filename(),
                utf8name,
                file.is_directory(),
                stringEndsWithIgnoreCase(utf8name, targetExtension),
                file.is_symlink()
                });
        }

        std::sort(filesInCurrentDir.begin(), filesInCurrentDir.end(), [](FilePickerFileEntry& left, FilePickerFileEntry& right) {
            if (left.isDirectory != right.isDirectory) {
                return left.isDirectory > right.isDirectory;
            }
            else if (left.matchesExtension != right.matchesExtension) {
                return left.matchesExtension > right.matchesExtension;
            }
            else {
                return stringToLower(left.displayFileName) < stringToLower(right.displayFileName);
            }
            });
        filesInCurrentDirValid = true;
    }
}

UIButton* PopupFilePicker::createFileButton(FilePickerFileEntry fileEntry)
{
    bool matchesExtension = fileEntry.matchesExtension;

    UIButton* btn = new UIButton(fileEntry.displayFileName + (fileEntry.isDirectory ? "/" : ""));
    btn->wxWidth = g_fnt->StatStringDimensions(btn->text).x + 15 + 30;
    btn->wxHeight = 30;

    btn->onRightClickCallback = [this, fileEntry](UIButton* btn) {
        g_openContextMenu({
            NamedOperation{TL("vsp.filepicker.deletefile"), 
                [this, fileEntry]() {
                    auto p = appendPath(currentDir, fileEntry.realFileName);
                    PopupYesNo* popup = new PopupYesNo(TL("vsp.filepicker.deletefile.confirm"), 
                        frmt("{}\n  {}", TL("vsp.filepicker.deletefile.confirm.desc"), convertStringToUTF8OnWin32(p)));
                    popup->onFinishCallback = [this, p](PopupYesNo* popup, bool yes) {
                        if (yes) {
                            try {
                                std::filesystem::remove_all(p);
                                populateFileList();
                            } catch (std::exception& e) {
                                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), frmt("{}\n  {}", TL("vsp.filepicker.error.deletefile"), e.what())));
                            }
                        }
                    };
                    g_addPopup(popup);
                }
            },
            NamedOperation{TL("vsp.filepicker.renamefile"), 
                [this, fileEntry]() {
                    auto p = appendPath(currentDir, fileEntry.realFileName);
                    PopupTextBox* popup = new PopupTextBox(TL("vsp.filepicker.renamefile.confirm"), 
                        frmt("{}\n  {}", TL("vsp.filepicker.renamefile.desc"), fileEntry.displayFileName),
                        convertStringToUTF8OnWin32(fileEntry.realFileName));
                    popup->allowEmptyText = false;
                    popup->onTextInputConfirmedCallback = [this, p](PopupTextBox* popup, std::string newName) {
                        try {
                            std::filesystem::rename(p, appendPath(currentDir, convertStringOnWin32(newName)));
                            populateFileList();
                        } catch (std::exception& e) {
                            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), frmt("{}\n  {}", TL("vsp.filepicker.error.renamefile"), e.what())));
                        }
                    };
                    g_addPopup(popup);
                }
            },
        });
    };

    SDL_Color primaryColor = fileEntry.isLink ? visualConfigColor("popup/filepicker/symlink_color")
                            : fileEntry.isDirectory ? visualConfigColor("popup/filepicker/directory_color")
                            : matchesExtension ? visualConfigColor("popup/filepicker/file_matching_color")
                            : visualConfigColor("popup/filepicker/file_nonmatching_color");

    u32 primaryColorU32 = sdlcolorToUint32(primaryColor);

    btn->icon =
        fileEntry.isLink ? g_iconFilePickerLink
        : fileEntry.isDirectory ? g_iconFilePickerDirectory
        : matchesExtension ? g_iconFilePickerSupportedFile
        : g_iconFilePickerFile;

    btn->colorTextFocused = btn->colorTextUnfocused = primaryColor;
            
    u32 fillBGPrimary = alphaBlend(0xD0000000, modAlpha(primaryColorU32, 0x30));
    btn->fill = Fill::Gradient(fillBGPrimary, 0xD0000000, fillBGPrimary, 0xD0000000);

    PlatformNativePathString wFileName = fileEntry.realFileName;
    std::string fileName = fileEntry.displayFileName;
    if (fileEntry.isDirectory) {
        btn->onClickCallback = [this, wFileName](UIButton *btn) {
            chooseDirectory(appendPath(currentDir, wFileName));
        };
    } else {
        btn->onClickCallback = [this, fileName](UIButton *btn) {
            currentFileName->setText(fileName);
        };
    }
    return btn;
}

void PopupFilePicker::populateRootDirsList()
{
    //root dir buttons
    driveList->subWidgets.freeAllDrawables();
    driveList->scrollOffset = { 0,0 };
    int rootY = 0;
    for (auto& rootDir : rootDirs) {
        UIButton* btn = new UIButton(rootDir.friendlyName);
        btn->position = { 0, rootY };
        btn->wxWidth = driveList->wxWidth - 30;
        btn->wxHeight = 30;
        btn->onClickCallback = [this, rootDir](UIButton* btn) {
            chooseDirectory(rootDir.path);
            };
        driveList->subWidgets.addDrawable(btn);
        rootY += btn->wxHeight;
    }
}
