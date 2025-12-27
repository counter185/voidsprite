#pragma once
#include "BasePopup.h"
#include "FontRenderer.h"
#include "UIButton.h"
#include "UILabel.h"
class PopupChooseAction :
    public BasePopup
{
protected:
    std::map<SDL_Scancode, UIButton*> actions;
public:

    PopupChooseAction(std::string tt, std::string tx, std::vector<std::pair<SDL_Scancode, NamedOperation>> ops) {

        wxHeight = 200;

        makeTitleAndDesc(tt, tx);

        wxWidth = ixmax(wxWidth, ixmax(g_fnt->StatStringDimensions(tx).x + 20, g_fnt->StatStringDimensions(tt, 22).x + 20));

        for (auto& [key, op] : ops) { 
            UIButton* b = actionButton(op.name);
            b->onClickCallback = [this, op](UIButton*) { 
                op.function();
                closePopup(); 
            };

            UILabel* kl = new UILabel(frmt("[{}]", SDL_GetScancodeName(key)), xySubtract(b->position, { 0,16 }), 12);
            kl->color = uint32ToSDLColor(0x60FFFFFF);
            wxsManager.addDrawable(kl);
            actions[key] = b;
        }
    }

    void defaultInputAction(SDL_Event evt) {
        if (evt.type == SDL_EVENT_KEY_DOWN) {
            if (actions.contains(evt.key.scancode)) {
                actions[evt.key.scancode]->click();
            }
        }
    }
};

