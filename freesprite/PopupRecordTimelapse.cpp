#include "PopupRecordTimelapse.h"
#include "UIButton.h"
#include "UIDropdown.h"
#include "UIStackPanel.h"
#include "maineditor.h"
#include "UITextField.h"
#include "io/io_avif.h"
#include "Notification.h"

PopupRecordTimelapse::PopupRecordTimelapse(MainEditor* caller) : parent(caller)
{
	wxHeight = 350;
	makeTitleAndDesc("Record timelapse", "Start recording timelapse of this editor session?");

	actionButton(TL("vsp.cmn.cancel"))->onClickCallback = [this](...) { closePopup(); };

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
		UIStackPanel::Horizontal(10, { new UILabel("Skip every n frames"), align->alignPoint(), new UINumberInputField(&skipNFrames)}),
		UIStackPanel::Horizontal(10, { new UILabel("MS per frame"), align->alignPoint(), new UINumberInputField(&msPerFrame)}),
		UIStackPanel::Horizontal(10, { new UILabel("Quality"), align->alignPoint(), qualityInputField}),
		UIStackPanel::Horizontal(10, { new UILabel("Repeat last frame"), align->alignPoint(), new UINumberInputField(&parent->timelapseRepeatLastFrame)}),
		UIStackPanel::Horizontal(10, { new UILabel("Upscale ratio"), align->alignPoint(), new UINumberInputField(&parent->timelapseUpscale)}),
	}, {10, 90}));

	actionButton(TL("vsp.cmn.confirm"))->onClickCallback = [this](...) { 
		platformTrySaveOtherFile(this, { {this->recorder.extension, this->recorder.name} }, "save timelapse", 0);
	};
}

void PopupRecordTimelapse::eventFileSaved(int evt_id, PlatformNativePathString name, int exporterIndex)
{
	auto newRec = recorder.createFn();
	newRec->startRecording(name, msPerFrame, quality);
	parent->timelapseSkipNFrames = skipNFrames;
	parent->timelapseStart(newRec, name);
	closePopup();
}
