#pragma once
#include "BasePopup.h"
#include "EventCallbackListener.h"

struct FormatDef {
    std::string name;
    std::string extension;
    std::string description;
    void* udata;

    std::string originalName = "";
};

class PopupChooseFormat : public BasePopup, EventCallbackListener
{
protected:
    std::vector<FormatDef> formats;
    std::string filterQuery = "";
    ScrollingPanel* formatsPanel = NULL;
    void* highlightUdata = NULL;

    std::function<void(FormatDef*,PlatformNativePathString)> onEventFileSavedCallback = NULL;

    bool hasDefaultFormats = false;
    std::vector<FormatDef> defaultFormats;

    void buildFormatList();
public:
    FormatDef* chosenTarget = NULL;

    std::function<void(FormatDef*)> onFormatChosenCallback = NULL;

    PopupChooseFormat(std::string tt, std::string tx, std::vector<FormatDef> f);

    void eventFileSaved(int evt_id, PlatformNativePathString name, int exporterIndex) override;

    static PopupChooseFormat* withDefaultExportFormats(std::string tt, std::string tx, u32 formatFlags = 0);
    static PopupChooseFormat* withDefaultRGBExportFormats(std::string tt, std::string tx);
    static PopupChooseFormat* withDefaultIndexedExportFormats(std::string tt, std::string tx);

    static PopupChooseFormat* withDefaultPaletteExportFormats(std::string tt, std::string tx);

    static std::vector<FormatDef> processFavouriteFormats(std::vector<FormatDef> srcList);

    void setHighlightUdata(void* udata);
    void filterList(std::string search);
    void chooseFormatAndDoFileSavePrompt(std::string promptTitle, std::function<void(FormatDef*,PlatformNativePathString)> callback);
    void finish(FormatDef* target);

};