#include "PopupFilePicker.h"
#include "FontRenderer.h"
#include "Notification.h"

PopupFilePicker::PopupFilePicker(std::string title, std::vector<std::pair<std::string, std::string>> fileTypes) {
    mode = FILEPICKER_OPENFILE;
    setSize({ 960, 540 });

    rootDirs = platformListRootDirectories();
    currentDir = rootDirs[0].path;

    driveList = new ScrollingPanel();
    driveList->position = { 3, 60 };
    driveList->wxWidth = 200;
    driveList->wxHeight = wxHeight - 170;
    driveList->bgColor = Fill::Gradient(0xFF101010, 0xFF101010, 0xFF101010, 0xFF202020);
    wxsManager.addDrawable(driveList);

    fileList = new ScrollingPanel();
    fileList->position = { 10 + driveList->wxWidth, 60 };
    fileList->wxWidth = wxWidth - 20 - driveList->wxWidth;
    fileList->wxHeight = wxHeight - 170;
    fileList->bgColor = Fill::Gradient(0xFF101010, 0xFF101010, 0xFF101010, 0xFF202020);
    wxsManager.addDrawable(fileList);

    currentDirLabel = new UILabel("/home/user");
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

    UIButton* cancelButton = actionButton(TL("vsp.cmn.cancel"));
    cancelButton->onClickCallback = [this](UIButton* btn) {
        this->closePopup();
    };
    UIButton* okButton = actionButton(TL("vsp.cmn.confirm"));
    okButton->onClickCallback = [this](UIButton* btn) {
        PlatformNativePathString fullFilePath = appendPath(currentDir, convertStringOnWin32(currentFileName->getText()));
        if (mode == FILEPICKER_OPENFILE) {
            if (currentFileName->getText().size() == 0 || !std::filesystem::exists(fullFilePath)) {
                g_addNotification(ErrorNotification(TL("vsp.filepicker.error"), TL("vsp.filepicker.filenotfound")));
            } else {
                if (callback != NULL) {
                    callback->eventFileOpen(callback_id, fullFilePath, -1);
                }
                this->closePopup();
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

    for (auto& file : std::filesystem::directory_iterator(currentDir)) {
        PlatformNativePathString wFileName = file.path().filename();
        std::string fileName = convertStringToUTF8OnWin32(wFileName);
        UIButton* btn = new UIButton( fileName);
        btn->position = { 5, fileY };
        btn->wxWidth = g_fnt->StatStringDimensions(fileName).x + 20;
        btn->wxHeight = 30;
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
