#include "PopupTextTool.h"
#include "globals.h"
#include "FontRenderer.h"
#include "brush/ToolText.h"
#include "UITextField.h"
#include "UIButton.h"
#include "UILabel.h"
#include "EventCallbackListener.h"
#include "UIDropdown.h"

PopupTextTool::PopupTextTool(ToolText* parent, std::string tt, std::string tx)
{
    caller = parent;
    textSize = parent->textSize;

    textbox = new UITextField();
    textbox->position = XY{ 20, 80 };
    textbox->setText(parent->text);
    textbox->wxWidth = 260;
    wxsManager.addDrawable(textbox);

    UILabel* label = new UILabel(TL("vsp.texttool.fontsize"));
    label->position = XY{ 20, 120 };
    wxsManager.addDrawable(label);

    textboxSize = new UITextField();
    textboxSize->position = XY{ 120, 120 };
    textboxSize->setText(std::to_string(parent->textSize));
    textboxSize->isNumericField = true;
    textboxSize->wxWidth = 120;
    wxsManager.addDrawable(textboxSize);

    wxsManager.addDrawable(new UILabel(TL("vsp.texttool.font"), { 20, 160 }));

    foundFonts = listAllSystemFonts();
    std::vector<std::string> fontNames;
    std::transform(foundFonts.begin(), foundFonts.end(), std::back_inserter(fontNames), fileNameFromPath);
    fontsDropdown = new UIDropdown(fontNames);
    fontsDropdown->position = XY{ 90, 160 };
    fontsDropdown->setTextToSelectedItem = true;
    fontsDropdown->onDropdownItemSelectedCallback = [this](UIDropdown* dd, int index, std::string name) {
        if (index >= 0 && index < foundFonts.size()) {
            fontPath = foundFonts[index];
        } else {
            fontPath = "";
        }
    };
    wxsManager.addDrawable(fontsDropdown);

    UIButton* nbutton = actionButton(TL("vsp.cmn.apply"));
    nbutton->onClickCallback = [this](UIButton*) {
        textSize = std::stoi(textboxSize->getText());
        caller->eventPopupClosed(EVENT_TOOLTEXT_POSTCONFIG, this);
        closePopup();
    };

    UIButton* nbutton2 = actionButton(TL("vsp.cmn.cancel"));
    nbutton2->onClickCallback = [this](UIButton*) {
        closePopup();
    };

    makeTitleAndDesc(tt, tx);
}

void PopupTextTool::defaultInputAction(SDL_Event evt)
{
    if (evt.type == SDL_EVENT_DROP_FILE) {
        std::string filePath = evt.drop.data;
        if (stringEndsWithIgnoreCase(filePath, ".ttf")) {
            fontPath = filePath;
            loginfo(frmt("[ToolText] changed font to {}", filePath));
            fontsDropdown->text = fileNameFromPath(filePath);
        }
    }
}

std::vector<std::string> PopupTextTool::listAllSystemFonts()
{
    auto paths = platformGetSystemFontPaths();
    std::vector<std::string> ret;
    for (auto& path : paths) {
        for (auto& file : platformListFilesInDir(path, ".ttf")) {
            ret.push_back(convertStringToUTF8OnWin32(file));
        }
    }
    return ret;
}
