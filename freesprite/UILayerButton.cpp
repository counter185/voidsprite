#include "UILayerButton.h"

void UILayerButton::eventButtonPressed(int evt_id)
{
    if (callback == NULL) {
        return;
    }
    if (evt_id == 0) {
        callback->eventGeneric(callback_id, 0, 0);
    }
    else if (evt_id == 1) {
        callback->eventGeneric(callback_id, 1, 0);
    }
}
