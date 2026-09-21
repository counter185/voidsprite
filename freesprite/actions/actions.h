#pragma once
#include "../globals.h"
#include "../maineditor.h"

inline std::vector<NamedEditorOperation> g_defaultEditorActions{};

void g_registerDefaultActions();

void action_editorExportUnitySpritesheet(MainEditor* editor);