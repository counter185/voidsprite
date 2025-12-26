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

void UndoLayersResized::undo(MainEditor* editor)
{
    for (auto& [layer, data] : storedLayerData) {
        auto oldData = layer->layerData;
        layer->layerData = data;
        layer->w = oldDimensions.x;
        layer->h = oldDimensions.y;
        layer->markLayerDirty();
        storedLayerData[layer] = oldData;
    }
    editor->canvas.dimensions = oldDimensions;
    editor->tileDimensions = oldTileSize;
}

void UndoLayersResized::redo(MainEditor* editor)
{
    for (auto& [layer, data] : storedLayerData) {
        auto oldData = layer->layerData;
        layer->layerData = data;
        layer->w = newDimensions.x;
        layer->h = newDimensions.y;
        layer->markLayerDirty();
        storedLayerData[layer] = oldData;
    }
    editor->canvas.dimensions = newDimensions;
    editor->tileDimensions = newTileSize;
}


void UndoLayerRemoved::undo(MainEditor* editor)
{
    editor->layers.insert(editor->layers.begin() + insertAt, l);
    editor->layerPicker->updateLayers();
}

void UndoLayerRemoved::redo(MainEditor* editor)
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


void UndoLayerModified::undo(MainEditor* editor)
{
    if (storedData != NULL) {
        auto& variant = l->layerData[targetVariant];
        u8* oldData = variant.pixelData;
        variant.pixelData = storedData;
        storedData = oldData;
        l->markLayerDirty();
    }
}

void UndoLayerModified::redo(MainEditor* editor)
{
    undo(editor);
}
