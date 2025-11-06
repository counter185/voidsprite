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
    XY scrollMin{};
    XY scrollMax{};

    void addButtons(std::vector<UIButton*> buttons);

public:
    bool scrollable = false;

    std::function<void(PopupContextMenu*)> onExitCallback = NULL;

    PopupContextMenu(std::vector<NamedOperation> actions);
    PopupContextMenu(std::vector<UIButton*> actions);

    bool takesTouchEvents() override { return true; }
    void render() override {
        renderContextMenuBackground();
        renderDrawables();
    }
    void tick() override;
	void takeInput(SDL_Event evt) override;
    void defaultInputAction(SDL_Event evt) override;
    void playPopupCloseVFX() override;

    void renderContextMenuBackground();

    XY getContentSize();
    void setContextMenuOrigin(XY screenPos);
    void finish() {
        if (onExitCallback != NULL) {
            onExitCallback(this);
		}
        closePopup();
    }
};

inline static void g_openContextMenu(std::vector<NamedOperation> o) {
    g_addPopup(new PopupContextMenu(o));
}