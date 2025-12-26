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


void UndoLayerReordered::undo(MainEditor* editor)
{
    editor->layers.erase(editor->layers.begin() + newIndex);
    editor->layers.insert(editor->layers.begin() + oldIndex, l);
    editor->layerPicker->updateLayers();
}

void UndoLayerReordered::redo(MainEditor* editor)
{
    editor->layers.erase(editor->layers.begin() + oldIndex);
    editor->layers.insert(editor->layers.begin() + newIndex, l);
    editor->layerPicker->updateLayers();
}


void UndoLayerVariantCreated::undo(MainEditor* editor)
{
    storedVariant = l->layerData[variantIndex];
    l->layerData.erase(l->layerData.begin() + variantIndex);
    if (variantIndex == l->currentLayerVariant) {
        editor->layer_switchVariant(l, ixmin(variantIndex, l->layerData.size() - 1));
    }
    editor->layerPicker->updateLayers();
}

void UndoLayerVariantCreated::redo(MainEditor* editor)
{
    l->layerData.insert(l->layerData.begin() + variantIndex, storedVariant);
    if (variantIndex == l->currentLayerVariant) {
        editor->layer_switchVariant(l, ixmin(variantIndex, l->layerData.size() - 1));
    }
    editor->layerPicker->updateLayers();
}

void UndoLayerOpacityChanged::undo(MainEditor* editor)
{
    l->layerAlpha = l->lastConfirmedlayerAlpha = oldOpacity;
    editor->layerPicker->updateLayers();
}

void UndoLayerOpacityChanged::redo(MainEditor* editor)
{
    l->layerAlpha = l->lastConfirmedlayerAlpha = newOpacity;
    editor->layerPicker->updateLayers();
}

void UndoCommentAdded::undo(MainEditor* editor)
{
    auto findAtPos = std::find_if(editor->comments.begin(), editor->comments.end(),
        [this](CommentData& c) {
            return xyEqual(c.position, comment.position);
        });
    if (findAtPos != editor->comments.end()) {
        editor->comments.erase(findAtPos);
    }
}

void UndoCommentAdded::redo(MainEditor* editor)
{
    editor->comments.push_back(comment);
}
