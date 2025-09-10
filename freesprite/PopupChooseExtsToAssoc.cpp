#include "Notification.h"
#include "PopupChooseExtsToAssoc.h"
#include "ScrollingPanel.h"
#include "UICheckbox.h"
#include "FileIO.h"

PopupChooseExtsToAssoc::PopupChooseExtsToAssoc()
{
    wxHeight = 400;
    wxWidth = 850;

    ScrollingPanel* sp = new ScrollingPanel();
    sp->wxWidth = wxWidth - 20;
    sp->wxHeight = wxHeight - 120;
    sp->position = XY{ 10, 80 };
    wxsManager.addDrawable(sp);

    int y = 0;
    for (auto& importer : g_fileImporters) {
        std::string ext = importer->extension();
        if (ext != "" && ext[0] == '.') {
            filetypes[ext] = false;
            std::string labelText = frmt("{} ({})", importer->name(), importer->extension());
            std::string alreadyAssoc = platformGetFileAssocForExtension(ext);
            UICheckbox* cb = new UICheckbox(labelText, false);
            if (alreadyAssoc != "") {
                cb->label->setText(frmt("{}    [-> {}]", labelText, alreadyAssoc));
                cb->label->color = alreadyAssoc == "voidsprite" ? SDL_Color{0xB3, 0xEA, 0xFF, 0xFF} 
                                   : SDL_Color{0xFC,0xFF,0xB6, 0xFF};
            }
            cb->onStateChangeCallback = [this, ext](UICheckbox* id, bool newState) {
                this->filetypes[ext] = newState;
            };
            cb->position = XY{ 0, y };
            sp->subWidgets.addDrawable(cb);
            y += 35;
        }
    }

    UICheckbox* dontOpenLaunchpadOnAssoc = new UICheckbox(TL("vsp.config.associateexts.dontopenlaunchpad"), false);
    dontOpenLaunchpadOnAssoc->position = { 5, wxHeight - 5 - dontOpenLaunchpadOnAssoc->checkbox->getDimensions().y };
    wxsManager.addDrawable(dontOpenLaunchpadOnAssoc);

    UIButton* closeBtn = actionButton(TL("vsp.cmn.cancel"));
    closeBtn->onClickCallback = [this](UIButton* id) {
        this->closePopup();
    };

    UIButton* confirmBtn = actionButton(TL("vsp.cmn.apply"));
    confirmBtn->onClickCallback = [this, dontOpenLaunchpadOnAssoc](UIButton* id) {
        std::vector<std::string> exts;
        for (auto& p : filetypes) {
            if (p.second) {
                exts.push_back(p.first);
            }
        }
        if (exts.size() > 0) {
            std::vector<std::string> additionalArgs = {};
            if (dontOpenLaunchpadOnAssoc->isChecked()) {
                additionalArgs.push_back("--no-launchpad");
            }
            if (platformAssocFileTypes(exts, additionalArgs)) {
                g_addNotification(SuccessNotification(TL("vsp.config.associateexts.success"), ""));
                closePopup();
            }
            else {
                g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.config.associateexts.error")));
            }
        }
        else {
            g_addNotification(ErrorNotification(TL("vsp.cmn.error"), TL("vsp.config.associateexts.noselect")));
        }
    };


    makeTitleAndDesc(TL("vsp.config.associateexts.title"), TL("vsp.config.associateexts.desc"));
}
