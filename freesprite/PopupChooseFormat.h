#pragma once
#include "BasePopup.h"
#include "EventCallbackListener.h"

struct FormatDef {
    std::string name;
    std::string extension;
    std::string description;
    void* udata;
};

class PopupChooseFormat : public BasePopup, EventCallbackListener
{
protected:
    std::vector<FormatDef> formats;
    std::string filterQuery = "";
    ScrollingPanel* formatsPanel = NULL;

    std::function<void(FormatDef*,PlatformNativePathString)> onEventFileSavedCallback = NULL;

    void buildFormatList();
public:
    FormatDef* chosenTarget = NULL;

    std::function<void(FormatDef*)> onFormatChosenCallback = NULL;

    PopupChooseFormat(std::string tt, std::string tx, std::vector<FormatDef> f);

    void eventFileSaved(int evt_id, PlatformNativePathString name, int exporterIndex) override;

    static PopupChooseFormat* withDefaultRGBExportFormats(std::string tt, std::string tx);
    static PopupChooseFormat* withDefaultIndexedExportFormats(std::string tt, std::string tx);

    void filterList(std::string search);
    void chooseFormatAndDoFileSavePrompt(std::string promptTitle, std::function<void(FormatDef*,PlatformNativePathString)> callback);
    void finish(FormatDef* target);

};