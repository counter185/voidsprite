#include "PopupSetParameters.h"
#include "Panel.h"
#include "UIButton.h"

PopupSetParameters::PopupSetParameters(std::string title, ParameterStore* paramstore, ParamList* params, std::string locKeyPrefix)
{
	wxWidth = 300;
	wxHeight = 400;

	makeTitleAndDesc(title, "");

	Panel* p = paramstore->generateVerticalUI([]() {}, params, locKeyPrefix);
	p->position = {5,50};
	wxsManager.addDrawable(p);

	wxHeight = 50 + p->getDimensions().y + 80;
	wxWidth = 30 + p->getDimensions().x;

	actionButton(TL("vsp.cmn.cancel"))->onClickCallback = [this](...) {
		finish(false);
	};
	actionButton(TL("vsp.cmn.confirm"))->onClickCallback = [this](...) {
		finish(true);
	};
}

void PopupSetParameters::finish(bool r) {
	if (r) {
		if (onFinishCallback != NULL) {
			onFinishCallback();
		}
	}
	closePopup();
}
