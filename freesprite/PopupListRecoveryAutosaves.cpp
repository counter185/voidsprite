#include "PopupListRecoveryAutosaves.h"
#include "ScrollingPanel.h"
#include "UIButton.h"
#include "maineditor.h"
#include "Notification.h"
#include "PopupYesNo.h"
#include "FileIO.h"

PopupListRecoveryAutosaves::PopupListRecoveryAutosaves()
{
    wxWidth = 700;
    makeTitleAndDesc(TL("vsp.launchpad.nav.recoveryautosaves"), TL("vsp.launchpad.recoveryautosaves.desc"));

    scrollPanel = new ScrollingPanel();
    scrollPanel->position = XY{ 10, 110 };
    scrollPanel->wxWidth = wxWidth - 20;
    scrollPanel->wxHeight = 240;
    scrollPanel->scrollVertically = true;
    scrollPanel->scrollHorizontally = false;
    scrollPanel->bgColor = Fill::Solid(0x70101010);
    wxsManager.addDrawable(scrollPanel);

    allFilesSizeLabel = new UILabel();
    allFilesSizeLabel->color = { 255,255,255,0xa0 };
    allFilesSizeLabel->fontsize = 16;
    allFilesSizeLabel->position = XY{ 5, wxHeight - 30 };
    wxsManager.addDrawable(allFilesSizeLabel);

    refreshList();

    UIButton* closeBtn = actionButton(TL("vsp.cmn.close"));
    closeBtn->onClickCallback = [this](UIButton*) { this->closePopup(); };
}

void PopupListRecoveryAutosaves::refreshList()
{
    scrollPanel->subWidgets.freeAllDrawables();

    allFilesSizeLabel->setText(TL("vsp.launchpad.recoveryautosaves.allfilesize") + bytesToFriendlyString(getAllRecoveryFilesSize()));

    std::vector<PlatformNativePathString> autosaveFiles = platformListFilesInDir(platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("/autosaves"), ".voidsn");
    std::sort(autosaveFiles.begin(), autosaveFiles.end(), [](const PlatformNativePathString& a, const PlatformNativePathString& b) {
        return fileNameFromPath(convertStringToUTF8OnWin32(a)) > fileNameFromPath(convertStringToUTF8OnWin32(b));
    });

    int x = 0;
    for (auto& f : autosaveFiles) {
        UIButton* b = new UIButton();
        b->text = fileNameFromPath(convertStringToUTF8OnWin32(f));
        b->position = XY{ 0,x };
        x += 30;
        b->wxWidth = scrollPanel->wxWidth - 40;
        b->wxHeight = 30;
        b->onClickCallback = [this, f](UIButton*) {
            MainEditor* newSsn = loadAnyIntoSession(convertStringToUTF8OnWin32(f));
            if (newSsn != NULL) {
                g_addScreen(newSsn);
                closePopup();
            }
            else {
                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.cmn.error.fileloadfail")));
            }
        };
        b->onRightClickCallback = [this, f](UIButton*) {
            PopupYesNo* confirmPopup = new PopupYesNo(TL("vsp.launchpad.recoveryautosaves.deleteconfirm"),
                TL("vsp.launchpad.recoveryautosaves.deleteconfirm.desc") + "\n\n" + convertStringToUTF8OnWin32(f));
            confirmPopup->onFinishCallback = [this, f](PopupYesNo* p, bool result) {
                if (result) {
                    if (std::filesystem::remove(f)) {
                        //platformDeleteFile(f);
                        g_addNotification(SuccessShortNotification(TL("vsp.launchpad.recoveryautosaves.deleted"), ""));
                        refreshList();
                    }
                    else {
                        g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.launchpad.recoveryautosaves.deletefail")));
                    }
                }
            };
            g_addPopup(confirmPopup);
        };
        scrollPanel->subWidgets.addDrawable(b);
    }
}

u64 PopupListRecoveryAutosaves::getAllRecoveryFilesSize()
{
    u64 totalSize = 0;

    std::filesystem::path autosaveDir = platformEnsureDirAndGetConfigFilePath() + convertStringOnWin32("/autosaves");
    for (auto& f : std::filesystem::directory_iterator(autosaveDir)) {
        if (f.path().extension() == ".voidsn") {
            totalSize += f.file_size();
        }
    }
    return totalSize;
}
