#include "UndoStack.h"
#include "maineditor.h"
#include "EditorLayerPicker.h"

void UndoLayerCreated::undo(MainEditor* editor)
{
    //remove layer from list
    auto pos = std::find(editor->layers.begin(), editor->layers.end(), l);
    if (pos != editor->layers.end()) {
        editor->layers.erase(pos);
    }
    if (editor->selLayer >= editor->layers.size()) {
        editor->switchActiveLayer(editor->layers.size() - 1);
    }
    editor->layerPicker->updateLayers();
}

void UndoLayerCreated::redo(MainEditor* editor)
{
    editor->layers.insert(editor->layers.begin() + insertAt, l);
    editor->layerPicker->updateLayers();
}
