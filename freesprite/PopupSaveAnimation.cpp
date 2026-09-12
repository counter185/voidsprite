#include "PopupSaveAnimation.h"
#include "io/io_avif.h"
#include "Notification.h"
#include "UIDropdown.h"
#include "UIButton.h"
#include "UIStackPanel.h"
#include "UITextField.h"
#include "UILabel.h"

std::vector<AnimationRecorder> g_getAnimRecorders()
{
	return {
#if VSP_USE_LIBAVIF
		{"AVIF animation", ".avif", "AVIF animation", []() { return new AVIFVideoEncoder(); }}
#endif
	};
}

//todo: merge with popupsaveanimation somehow
//figure it out when there are more save formats than just avif
PopupSaveAnimation::PopupSaveAnimation(std::string tt, std::string tx)
{
	wxHeight = 350;
	makeTitleAndDesc(tt, tx);

	std::vector<AnimationRecorder> recorders = g_getAnimRecorders();

	if (recorders.empty()) {
		g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "No compatible video encoders"));
		return;
	}

	recorder = recorders.front();
	std::vector<std::string> recNames;
	std::transform(recorders.begin(), recorders.end(), std::back_inserter(recNames), [](auto a) { return a.name; });
	UIDropdown* recorderPicker = new UIDropdown(recNames);
	recorderPicker->onDropdownItemSelectedCallback = [this, recorders](UIDropdown* d, int i, std::string s) {
		recorder = recorders[i];
	};
	recorderPicker->text = recorder.name;

	UINumberInputField* qualityInputField = new UINumberInputField(&quality);
	qualityInputField->validateFunction = [](int v) { return v >= 0 && v <= 100; };

	HAlign* align = new HAlign();
	wxsManager.addDrawable(UIStackPanel::Vertical(3, {
		UIStackPanel::Horizontal(10, { new UILabel("Format"), recorderPicker}),
		UIStackPanel::Horizontal(10, { new UILabel("MS per frame"), align->alignPoint(), new UINumberInputField(&msPerFrame)}),
		UIStackPanel::Horizontal(10, { new UILabel("Quality"), align->alignPoint(), qualityInputField}),
		UIStackPanel::Horizontal(10, { new UILabel("Repeat count"), align->alignPoint(), new UINumberInputField(&repeatTimes)}),
		UIStackPanel::Horizontal(10, { new UILabel("Upscale ratio"), align->alignPoint(), new UINumberInputField(&scale)}),
	}, { 10, 90 }));

	actionButton(TL("vsp.cmn.cancel"))->onClickCallback = [this](...) { closePopup(); };
	actionButton(TL("vsp.cmn.confirm"))->onClickCallback = [this](...) {
		platformTrySaveOtherFile(this, { {this->recorder.extension, this->recorder.name} }, "save animation", 0);
	};
}

void PopupSaveAnimation::eventFileSaved(int evt_id, PlatformNativePathString name, int exporterIndex)
{
	if (onConfirmCallback != NULL) {
		onConfirmCallback(this, name);
	}
	closePopup();
}
