#include "BasePopup.h"
#include "EventCallbackListener.h"

void BasePopup::closePopup() {
    g_popDisposeLastPopup(false);
    if (callback != NULL) {
        callback->eventPopupClosed(callback_id, this);
    }
    delete this;
}
