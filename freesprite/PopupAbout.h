#pragma once

#include "BasePopup.h"
#include "UILabel.h"
#include "UIButton.h"
#include "ScrollingPanel.h"
#include "TabbedView.h"
#include "FileIO.h"
#include "license.h"

inline std::string listOfContributors = ""
#if __ANDROID__
        //fix this symlink some day
        "< list of contributors currently only available on GitHub >"
#else
#include "../CONTRIBUTORS.txt"
#endif
;

class PopupAbout : public BasePopup {
public:
    PopupAbout() {
        setSize({800, 450});
        UIButton* close = actionButton(TL("vsp.cmn.close"));
        close->onClickCallback = [this](UIButton*){ this->closePopup(); };

        std::string generalTabContents =
            getVSPShortLicense()
            + "\n\n-----\nThe full license text is copied below:\n\n\n"
            + getVSPFullLicense();

        std::string contributorTabContents =
            TL("vsp.about.contributors") + "\n"
            + listOfContributors + "\n\n"
            + TL("vsp.about.translations") + "\n";

        for (auto [locale, data] : getLocalizations()) {
            if (data.langCredit != "") {
                contributorTabContents += std::format("{}:  {}", data.langName, data.langCredit);
            }
        }

        std::vector<std::pair<std::string, std::string>> aboutTabContents = {
            {TL("vsp.about.tab.general"), generalTabContents},
            {TL("vsp.about.tab.contributors"), contributorTabContents},
            {TL("vsp.about.tab.systeminfo"), platformGetSystemInfo()},
            {TL("vsp.about.tab.libs"), getAllLibsVersions() + "\n" + getLibraryLicenses()},
            {TL("vsp.about.tab.fonts"), getFontLicense()},
        };

        std::vector<Tab> generatedTabs;
        for (auto& [tabName, _] : aboutTabContents) {
            generatedTabs.push_back({tabName});
        }

        TabbedView* aboutTabs = new TabbedView(generatedTabs);
        aboutTabs->position = XY{10,50};
        wxsManager.addDrawable(aboutTabs);

        int i = 0;
        for (auto& [tabName, tabContents] : aboutTabContents) {
            ScrollingPanel* aboutView = new ScrollingPanel();
            aboutView->position = {0,0};
            aboutView->wxWidth = wxWidth - 20;
            aboutView->wxHeight = wxHeight - 120;
            aboutTabs->tabs[i++].wxs.addDrawable(aboutView);

            UILabel* primaryText = new UILabel(tabContents);
            primaryText->position = {5,5};
            primaryText->fontsize = 16;
            aboutView->subWidgets.addDrawable(primaryText);

        }
        makeTitleAndDesc(TL("vsp.about.title"), "");
    }

};
