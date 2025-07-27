#pragma once
#include "BasePopup.h"
class PopupContextMenu :
    public BasePopup
{
private:
    XY originPoint;
    XY contentSize = { 0,0 };
    Panel* itemsPanel = NULL;
    std::vector<Drawable*> items;

public:
    PopupContextMenu(std::vector<NamedOperation> actions);

    void render() override {
        renderContextMenuBackground();
        renderDrawables();
    }
    void tick() override;
    void defaultInputAction(SDL_Event evt) override {
        if (evt.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            closePopup();
        }
    }
    void playPopupCloseVFX() override;
    void renderContextMenuBackground();
};

inline static void g_openContextMenu(std::vector<NamedOperation> o) {
	g_addPopup(new PopupContextMenu(o));
}