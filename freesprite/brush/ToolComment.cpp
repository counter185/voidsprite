#include "ToolComment.h"
#include "../PopupTextBox.h"

void ToolComment::clickPress(MainEditor* editor, XY pos)
{
	if (editor->eraserMode) {
		editor->removeCommentAt(pos);
	}
	else {
		if (editor->canAddCommentAt(pos)) {
			clickPos = pos;
			clickEditor = editor;
			PopupTextBox* textInput = new PopupTextBox("Add comment", std::format("Set comment text at {}:{}:", pos.x, pos.y), "", 440);
			textInput->setCallbackListener(EVENT_MAINEDITOR_ADD_COMMENT, this);
			g_addPopup(textInput);
		}
	}
}

void ToolComment::eventTextInputConfirm(int evt_id, std::string data)
{
	if (evt_id == EVENT_MAINEDITOR_ADD_COMMENT) {
		clickEditor->addCommentAt(clickPos, data);
	}
}
