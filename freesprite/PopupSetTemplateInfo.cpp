#include "PopupSetTemplateInfo.h"
#include "UIButton.h"
#include "UITextField.h"
#include "maineditor.h"
#include "UIStackPanel.h"
#include "UILabel.h"

PopupSetTemplateInfo::PopupSetTemplateInfo(MainEditor* caller) : caller(caller)
{
    setSize({600,270});
    makeTitleAndDesc("Set template info", "When using this session as a template, these details will appear.");

    UITextField* titleField = new UITextField(caller->ssne.templateName);
    UITextField* descField = new UITextField(caller->ssne.templateDescription);

    titleField->wxWidth = descField->wxWidth = 400;

    UIStackPanel* sp = UIStackPanel::Vertical(3, {
        new UILabel("Template name"),
        titleField,
        Panel::Space(1,8),
        new UILabel("Template description"),
        descField
    });
    sp->position = { 20, 80 };
    wxsManager.addDrawable(sp);


    actionButton(TL("vsp.cmn.cancel"))->onClickCallback = [this](UIButton* btn) {
        closePopup();
    };

    actionButton(TL("vsp.nav.save"))->onClickCallback = [this, caller, titleField, descField](UIButton* btn) {
        caller->ssne.templateName = titleField->getText();
        caller->ssne.templateDescription = descField->getText();
        closePopup();
    };
}
